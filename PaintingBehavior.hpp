#ifndef ALS_PAINTINGBEHAVIOR
#define ALS_PAINTINGBEHAVIOR

namespace als{
	typedef struct PaintingBehavior
	{
		float painting_scale=1;
		float painting_alpha=1;
		float floating_amplitude=0.05/100;
		float floating_time=8;
		float jump_height=1/100;
		float jump_time=0.3;
		void load(const char* file_name);
	} PaintingBehavior;
}

#endif //ALS_PAINTINGBEHAVIOR
