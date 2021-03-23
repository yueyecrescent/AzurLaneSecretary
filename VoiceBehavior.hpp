#ifndef ALS_VOICEBEHAVIOR
#define ALS_VOICEBEHAVIOR

namespace als{
	typedef struct VoiceBehavior
	{
		bool main_voice_on=true;
		bool backyard_voice_on=false;
		float voice_interval=50;
		float voice_volume=1;
		void load(const char* file_name);
	} VoiceBehavior;
}

#endif //ALS_VOICEBEHAVIOR
