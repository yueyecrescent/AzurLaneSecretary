#ifndef ALS_AGSYS_ALSAPP
#define ALS_AGSYS_ALSAPP

#include "../../AMEIZ-3-1706/engine/engine.h"
#include "../../AMEIZ-3-1706/engine/ext/ext.h"
#include "../Settings.hpp"
#include "../PaintingBehavior.hpp"
#include "../SDCharBehavior.hpp"
#include "../VoiceBehavior.hpp"
#include <iostream>
#include <unordered_map>

namespace als{
	namespace agsys{
		class ALSApp:public engine::app::DesktopRenderer<ALSApp>
		{
		public:
			friend void exit_als(ALSApp* app);
			friend void mouse_start_drag(ALSApp* app);
			friend void mouse_left_click(ALSApp* app);
			friend void mouse_middle_click(ALSApp* app);
			friend void mouse_right_click(ALSApp* app);
			friend void mouse_end_drag(ALSApp* app);
			friend void painting_sd_exchange(ALSApp* app);
			friend void alpha_inc(ALSApp* app);
			friend void alpha_dec(ALSApp* app);

			const char* current_looping_anm;
			engine::io::KeyValuePairInt current_interaction_looping_anm={nullptr,0};//key is the animation,value is the mouse button.
			int mouse_left_state=0,mouse_middle_state=0,mouse_right_state=0;//By clicking the mouse button,the state will exchange.Only used in SD char mode.

			int voice_login_num=0,voice_main_num=0,voice_touch_num=0;//In application,all voice are named like voice_1,voice_2,voice_3,...,voice_n.
			float floating_calc_time=0,voice_calc_time=0,idle_calc_time=0,jump_calc_time=0,floating_f=0,jump_f=0;//jump_T is a half cycle.
			bmathematik::algebra::linear_algebra::Vector3f position_before_jump;
			bool is_jumping=false;

			als::Settings settings;
			als::PaintingBehavior painting_behavior;
			als::SDCharBehavior sdchar_behavior;
			als::VoiceBehavior voice_behavior;

			const char* sd_assets,*painting_assets,*voice_assets;
			engine::core::Sprite* painting_sprite,*sd_sprite;

			engine::ext::spine::SpineSys* spine_sys;

			void window_hints();
			void preinitialize();
			void initialize();
			void terminate();
			void update(float tpf);
			void loadVoiceGroup(const char* voice_group_name,int *loaded_voice_num,const char* audiosys_group_name);
			void addSDCharAnimation(const char* anm_name,int count);//-1 means looping.
			void randomSDCharAnimation();
			void playInteractionAnimation(int key,const char* anm_name,int anm_cycles,int& state);//state is the button state,like mouse_left_state.

			const char* spineAssetsPath(const char* char_name,int skin);
			const char* paintingAssetsPath(const char* char_name,int skin);
		};
	}
}

#endif //ALS_AGSYS_ALSAPP
