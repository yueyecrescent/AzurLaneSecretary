#ifndef ALS_SETTINGS
#define ALS_SETTINGS

namespace als{
	typedef struct Settings
	{
		const char* char_name="lafei";
		const char* skin="1";//1 is default.
		int sd_on=false;
		float main_bgm_volume=1;
		float backyard_bgm_volume=1;
		float ui_sound_volume=1;
		int fps_limit=30;
		int transparent_color_filter_tolerance=0;

		void load(const char* file_name);
	} Settings;
}

#endif //ALS_SETTINGS
