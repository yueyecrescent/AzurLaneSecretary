#include "PaintingBehavior.hpp"
#include <AMEIZ-3-1706/anm2dr/util/Utils.hpp>
#include <AMEIZ-3-1706/anm2dr/util/ArrayList.hpp>
#include <AMEIZ-3-1706/engine/io/ValueUtils.hpp>
#include <AMEIZ-3-1706/engine/io/KeyValueParser.hpp>
#include <math.h>

using namespace als;
using namespace anm2dr::util;
using namespace engine::io;

void PaintingBehavior::load(const char* file_name)
{
	size_t str_len;
	char* settings=(char*)readBytesFromFile(file_name,&str_len);
	ArrayList<KeyValuePairStr> &key_value_pairs=parseKeyValuePairStr(settings,str_len);
	for(int i=0;i<=key_value_pairs.getLastIndex();++i)
	{
		KeyValuePairStr pair=key_value_pairs.get(i);
		if(str_eq("floating_amplitude",pair.key))
			floating_amplitude=parseFloat(pair.value)/100;
		else if(str_eq("floating_time",pair.key))
			floating_time=parseFloat(pair.value);
		else if(str_eq("jump_height",pair.key))
			jump_height=parseFloat(pair.value)/100;
		else if(str_eq("jump_time",pair.key))
			jump_time=parseFloat(pair.value);
		else if(str_eq("painting_scale",pair.key))
			painting_scale=parseFloat(pair.value);
		else if(str_eq("painting_alpha",pair.key))
			painting_alpha=parseFloat(pair.value);
	}
	free(settings);
	delete &key_value_pairs;
}
