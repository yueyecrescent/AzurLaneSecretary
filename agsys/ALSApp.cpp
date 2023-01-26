#include "ALSApp.hpp"
#include <AMEIZ-3-1706/anm2dr/util/CStringUtils.hpp>
#include <AMEIZ-3-1706/engine/io/ValueUtils.hpp>
#include <AMEIZ-3-1706/engine/io/KeyValueParser.hpp>
#include <AMEIZ-3-1706/engine/io/ArrayParser.hpp>

#define DEBUG false

using namespace anm2dr::audio;
using namespace anm2dr::util;
using namespace anm2dr::color;
using namespace anm2dr::image;
using namespace anm2dr::font;
using namespace engine::app;
using namespace engine::core;
using namespace engine::io;
using namespace engine::graphics;
using namespace engine::audio;
using namespace engine::graphics::texture;
using namespace engine::graphics::animation;
using namespace engine::graphics::shader;
using namespace engine::graphics::text;
using namespace engine::ext::spine;
using namespace bmathematik::algebra::linear_algebra;
using namespace als::agsys;

extern ALSApp app;
//将spine的texture的黑、白色改为相近的颜色，方便透明背景
void _spine_texture_process(RasterImage* texture)
{
	//texture->replacePixelRGB({255,255,255,0},{254,254,254,0});
	texture->replacePixelRGB({0,0,0,0},{1,1,1,0});
}

namespace als{
	namespace agsys{
		void exit_als(ALSApp* app)
		{
			app->exit();
		}

		void mouse_start_drag(ALSApp* app)
		{

			if(app->settings.sd_on)
			{
				app->KeyInputSys.set_mouse_click_pos_delta(Vector3f(app->KeyInputSys.cursor_x_ndc()-app->sd_sprite->position.x,app->KeyInputSys.cursor_y_ndc()-app->sd_sprite->position.y));
				if(app->settings.ui_sound_volume>0)
					app->AudioSys.play("drag");
				if(app->sdchar_behavior.drag_anm)//Don't set current_looping_anm.It will be used when end drag.
				{
					app->spine_sys->setEmptyInstanceAnimation("sd_char");
				}
				app->spine_sys->setInstanceAnimation("sd_char",app->sdchar_behavior.drag_anm,true);
			}else if(app->settings.load_painting)
			app->KeyInputSys.set_mouse_click_pos_delta(Vector3f(app->KeyInputSys.cursor_x_ndc()-app->painting_sprite->position.x,app->KeyInputSys.cursor_y_ndc()-app->painting_sprite->position.y));
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
				if(app->voice_behavior.backyard_voice_on&&app->settings.load_voice)
					app->randomVoice("touch");
			}else{
				if(app->painting_behavior.jump_height&&!app->is_jumping)
				{
					app->is_jumping=true;//Set jump mode.
					app->position_before_jump=app->painting_sprite->position;
				}
				for(int i=0;i<=app->painting_area_info.getLastIndex();++i)
				{
					Node<const char*,AreaRect> area=app->painting_area_info[i];
					if(str_eq(app->settings.skin,split_once(area.name,'_',0,false,false)))//Find the correct area for skin.
					{
						if(app->KeyInputSys.test_cursur_in_sprite_rect(app->painting_sprite,{0.5,0.25,0},area.value.lbx,area.value.lby,area.value.rux,area.value.ruy))
							if(app->voice_behavior.main_voice_on&&app->settings.load_voice)
							{
								app->randomVoice(split_once(area.name,'_',0,false,true));
								goto END_VOICE;
							}
					}
				}
				if(app->voice_behavior.main_voice_on&&app->settings.load_voice)
					app->randomVoice("touch");
				END_VOICE:;
			}
		}

