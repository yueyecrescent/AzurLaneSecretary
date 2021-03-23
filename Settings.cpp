#include "Settings.hpp"
#include "../AMEIZ-3-1706/anm2dr/util/Utils.hpp"
#include "../AMEIZ-3-1706/anm2dr/util/ArrayList.hpp"
#include "../AMEIZ-3-1706/engine/io/KeyValueParser.hpp"
#include <math.h>

using namespace als;
using namespace anm2dr::util;
using namespace engine::io;

void Settings::load(const char* file_name)
{
	size_t str_len;
	char* settings=(char*)readBytesFromFile(file_name,&str_len);
	ArrayList<KeyValuePairStr> &key_value_pairs=parseKeyValuePairStr(settings,str_len);
	for(int i=0;i<=key_value_pairs.getLastIndex();++i)
	{
		KeyValuePairStr pair=key_value_pairs.get(i);
		if(str_eq("char_name",pair.key))
			char_name=copyCstr(pair.value);
		else if(str_eq("skin",pair.key))
			skin=copyCstr(pair.value);
		else if(str_eq("default_mode",pair.key))
			sd_on=str_eq("sd",pair.value);
		else if(str_eq("fps_limit",pair.key))
			fps_limit=parseInt(pair.value);
		else if(str_eq("main_bgm_volume",pair.key))
			main_bgm_volume=parseFloat(pair.value);
		else if(str_eq("backyard_bgm_volume",pair.key))
			backyard_bgm_volume=parseFloat(pair.value);
		else if(str_eq("ui_sound_volume",pair.key))
			ui_sound_volume=parseFloat(pair.value);
		else if(str_eq("transparent_color_filter_tolerance",pair.key))
			transparent_color_filter_tolerance=parseInt(pair.value);
	}
	free(settings);
	delete &key_value_pairs;
}
