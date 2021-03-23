#include "ALSApp.hpp"
#include "../../AMEIZ-3-1706/anm2dr/util/Utils.hpp"
#include "../../AMEIZ-3-1706/engine/io/KeyValueParser.hpp"

using namespace anm2dr::audio;
using namespace anm2dr::util;
using namespace anm2dr::color;
using namespace engine::app;
using namespace engine::core;
using namespace engine::io;
using namespace engine::graphics;
using namespace engine::audio;
using namespace engine::graphics::texture;
using namespace engine::graphics::animation;
using namespace engine::graphics::shader;
using namespace engine::ext::spine;
using namespace bmathematik::algebra::linear_algebra;
using namespace als::agsys;

extern ALSApp app;

namespace als{
	namespace agsys{
		void exit_als(ALSApp* app)
		{
			app->exit();
		}

		void mouse_start_drag(ALSApp* app)
		{
			app->KeyInputSys.set_mouse_click_pos_delta(Vector3f(app->KeyInputSys.cursor_x_ndc()-app->painting_sprite->position.x,app->KeyInputSys.cursor_y_ndc()-app->painting_sprite->position.y));
			if(app->settings.sd_on)
			{
				if(app->settings.ui_sound_volume>0)
					app->AudioSys.play("drag");
				if(app->sdchar_behavior.drag_anm)//Don't set current_looping_anm.It will be used when end drag.
				{
					app->spine_sys->setEmptyInstanceAnimation("sd_char");
				}
				app->spine_sys->setInstanceAnimation("sd_char",app->sdchar_behavior.drag_anm,true);
			}
		}

		void mouse_end_drag(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				if(app->settings.ui_sound_volume>0)
					app->AudioSys.play("drag");
				app->addSDCharAnimation(app->current_looping_anm,-1);
			}
		}

		void mouse_left_click(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->playInteractionAnimation(MOUSE(BUTTON_LEFT),app->sdchar_behavior.left_click_anm,app->sdchar_behavior.left_click_anm_cycles,app->mouse_left_state);
				if(app->settings.ui_sound_volume>0)
					app->AudioSys.play("click");
				if(app->voice_behavior.backyard_voice_on)
					app->AudioSys.play("voice",str_join("touch_",random(1,app->voice_touch_num)),AUDIO_IGNORE);
			}else{
				if(app->painting_behavior.jump_height&&!app->is_jumping)
				{
					app->is_jumping=true;//Set jump mode.
					app->position_before_jump=app->painting_sprite->position;
				}
				if(app->voice_behavior.main_voice_on)
					app->AudioSys.play("voice",str_join("touch_",random(1,app->voice_touch_num)),AUDIO_IGNORE);
			}
		}

		void mouse_middle_click(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->playInteractionAnimation(MOUSE(BUTTON_MIDDLE),app->sdchar_behavior.middle_click_anm,app->sdchar_behavior.middle_click_anm_cycles,app->mouse_middle_state);
				if(app->settings.ui_sound_volume>0)
					app->AudioSys.play("click");
				if(app->voice_behavior.backyard_voice_on)
					app->AudioSys.play("voice",str_join("touch_",random(1,app->voice_touch_num)),AUDIO_IGNORE);
			}
		}

		void mouse_right_click(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->playInteractionAnimation(MOUSE(BUTTON_RIGHT),app->sdchar_behavior.right_click_anm,app->sdchar_behavior.right_click_anm_cycles,app->mouse_right_state);
				if(app->settings.ui_sound_volume>0)
					app->AudioSys.play("click");
				if(app->voice_behavior.backyard_voice_on)
					app->AudioSys.play("voice",str_join("touch_",random(1,app->voice_touch_num)),AUDIO_IGNORE);
			}
		}

		void painting_sd_exchange(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->spine_sys->setInstanceVisible("sd_char",false);
				app->L2Sys.setInstanceState(app->settings.char_name,"painting_char",true,true);
				app->AudioSys.stop("backyard_bgm");
				if(app->settings.main_bgm_volume>0)
					app->AudioSys.playLoop("main_bgm");
				if(!app->voice_behavior.main_voice_on)
					app->AudioSys.stopGroup("voice");
				app->settings.sd_on=false;
				app->floating_calc_time=0;
			}else{
				app->spine_sys->setInstanceVisible("sd_char",true);
				app->L2Sys.setInstanceState(app->settings.char_name,"painting_char",false,false);
				app->AudioSys.stop("main_bgm");
				if(app->settings.backyard_bgm_volume>0)
					app->AudioSys.playLoop("backyard_bgm");
				if(!app->voice_behavior.backyard_voice_on)
					app->AudioSys.stopGroup("voice");
				app->settings.sd_on=true;
				app->floating_calc_time=0;
			}
		}

		void alpha_inc(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->sdchar_behavior.sdchar_alpha+=0.005;
				if(app->sdchar_behavior.sdchar_alpha>1)
					app->sdchar_behavior.sdchar_alpha=1;
			}else{
				app->painting_behavior.painting_alpha+=0.005;
				if(app->painting_behavior.painting_alpha>1)
					app->painting_behavior.painting_alpha=1;
			}
		}

		void alpha_dec(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->sdchar_behavior.sdchar_alpha-=0.005;
				if(app->sdchar_behavior.sdchar_alpha<0.005)//If you set it as 0,the window will lose focus.
					app->sdchar_behavior.sdchar_alpha=0.005;
			}else{
				app->painting_behavior.painting_alpha-=0.005;
				if(app->painting_behavior.painting_alpha<0.005)
					app->painting_behavior.painting_alpha=0.005;
			}
		}
	}
}