		void mouse_middle_click(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->playInteractionAnimation(MOUSE(BUTTON_MIDDLE),app->sdchar_behavior.middle_click_anm,app->sdchar_behavior.middle_click_anm_cycles,app->mouse_middle_state);
				if(app->settings.ui_sound_volume>0)
					app->AudioSys.play("click");
			}
		}

		void mouse_right_click(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->playInteractionAnimation(MOUSE(BUTTON_RIGHT),app->sdchar_behavior.right_click_anm,app->sdchar_behavior.right_click_anm_cycles,app->mouse_right_state);
				if(app->settings.ui_sound_volume>0)
					app->AudioSys.play("click");
			}
		}

		void painting_sd_exchange(ALSApp* app)
		{
			if(!(app->settings.load_painting)||!(app->settings.load_sdchar))
				return;
			if(app->settings.sd_on)
			{
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
				app->painting_sprite->position=app->sd_sprite->position;
				app->spine_sys->setInstanceVisible("sd_char",false);
				app->LR2Sys.setInstanceState(app->settings.char_name,"painting_char",true,true);
				app->AudioSys.stop("backyard_bgm");
				if(app->settings.main_bgm_volume>0)
					app->AudioSys.playLoop("main_bgm");
				if(!app->voice_behavior.main_voice_on&&app->settings.load_voice)
					app->AudioSys.stopGroup("voice");
				app->settings.sd_on=false;
				app->floating_calc_time=0;
				app->walking_count=0;
				app->addSDCharAnimation(app->sdchar_behavior.idle_anm,-1);
			}else{
				app->sd_sprite->position=app->painting_sprite->position;
				app->spine_sys->setInstanceVisible("sd_char",true);
				app->LR2Sys.setInstanceState(app->settings.char_name,"painting_char",false,false);
				app->AudioSys.stop("main_bgm");
				if(app->settings.backyard_bgm_volume>0)
					app->AudioSys.playLoop("backyard_bgm");
				if(!app->voice_behavior.backyard_voice_on&&app->settings.load_voice)
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

		void turn_left(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->spine_sys->setInstanceFlippingState("sd_char",true,false);
			}
		}

		void turn_right(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->spine_sys->setInstanceFlippingState("sd_char",false,false);
			}
		}

		void walk_left(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->sd_sprite->position-={app->sdchar_behavior.walking_left*app->get_tpf(),0,0};
			}
		}

		void walk_right(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->sd_sprite->position+={app->sdchar_behavior.walking_right*app->get_tpf(),0,0};
			}
		}

		void walk_up(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->sd_sprite->position+={0,app->sdchar_behavior.walking_up*app->get_tpf(),0};
			}
		}

		void walk_down(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				app->sd_sprite->position-={0,app->sdchar_behavior.walking_down*app->get_tpf(),0};
			}
		}

		void start_walking_left(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				if(!app->walking_count++)
					app->addSDCharAnimation(app->sdchar_behavior.walking_left_anm,-1);
				app->spine_sys->setInstanceFlippingState("sd_char",true,false);
			}
		}

		void start_walking_right(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				if(!app->walking_count++)
					app->addSDCharAnimation(app->sdchar_behavior.walking_right_anm,-1);
				app->spine_sys->setInstanceFlippingState("sd_char",false,false);
			}
		}

		void start_walking_up(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				if(!app->walking_count++)
					app->addSDCharAnimation(app->sdchar_behavior.walking_up_anm,-1);
			}
		}

		void start_walking_down(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				if(!app->walking_count++)
					app->addSDCharAnimation(app->sdchar_behavior.walking_down_anm,-1);
			}
		}

		void stop_walking(ALSApp* app)
		{
			if(app->settings.sd_on)
			{
				--app->walking_count;
				if(!app->walking_count)
				{
					app->addSDCharAnimation(app->sdchar_behavior.idle_anm,-1);
					app->mouse_left_state=0;
					app->mouse_middle_state=0;
					app->mouse_right_state=0;
				}
			}
		}
	}
}

void ALSApp::window_hints()
{

}

