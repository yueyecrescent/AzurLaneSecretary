#ifndef ALS_SDCHARBEHAVIOR
#define ALS_SDCHARBEHAVIOR

#include <AMEIZ-3-1706/anm2dr/util/ArrayList.hpp>
#include <AMEIZ-3-1706/engine/io/KeyValueParser.hpp>

namespace als{
	typedef struct SDCharBehavior
	{
		float sdchar_scale=1;
		float sdchar_alpha=1;
		bool premultiplied_alpha=true;
		const char* sdchar_skel_format="skel";
		const char* idle_anm="stand2";
		const char* drag_anm="tuozhuai2";
		const char* left_click_anm="touch";
		int left_click_anm_cycles=1;//-1 means looping.
		const char* middle_click_anm="none";
		int middle_click_anm_cycles=0;
		const char* right_click_anm="dance";
		int right_click_anm_cycles=-1;
		anm2dr::util::ArrayList<engine::io::KeyValuePairInt> idle_random_anm;
		bool have_random_anm=false;//This cannot be config by users.
		float idle_random_anm_interval=10;
		float anm_default_mix=0.25;
		bool enable_walking=false;
		const char*walking_left_anm,*walking_right_anm,*walking_up_anm,*walking_down_anm;
		float walking_left=0,walking_right=0,walking_up=0,walking_down=0;//Related to height.
		bool enable_auto_walking=false;

		void load(const char* file_name,int window_width,int window_height);
	} SDCharBehavior;
}

#endif //ALS_SDCHARBEHAVIOR