void ALSApp::window_hints()
{

}

void ALSApp::preinitialize()
{//getWindow().setMaximize(false);getWindow().setWindowSize(400,200);
	settings.load(rpath("settings.config"));
	painting_behavior.load(rpath("painting_behavior.config"));
	sdchar_behavior.load(rpath("sdchar_behavior.config"));
	voice_behavior.load(rpath("voice_behavior.config"));
	current_looping_anm=sdchar_behavior.idle_anm;
	floating_f=2*M_PI/painting_behavior.floating_time;
	jump_f=2*M_PI/painting_behavior.jump_time;
	sd_assets=rpath(str_join("assets/char/",settings.char_name,"/sdchar/",settings.char_name,str_eq(settings.skin,"1")?"":(str_join("_",settings.skin))));
	painting_assets=rpath(str_join("assets/char/",settings.char_name,"/painting/",settings.char_name,str_eq(settings.skin,"1")?"":(str_join("_",settings.skin))));
	voice_assets=rpath(str_join("assets/char/",settings.char_name,"/voice/"));
	getWindow().setBackColor(ColorRGBA(anm2dr::image::getAvailableGrayScale(new const char*[2]{str_join(sd_assets,".png"),str_join(painting_assets,".png")},2,settings.transparent_color_filter_tolerance),0));
	getWindow().setBackColorTransparentPenetrate(true);
	setFPSLimit(settings.fps_limit);
}

void ALSApp::loadVoiceGroup(const char* voice_group_name,int *loaded_voice_num,const char* audiosys_group_name)
{
	for(int i=0;;++i)
	{
		const char* voice_path=nullptr;
		if(str_eq(settings.skin,"g"))
			voice_path=str_join(voice_assets,voice_group_name,i?str_join('_',i):"","_g.wav");
		else if(str_eq(settings.skin,"h"))
			voice_path=str_join(voice_assets,voice_group_name,i?str_join('_',i):"","_h.wav");
		else if(str_eq(settings.skin,"1"))
			voice_path=str_join(voice_assets,voice_group_name,i?str_join('_',i):"",".wav");
		else
			voice_path=str_join(voice_assets,voice_group_name,i?str_join('_',i):"",str_join('_',parseInt(settings.skin)-1),".wav");
		PCMAudio *voice=nullptr;
		if(isExist(voice_path))
		{
			voice=readPCMAudio(voice_path);
			++*loaded_voice_num;
		}else{
			voice_path=str_join(voice_assets,voice_group_name,i?str_join('_',i):"",".wav");
			if(isExist(voice_path))
			{
				voice=readPCMAudio(voice_path);
				++*loaded_voice_num;
			}else
				return;
		}
		voice->channel_layout=AV_CH_LAYOUT_MONO;
		voice->updateSampleCount();
		if(voice_behavior.voice_volume!=1)
			voice->mulVolumeFactor(voice_behavior.voice_volume);
		AudioSys.loadAudioToGroup("voice",str_join(audiosys_group_name,'_',*loaded_voice_num),*voice);
	}
}

void ALSApp::addSDCharAnimation(const char* anm_name,int count)
{
	if(count==-1)
	{
		if(!anm_name)
		{
			spine_sys->setEmptyInstanceAnimation("sd_char");
			current_looping_anm=anm_name;
			return;
		}
		spine_sys->setInstanceAnimation("sd_char",anm_name,true);
		current_looping_anm=anm_name;
		return;
	}
	spine_sys->insertAnimationToLoopAnimationI("sd_char",current_looping_anm,anm_name,count);
}

