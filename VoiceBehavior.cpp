#include "VoiceBehavior.hpp"
#include <AMEIZ-3-1706/anm2dr/util/Utils.hpp>
#include <AMEIZ-3-1706/anm2dr/util/ArrayList.hpp>
#include <AMEIZ-3-1706/engine/io/ValueUtils.hpp>
#include <AMEIZ-3-1706/engine/io/KeyValueParser.hpp>
#include <math.h>

using namespace als;
using namespace anm2dr::util;
using namespace engine::io;

void VoiceBehavior::load(const char* file_name)
{
	size_t str_len;
	char* settings=(char*)readBytesFromFile(file_name,&str_len);
	ArrayList<KeyValuePairStr> &key_value_pairs=parseKeyValuePairStr(settings,str_len);
	for(int i=0;i<=key_value_pairs.getLastIndex();++i)
	{
		KeyValuePairStr pair=key_value_pairs.get(i);
		if(str_eq("main_voice_on",pair.key))
			main_voice_on=parseBool(pair.value);
		else if(str_eq("backyard_voice_on",pair.key))
			backyard_voice_on=parseBool(pair.value);
		else if(str_eq("voice_interval",pair.key))
			voice_interval=parseFloat(pair.value);
		else if(str_eq("voice_volume",pair.key))
			voice_volume=parseFloat(pair.value);
	}
	free(settings);
	delete &key_value_pairs;
}
