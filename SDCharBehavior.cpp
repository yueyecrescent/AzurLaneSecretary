#include "SDCharBehavior.hpp"
#include "../AMEIZ-3-1706/anm2dr/util/Utils.hpp"
#include "../AMEIZ-3-1706/engine/io/ArrayParser.hpp"
#include <math.h>

using namespace als;
using namespace anm2dr::util;
using namespace engine::io;

void SDCharBehavior::load(const char* file_name)
{
	size_t str_len;
	char* settings=(char*)readBytesFromFile(file_name,&str_len);
	ArrayList<KeyValuePairStr> &key_value_pairs=parseKeyValuePairStr(settings,str_len);
	for(int i=0;i<=key_value_pairs.getLastIndex();++i)
	{
		KeyValuePairStr pair=key_value_pairs.get(i);
		if(str_eq("idle_anm",pair.key))
		{
			if(str_eq("none",pair.value))
			{
				idle_anm=nullptr;
				continue;
			}
			idle_anm=copyCstr(pair.value);
		}
		else if(str_eq("drag_anm",pair.key))
			drag_anm=copyCstr(pair.value);
		else if(str_eq("left_click_anm",pair.key))
		{
			if(str_eq("none",pair.value))
			{
				left_click_anm=nullptr;
				continue;
			}
			left_click_anm=copyCstr(pair.value);
		}
		else if(str_eq("left_click_anm_cycles",pair.key))
		{
			if(str_eq("loop",pair.value))
			{
				left_click_anm_cycles=-1;
				continue;
			}
			left_click_anm_cycles=parseInt(pair.value);
		}
		else if(str_eq("middle_click_anm",pair.key))
		{
			if(str_eq("none",pair.value))
			{
				middle_click_anm=nullptr;
				continue;
			}
			middle_click_anm=copyCstr(pair.value);
		}
		else if(str_eq("middle_click_anm_cycles",pair.key))
		{
			if(str_eq("loop",pair.value))
			{
				middle_click_anm_cycles=-1;
				continue;
			}
			middle_click_anm_cycles=parseInt(pair.value);
		}
		else if(str_eq("right_click_anm",pair.key))
		{
			if(str_eq("none",pair.value))
			{
				right_click_anm=nullptr;
				continue;
			}
			right_click_anm=copyCstr(pair.value);
		}
		else if(str_eq("right_click_anm_cycles",pair.key))
		{
			if(str_eq("loop",pair.value))
			{
				right_click_anm_cycles=-1;
				continue;
			}
			right_click_anm_cycles=parseInt(pair.value);
		}
		else if(str_eq("idle_random_anm",pair.key))
		{
			if(str_eq("none",pair.value))
			{
				have_random_anm=false;
				continue;
			}
			ArrayList<const char*> anm_info=parseSingleArrayStr(pair.value);
			for(int i=0;i<=anm_info.getLastIndex();++i)
			{
				ArrayList<KeyValuePairStr> single_anm_info=parseKeyValuePairStr(anm_info[i],'#',':');
				idle_random_anm.add(KeyValuePairInt(nullptr,parseInt(single_anm_info[0].value)));
				idle_random_anm[i].key=copyCstr(single_anm_info[0].key);
			}
			have_random_anm=true;
		}
		else if(str_eq("idle_random_anm_interval",pair.key))
			idle_random_anm_interval=parseFloat(pair.value);
		else if(str_eq("anm_default_mix",pair.key))
			anm_default_mix=parseFloat(pair.value);
		else if(str_eq("enale_walking",pair.key))
			enale_walking=parseBool(pair.value);
		else if(str_eq("enable_auto_walking",pair.key))
			enable_auto_walking=parseBool(pair.value);
		else if(str_eq("sdchar_scale",pair.key))
			sdchar_scale=parseFloat(pair.value);
		else if(str_eq("sdchar_alpha",pair.key))
			sdchar_alpha=parseFloat(pair.value);
	}
	free(settings);
	delete &key_value_pairs;
}