void ALSApp::preinitialize()
{
#if DEBUG
	getWindow().setMaximize(false);getWindow().setWindowSize(400,200);
#endif
	settings.load(rpath("settings.config"));
	sd_assets=rpath(str_join("assets/char/",settings.char_name,"/sdchar/",settings.char_name,str_eq(settings.skin,"1")?"":(str_join("_",settings.skin))));
	painting_assets=rpath(str_join("assets/char/",settings.char_name,"/painting/",settings.char_name,str_eq(settings.skin,"1")?"":(str_join("_",settings.skin))));
	voice_assets=rpath(str_join("assets/char/",settings.char_name,"/voice/"));
	painting_behavior.load(rpath("painting_behavior.config"));
	const char* sdchar_behavior_file=rpath("sdchar_behavior.config");
	if(settings.custom_sdchar_behavior)
	{
		const char* new_sdchar_behavior_file=str_join(sd_assets,"_sdchar_behavior.config");
		if(isExist(new_sdchar_behavior_file))
		{
			sdchar_behavior_file=new_sdchar_behavior_file;
			delete new_sdchar_behavior_file;
		}
	}
	sdchar_behavior.load(sdchar_behavior_file,getWindow().getWidth(),getWindow().getHeight());
	delete sdchar_behavior_file;
	voice_behavior.load(rpath("voice_behavior.config"));
	current_looping_anm=sdchar_behavior.idle_anm;
	floating_f=2*M_PI/painting_behavior.floating_time;
	jump_f=2*M_PI/painting_behavior.jump_time;
	getWindow().setBackColor({0,0,0,1});
	getWindow().setBackColorTransparentPenetrate(true);
	setFPSLimit(settings.fps_limit);
}

void ALSApp::loadVoiceGroup(const char* voice_group_name,const char* config_group_name)
{
	int iter=0;
	const char* voice_path=str_join(voice_assets,config_group_name,".wav");
	if(!isExist(voice_path))//Check if the voice is a group like main,or a single voice like login.
		iter=1;
	for(;;++iter)
	{
		if(str_eq(settings.skin,"g"))
			voice_path=str_join(voice_assets,config_group_name,iter?str_join('_',iter):"","_g.wav");
		else if(str_eq(settings.skin,"h"))
			voice_path=str_join(voice_assets,config_group_name,iter?str_join('_',iter):"","_h.wav");
		else if(str_eq(settings.skin,"1"))
			voice_path=str_join(voice_assets,config_group_name,iter?str_join('_',iter):"",".wav");
		else
			voice_path=str_join(voice_assets,config_group_name,iter?str_join('_',iter):"",str_join('_',parseInt(settings.skin)-1),".wav");
		PCMAudio *voice=nullptr;
		if(isExist(voice_path))
		{
			voice=readPCMAudio(voice_path);
			++voice_info[voice_group_name];
		}else{
			voice_path=str_join(voice_assets,config_group_name,iter?str_join('_',iter):"",".wav");
			if(isExist(voice_path))
			{
				voice=readPCMAudio(voice_path);
				++voice_info[voice_group_name];
			}else{
				return;
			}
		}
		voice->channel_layout=AV_CH_LAYOUT_MONO;
		voice->updateSampleCount();
		if(voice_behavior.voice_volume!=1)
			voice->mulVolumeFactor(voice_behavior.voice_volume);
		AudioSys.loadAudioToGroup("voice",str_join(voice_group_name,'_',voice_info[voice_group_name]),*voice);
	}
}