void ALSApp::randomSDCharAnimation()
{
	int random_index=random(0,sdchar_behavior.idle_random_anm.getLastIndex());
	const char* anm_name=sdchar_behavior.idle_random_anm[random_index].key;
	int anm_count=sdchar_behavior.idle_random_anm[random_index].value;
	spine_sys->insertAnimationToLoopAnimationI("sd_char",current_looping_anm,anm_name,anm_count);
	idle_calc_time-=spine_sys->getDurationOfAnimation(settings.char_name,anm_name)*anm_count;
}

void ALSApp::playInteractionAnimation(int key,const char* anm_name,int anm_cycles,int& state)
{
	if(anm_name)//If the interaction is a none empty animation.
	{
		if(anm_cycles==-1)//Looping animation.
		{
			state=~state;
			if(state)//Cover the current looping interaction animation.
			{
				current_interaction_looping_anm.key=anm_name;
				current_interaction_looping_anm.value=key;
			}else{
				if(current_interaction_looping_anm.value==key)
					current_interaction_looping_anm.key=sdchar_behavior.idle_anm;
			}
			addSDCharAnimation(current_interaction_looping_anm.key,-1);
			return;
		}
		addSDCharAnimation(anm_name,anm_cycles);//Normal animation.
	}
}

void ALSApp::initialize()
{
	sd_sprite=new Sprite();
	spine_sys=new SpineSys(&(L2Sys.L1Sys));
	spine_sys->loadSpineObject(settings.char_name,str_join(sd_assets,".atlas"),str_join(sd_assets,".skel"),0.002*sdchar_behavior.sdchar_scale);
	spine_sys->newInstance(settings.char_name,"sd_char");
	spine_sys->bindPObjectInfo("sd_char",sd_sprite);
	if(sdchar_behavior.idle_anm)
		spine_sys->setInstanceAnimation("sd_char",sdchar_behavior.idle_anm);
	spine_sys->setInstanceAnimationDefaultMixTime("sd_char",sdchar_behavior.anm_default_mix);

	painting_sprite=new Sprite(str_join(painting_assets,".png"),0.001*painting_behavior.painting_scale);
	L2Sys.addObject(painting_sprite->robj,settings.char_name,{0.5,0.25,0.5});
	L2Sys.newInstance(settings.char_name,"painting_char",new SingleRInstanceInfo());
	L2Sys.bindPObjectInfo(settings.char_name,"painting_char",painting_sprite);

	PCMAudio *bgm_main=readPCMAudio(rpath("assets/sound/bgm/main.wav"));
	bgm_main->channel_layout=AV_CH_LAYOUT_STEREO;
	bgm_main->updateSampleCount();
	if(settings.main_bgm_volume!=1)
		bgm_main->mulVolumeFactor(settings.main_bgm_volume);
	AudioSys.loadAudio("main_bgm",*bgm_main);

	PCMAudio *bgm_backyard=readPCMAudio(rpath("assets/sound/bgm/backyard.wav"));
	bgm_backyard->channel_layout=AV_CH_LAYOUT_STEREO;
	bgm_backyard->updateSampleCount();
	if(settings.backyard_bgm_volume!=1)
		bgm_backyard->mulVolumeFactor(settings.backyard_bgm_volume);
	AudioSys.loadAudio("backyard_bgm",*bgm_backyard);

	PCMAudio *click=readPCMAudio(rpath("assets/sound/ui/ui-boat_click.wav"));
	click->channel_layout=AV_CH_LAYOUT_MONO;
	click->updateSampleCount();
	if(settings.ui_sound_volume!=1)
		click->mulVolumeFactor(settings.ui_sound_volume);
	AudioSys.loadAudio("click",*click);

	PCMAudio *drag=readPCMAudio(rpath("assets/sound/ui/ui-boat_drag.wav"));
	drag->channel_layout=AV_CH_LAYOUT_MONO;
	drag->updateSampleCount();
	if(settings.ui_sound_volume!=1)
		drag->mulVolumeFactor(settings.ui_sound_volume);
	AudioSys.loadAudio("drag",*drag);

	AudioSys.createGroup("voice");

	//Load voice
	loadVoiceGroup("login",&voice_login_num,"login");
	loadVoiceGroup("main",&voice_main_num,"main");
	loadVoiceGroup("touch",&voice_touch_num,"touch");

	KeyInputSys.bind_key(GLFW_MOUSE_BUTTON_LEFT ,MOUSE_EVENT_START_DRAG ,Mouse,mouse_start_drag);
	KeyInputSys.bind_key(GLFW_MOUSE_BUTTON_LEFT ,MOUSE_EVENT_END_DRAG ,Mouse,mouse_end_drag);
	KeyInputSys.bind_key(GLFW_MOUSE_BUTTON_LEFT ,MOUSE_EVENT_CLICK ,Mouse,mouse_left_click);
	KeyInputSys.bind_key(GLFW_KEY_TAB,KEYBOARD_EVENT_HIT,Keyboard,painting_sd_exchange);
	KeyInputSys.bind_key(GLFW_KEY_ESCAPE,KEYBOARD_EVENT_HIT,Keyboard,exit_als);
	KeyInputSys.bind_key(GLFW_KEY_PAGE_UP,KEYBOARD_EVENT_ON_PRESS,Keyboard,alpha_inc);
	KeyInputSys.bind_key(GLFW_KEY_PAGE_DOWN,KEYBOARD_EVENT_ON_PRESS,Keyboard,alpha_dec);

	if(sdchar_behavior.middle_click_anm)
		KeyInputSys.bind_key(GLFW_MOUSE_BUTTON_MIDDLE ,MOUSE_EVENT_CLICK ,Mouse,mouse_middle_click);
	if(sdchar_behavior.right_click_anm)
		KeyInputSys.bind_key(GLFW_MOUSE_BUTTON_RIGHT ,MOUSE_EVENT_CLICK ,Mouse,mouse_right_click);

	if(settings.sd_on)
	{
		spine_sys->setInstanceVisible("sd_char",true);
		L2Sys.setInstanceState(settings.char_name,"painting_char",false,false);
		if(settings.backyard_bgm_volume>0)
			AudioSys.playLoop("backyard_bgm");
	}else{
		spine_sys->setInstanceVisible("sd_char",false);
		L2Sys.setInstanceState(settings.char_name,"painting_char",true,true);
		if(settings.main_bgm_volume>0)
			AudioSys.playLoop("main_bgm");
		if(voice_behavior.main_voice_on)
			AudioSys.play("voice",str_join("login_",random(1,voice_login_num)),AUDIO_IGNORE);
	}
}

