#ifndef ALS_SETTINGS
#define ALS_SETTINGS

namespace als{
	typedef struct Settings
	{
		const char* char_name="lafei";
		const char* skin="1";//1 is default.
		int sd_on=false;
		bool custom_sdchar_behavior=false;
		bool load_painting=true;
		bool load_sdchar=true;
		bool load_voice=true;
		float main_bgm_volume=1;
		float backyard_bgm_volume=1;
		float ui_sound_volume=1;
		int fps_limit=30;

		void load(const char* file_name);
	} Settings;
}

#endif //ALS_SETTINGS