void ALSApp::loadVoice(const char* voice_config)
{
	if(!isExist(voice_config))
		return;
	size_t str_len;
	char* settings=(char*)readBytesFromFile(voice_config,&str_len);
	ArrayList<engine::core::Node<const char*,anm2dr::util::ArrayList<const char*> > > &voice_grps=parseSingleNamedArrayStr(settings,str_len);
	for(int gi=0;gi<=voice_grps.getLastIndex();++gi)
	{
		Node<const char*,ArrayList<const char*> > voice_grp=voice_grps.get(gi);
		ArrayList<const char*> &voice=voice_grp.value;
		for(int vi=0;vi<=voice.getLastIndex();++vi)
		{
			const char* voice_name=voice[vi];
			if(str_eq(substr(voice_name,0,4),"all_"))
				loadVoiceGroup(voice_grp.name,substr(voice_name,4));
			else
			{
				PCMAudio *voice=readPCMAudio(str_join(voice_assets,voice_name,".wav"));
				voice->channel_layout=AV_CH_LAYOUT_MONO;
				voice->updateSampleCount();
				++voice_info[voice_grp.name];
				AudioSys.loadAudioToGroup("voice",str_join(voice_grp.name,'_',voice_info[voice_grp.name]),*voice);
			}
		}
	}
}

void ALSApp::randomVoice(const char *voice_grp)
{
	AudioSys.play("voice",str_join(voice_grp,'_',random(1,voice_info[voice_grp])),AUDIO_IGNORE);
}