void ALSApp::terminate()
{

}

void ALSApp::update(float tpf)
{

	if((voice_behavior.main_voice_on&&!settings.sd_on)||(voice_behavior.backyard_voice_on&&settings.sd_on))
	{
		voice_calc_time+=tpf;
		if(voice_calc_time>voice_behavior.voice_interval)
		{
			AudioSys.play("voice",str_join("main_",random(1,voice_main_num)),AUDIO_IGNORE);
			voice_calc_time-=voice_behavior.voice_interval;
		}
	}
	if(KeyInputSys.is_mouse_on_drag(GLFW_MOUSE_BUTTON_LEFT))//Dragging
	{
		sd_sprite->position={KeyInputSys.cursor_x_ndc()-KeyInputSys.mouse_click_pos_delta().x,KeyInputSys.cursor_y_ndc()-KeyInputSys.mouse_click_pos_delta().y,0};
		painting_sprite->position=sd_sprite->position;
	}else{//Standing
		if(!settings.sd_on)//Only on painting mode.
		{
			if(is_jumping)
			{
				jump_calc_time+=tpf;
				if(jump_calc_time>painting_behavior.jump_time)
				{
					jump_calc_time=0;
					is_jumping=false;
					painting_sprite->position=position_before_jump;
				}else
					painting_sprite->position+={0,painting_behavior.jump_height*sin(jump_f*jump_calc_time),0};
			}else{
				if(painting_behavior.floating_amplitude)
				{
					floating_calc_time+=tpf;
					if(floating_calc_time>painting_behavior.floating_time)
						floating_calc_time-=painting_behavior.floating_time;
					painting_sprite->position+={0,painting_behavior.floating_amplitude*sin(floating_f*floating_calc_time),0};
				}
			}
		}else{//For SD char mode.
			if(sdchar_behavior.have_random_anm&&str_eq(sdchar_behavior.idle_anm,current_looping_anm))//When char is idling.
			{
				idle_calc_time+=tpf;
				if(idle_calc_time>sdchar_behavior.idle_random_anm_interval)//Random idle animation.
				{
					idle_calc_time-=sdchar_behavior.idle_random_anm_interval;
					randomSDCharAnimation();
				}
			}
		}
	}
	L2Sys.L1Sys.shader.set_uniform("alpha_factor",sdchar_behavior.sdchar_alpha);
	spine_sys->update(tpf);
	painting_sprite->update_dynamics(tpf);
	L2Sys.L1Sys.shader.set_uniform("alpha_factor",painting_behavior.painting_alpha);
}
