#!/bin/bash
# update the required pti headers

# repo url
CUTE_ASEPRITE_H="https://raw.githubusercontent.com/mathewmariani/cute_headers/master/cute_aseprite.h"
STB_VORBIS_C="https://raw.githubusercontent.com/nothings/stb/master/stb_vorbis.c"

# output directory
OUTPUT=./tests

# curl all libs
curl $CUTE_ASEPRITE_H > $OUTPUT/cute_aseprite.h
curl $STB_VORBIS_C > $OUTPUT/stb_vorbis.c
