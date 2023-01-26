#ifndef ALS_AGSYS_ALSAPP
#define ALS_AGSYS_ALSAPP

#include <AMEIZ-3-1706/engine/engine.h>
#include <AMEIZ-3-1706/engine/ext/ext.h>
#include "../Settings.hpp"
#include "../PaintingBehavior.hpp"
#include "../SDCharBehavior.hpp"
#include "../VoiceBehavior.hpp"
#include <iostream>
#include <unordered_map>

namespace als{
	namespace agsys{
		typedef struct AreaRect
		{
			float lbx,lby,rux,ruy;//lb=left-bottom,ru=right-upper.
		} AreaRect;

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
			friend void turn_left(ALSApp* app);
			friend void turn_right(ALSApp* app);
			friend void walk_left(ALSApp* app);
			friend void walk_right(ALSApp* app);
			friend void walk_up(ALSApp* app);
			friend void walk_down(ALSApp* app);
			friend void start_walking_left(ALSApp* app);
			friend void start_walking_right(ALSApp* app);
			friend void start_walking_up(ALSApp* app);
			friend void start_walking_down(ALSApp* app);
			friend void stop_walking(ALSApp* app);

			const char* current_looping_anm;
			engine::io::KeyValuePairInt current_interaction_looping_anm={nullptr,0};//key is the animation,value is the mouse button.
			int mouse_left_state=0,mouse_middle_state=0,mouse_right_state=0;//By clicking the mouse button,the state will exchange.Only used in SD char mode.

			//Restore the voice group name and number.
			std::unordered_map<std::string,int> voice_info;//In application,all voice are named like voice_1,voice_2,voice_3,...,voice_n.They are called voice_group_info.
			ArrayList<engine::core::Node<const char*,AreaRect> > painting_area_info;//Node name is rect area name.When clicking this area,the voice group with the same name will be played.If there is no voice group,then default voice(touch) will be played.
			float floating_calc_time=0,voice_calc_time=0,idle_calc_time=0,jump_calc_time=0,floating_f=0,jump_f=0;//jump_T is a half cycle.
			bmathematik::algebra::linear_algebra::Vector3f position_before_jump;
			bool is_jumping=false;
			int walking_count=0;

			als::Settings settings;
			als::PaintingBehavior painting_behavior;
			als::SDCharBehavior sdchar_behavior;
			als::VoiceBehavior voice_behavior;

			const char* sd_assets,*painting_assets,*voice_assets;
			engine::core::Sprite* painting_sprite,*sd_sprite;
			engine::graphics::text::TextRectArea* voice_text;

			engine::ext::spine::SpineSys* spine_sys;

			void window_hints();
			void preinitialize();
			void initialize();
			void terminate();
			void update(float tpf);
			//Voice
			void loadVoiceGroup(const char* voice_group_name,const char* config_group_name);
			void loadVoice(const char* voice_config);
			void randomVoice(const char *voice_grp);
			//Painting
			void loadPaintingConfig(const char* painting_config);
			//SD Char
			void addSDCharAnimation(const char* anm_name,int count);//-1 means looping.
			void randomSDCharAnimation();
			void playInteractionAnimation(int key,const char* anm_name,int anm_cycles,int& state);//state is the button state,like mouse_left_state.

			const char* spineAssetsPath(const char* char_name,int skin);
			const char* paintingAssetsPath(const char* char_name,int skin);
		};
	}
}

#endif //ALS_AGSYS_ALSAPP