void ALSApp::loadPaintingConfig(const char* painting_config)
{
	int pwidth=painting_sprite->robj->getColorMap(0).getImageWidth(),pheight=painting_sprite->robj->getColorMap(0).getImageHeight();
	if(isExist(painting_config))
	{
		size_t str_len;
		char* settings=(char*)readBytesFromFile(painting_config,&str_len);
		ArrayList<engine::core::Node<const char*,anm2dr::util::ArrayList<const char*> > > &painting_area_grps=parseSingleNamedArrayStr(settings,str_len);
		for(int gi=0;gi<=painting_area_grps.getLastIndex();++gi)
		{
			Node<const char*,ArrayList<const char*> > painting_area_grp=painting_area_grps.get(gi);
			ArrayList<const char*> &area=painting_area_grp.value;
			painting_area_info.add(Node<const char*,AreaRect>(painting_area_grp.name,{(float)parseInt(area[0])/pwidth,(float)(pheight-parseInt(area[1]))/pheight,(float)parseInt(area[2])/pwidth,(float)(pheight-parseInt(area[3]))/pheight}));
		}
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
	if(settings.load_painting)
	{
		Texture2D painting_texture(str_join(painting_assets,".png"));
		//painting_texture.image->replacePixelRGB({255,255,255,0},{254,254,254,0});
		painting_texture.image->replacePixelRGB({0,0,0,0},{1,1,1,0});
		painting_sprite=new Sprite(painting_texture,0.001*painting_behavior.painting_scale);
		loadPaintingConfig(rpath(str_join("assets/char/",settings.char_name,"/painting/painting.config")));
		LR2Sys.addObject(painting_sprite->robj,settings.char_name,{0.5,0.25,0});
		LR2Sys.newInstance(settings.char_name,"painting_char",new SingleRInstanceInfo());
		LR2Sys.bindPObjectInfo(settings.char_name,"painting_char",painting_sprite);
	}

	if(settings.load_sdchar)
	{
		sd_sprite=new Sprite();
		spine_sys=new SpineSys(&(LR2Sys.LR1Sys));
		spine_sys->loadSpineObject(settings.char_name,str_join(sd_assets,".atlas"),str_join(sd_assets,sdchar_behavior.sdchar_skel_format?str_join(".",sdchar_behavior.sdchar_skel_format):""),0.002*sdchar_behavior.sdchar_scale);
		spine_sys->newInstance(settings.char_name,"sd_char");
		spine_sys->bindPObjectInfo("sd_char",sd_sprite);
		if(sdchar_behavior.idle_anm)
			spine_sys->setInstanceAnimation("sd_char",sdchar_behavior.idle_anm);
		spine_sys->setInstanceAnimationDefaultMixTime("sd_char",sdchar_behavior.anm_default_mix);
		spine_sys->setInstancePremultipliedAlpha("sd_char",sdchar_behavior.premultiplied_alpha);
		spine_sys->getShader().set_uniform("premultiplied_alpha",sdchar_behavior.premultiplied_alpha);
	}
#if DEBUG
	std::cout<<"Painting Shader Info:"<<std::endl;
	LR2Sys.LR1Sys.default_shader.checkError();
	std::cout<<"Spine Shader Info:"<<std::endl;
	spine_sys->vertex_data.shader->checkError();
#endif
/*
	voice_text=new TextRectArea();
	voice_text->shader=&(LR2Sys.LR1Sys.text_shader);
	voice_text->setFont(new Font(rpath(str_join("assets/fonts/","test.otf")),24));
	setlocale(LC_ALL,"chs");
	voice_text->setText(L"娴嬭瘯");
	LR2Sys.addObject(voice_text,"text",{0.5,0.25,0});
	LR2Sys.newInstance("text","voice_text",new SingleRInstanceInfo());
	LR2Sys.bindPObjectInfo("text","voice_text",painting_sprite);
	LR2Sys.setInstanceState("text","voice_text",true,true);
*/
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

	if(settings.load_voice)
	{
		AudioSys.createGroup("voice");
		//Load voice
		loadVoice(str_join(voice_assets,"voice.config"));
	}

	KeyInputSys.bind_key(GLFW_MOUSE_BUTTON_LEFT ,MOUSE_EVENT_START_DRAG ,Mouse,mouse_start_drag);
	KeyInputSys.bind_key(GLFW_MOUSE_BUTTON_LEFT ,MOUSE_EVENT_END_DRAG ,Mouse,mouse_end_drag);
	KeyInputSys.bind_key(GLFW_MOUSE_BUTTON_LEFT ,MOUSE_EVENT_CLICK ,Mouse,mouse_left_click);
	KeyInputSys.bind_key(GLFW_KEY_TAB,KEYBOARD_EVENT_HIT,Keyboard,painting_sd_exchange);
	KeyInputSys.bind_key(GLFW_KEY_ESCAPE,KEYBOARD_EVENT_HIT,Keyboard,exit_als);
	KeyInputSys.bind_key(GLFW_KEY_PAGE_UP,KEYBOARD_EVENT_ON_PRESS,Keyboard,alpha_inc);
	KeyInputSys.bind_key(GLFW_KEY_PAGE_DOWN,KEYBOARD_EVENT_ON_PRESS,Keyboard,alpha_dec);


	if(sdchar_behavior.enable_walking)
	{
		KeyInputSys.bind_key(GLFW_KEY_LEFT,KEYBOARD_EVENT_START_PRESS,Keyboard,start_walking_left);
		KeyInputSys.bind_key(GLFW_KEY_RIGHT,KEYBOARD_EVENT_START_PRESS,Keyboard,start_walking_right);
		KeyInputSys.bind_key(GLFW_KEY_UP,KEYBOARD_EVENT_START_PRESS,Keyboard,start_walking_up);
		KeyInputSys.bind_key(GLFW_KEY_DOWN,KEYBOARD_EVENT_START_PRESS,Keyboard,start_walking_down);
		KeyInputSys.bind_key(GLFW_KEY_LEFT,KEYBOARD_EVENT_ON_PRESS,Keyboard,walk_left);
		KeyInputSys.bind_key(GLFW_KEY_RIGHT,KEYBOARD_EVENT_ON_PRESS,Keyboard,walk_right);
		KeyInputSys.bind_key(GLFW_KEY_UP,KEYBOARD_EVENT_ON_PRESS,Keyboard,walk_up);
		KeyInputSys.bind_key(GLFW_KEY_DOWN,KEYBOARD_EVENT_ON_PRESS,Keyboard,walk_down);
		KeyInputSys.bind_key(GLFW_KEY_LEFT,KEYBOARD_EVENT_END_PRESS,Keyboard,stop_walking);
		KeyInputSys.bind_key(GLFW_KEY_RIGHT,KEYBOARD_EVENT_END_PRESS,Keyboard,stop_walking);
		KeyInputSys.bind_key(GLFW_KEY_UP,KEYBOARD_EVENT_END_PRESS,Keyboard,stop_walking);
		KeyInputSys.bind_key(GLFW_KEY_DOWN,KEYBOARD_EVENT_END_PRESS,Keyboard,stop_walking);
	}else{
		KeyInputSys.bind_key(GLFW_KEY_LEFT,KEYBOARD_EVENT_START_PRESS,Keyboard,turn_left);
		KeyInputSys.bind_key(GLFW_KEY_RIGHT,KEYBOARD_EVENT_START_PRESS,Keyboard,turn_right);
	}

	if(sdchar_behavior.middle_click_anm)
		KeyInputSys.bind_key(GLFW_MOUSE_BUTTON_MIDDLE ,MOUSE_EVENT_CLICK ,Mouse,mouse_middle_click);
	if(sdchar_behavior.right_click_anm)
		KeyInputSys.bind_key(GLFW_MOUSE_BUTTON_RIGHT ,MOUSE_EVENT_CLICK ,Mouse,mouse_right_click);

	//Rendering
	if(settings.sd_on)
	{
		if(settings.load_sdchar)
		{
			spine_sys->setInstanceVisible("sd_char",true);
		}
		if(settings.load_painting)
			LR2Sys.setInstanceState(settings.char_name,"painting_char",false,false);
		if(settings.backyard_bgm_volume>0)
			AudioSys.playLoop("backyard_bgm");
		if(voice_behavior.backyard_voice_on&&settings.load_voice)
			AudioSys.play("voice",str_join("login_",random(1,voice_info["login"])),AUDIO_IGNORE);
	}else{
		if(settings.load_sdchar)
			spine_sys->setInstanceVisible("sd_char",false);
		if(settings.load_painting)
		{
			LR2Sys.setInstanceState(settings.char_name,"painting_char",true,true);
		}
		if(settings.main_bgm_volume>0)
			AudioSys.playLoop("main_bgm");
		if(voice_behavior.main_voice_on&&settings.load_voice)
			AudioSys.play("voice",str_join("login_",random(1,voice_info["login"])),AUDIO_IGNORE);
	}
}

void ALSApp::terminate()
{

}

void ALSApp::update(float tpf)
{
	if(settings.load_voice)
	{
		if((voice_behavior.main_voice_on&&!settings.sd_on)||(voice_behavior.backyard_voice_on&&settings.sd_on))
		{
			voice_calc_time+=tpf;
			if(voice_calc_time>voice_behavior.voice_interval)
			{
				randomVoice("main");
				voice_calc_time-=voice_behavior.voice_interval;
			}
		}
	}
	if(KeyInputSys.is_mouse_on_drag(GLFW_MOUSE_BUTTON_LEFT))//Dragging
	{
		if(settings.load_sdchar)
			sd_sprite->position={KeyInputSys.cursor_x_ndc()-KeyInputSys.mouse_click_pos_delta().x,KeyInputSys.cursor_y_ndc()-KeyInputSys.mouse_click_pos_delta().y,0};
		if(settings.load_painting)
			painting_sprite->position={KeyInputSys.cursor_x_ndc()-KeyInputSys.mouse_click_pos_delta().x,KeyInputSys.cursor_y_ndc()-KeyInputSys.mouse_click_pos_delta().y,0};
	}else{//Standing
		if(!settings.sd_on&&settings.load_painting)//Only on painting mode.
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
			if(settings.load_sdchar)
			{
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
	}
	if(settings.sd_on&&settings.load_sdchar)
	{
		spine_sys->getShader().set_uniform("alpha_factor",sdchar_behavior.sdchar_alpha);
		spine_sys->update(tpf);
	}
	if((!settings.sd_on)&&settings.load_painting)
	{
		painting_sprite->update_dynamics(tpf);
		LR2Sys.LR1Sys.default_shader.set_uniform("alpha_factor",painting_behavior.painting_alpha);
	}
}
