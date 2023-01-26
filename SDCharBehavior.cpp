#include "SDCharBehavior.hpp"
#include <AMEIZ-3-1706/anm2dr/util/Utils.hpp>
#include <AMEIZ-3-1706/engine/io/ValueUtils.hpp>
#include <AMEIZ-3-1706/engine/io/ArrayParser.hpp>
#include <math.h>

using namespace als;
using namespace anm2dr::util;
using namespace engine::io;

void SDCharBehavior::load(const char* file_name,int window_width,int window_height)
{
	size_t str_len;
	char* settings=(char*)readBytesFromFile(file_name,&str_len);
	ArrayList<KeyValuePairStr> &key_value_pairs=parseKeyValuePairStr(settings,str_len);
	for(int i=0;i<=key_value_pairs.getLastIndex();++i)
	{
		KeyValuePairStr pair=key_value_pairs.get(i);
		if(str_eq("sdchar_skel_format",pair.key))
		{
			if(str_eq("",pair.value))
			{
				sdchar_skel_format=nullptr;
				continue;
			}
			sdchar_skel_format=copyCstr(pair.value);
		}
		else if(str_eq("idle_anm",pair.key))
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
		else if(str_eq("enable_walking",pair.key))
			enable_walking=parseBool(pair.value);
		if(str_eq("walking_left_anm",pair.key))
		{
			if(str_eq("none",pair.value))
			{
				walking_left_anm=nullptr;
				continue;
			}
			walking_left_anm=copyCstr(pair.value);
		}
		if(str_eq("walking_right_anm",pair.key))
		{
			if(str_eq("none",pair.value))
			{
				walking_right_anm=nullptr;
				continue;
			}
			walking_right_anm=copyCstr(pair.value);
		}
		if(str_eq("walking_up_anm",pair.key))
		{
			if(str_eq("none",pair.value))
			{
				walking_up_anm=nullptr;
				continue;
			}
			walking_up_anm=copyCstr(pair.value);
		}
		if(str_eq("walking_down_anm",pair.key))
		{
			if(str_eq("none",pair.value))
			{
				walking_down_anm=nullptr;
				continue;
			}
			walking_down_anm=copyCstr(pair.value);
		}
		else if(str_eq("walking_left",pair.key))
		{
			walking_left=parseFloat(pair.value)/10*((float)window_height/window_width);
		}
		else if(str_eq("walking_right",pair.key))
		{
			walking_right=parseFloat(pair.value)/10*((float)window_height/window_width);
		}
		else if(str_eq("walking_up",pair.key))
		{
			walking_up=parseFloat(pair.value)/10;
		}
		else if(str_eq("walking_down",pair.key))
		{
			walking_down=parseFloat(pair.value)/10;
		}
		else if(str_eq("enable_auto_walking",pair.key))
			enable_auto_walking=parseBool(pair.value);
		else if(str_eq("sdchar_scale",pair.key))
			sdchar_scale=parseFloat(pair.value);
		else if(str_eq("sdchar_alpha",pair.key))
			sdchar_alpha=parseFloat(pair.value);
		else if(str_eq("premultiplied_alpha",pair.key))
			premultiplied_alpha=parseBool(pair.value);
	}
	free(settings);
	delete &key_value_pairs;
}
