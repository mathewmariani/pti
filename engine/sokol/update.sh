#!/bin/bash
# update the required sokol headers

DIR=./engine/sokol
SOKOL_APP=https://raw.githubusercontent.com/floooh/sokol/master/sokol_app.h
SOKOL_ARGS=https://raw.githubusercontent.com/floooh/sokol/master/sokol_args.h
SOKOL_AUDIO=https://raw.githubusercontent.com/floooh/sokol/master/sokol_audio.h
SOKOL_GFX=https://raw.githubusercontent.com/floooh/sokol/master/sokol_gfx.h
SOKOL_GL=https://raw.githubusercontent.com/floooh/sokol/master/util/sokol_gl.h
SOKOL_GLUE=https://raw.githubusercontent.com/floooh/sokol/master/sokol_glue.h
SOKOL_LOG=https://raw.githubusercontent.com/floooh/sokol/master/sokol_log.h

curl $SOKOL_APP > $DIR/sokol_app.h
curl $SOKOL_ARGS > $DIR/sokol_args.h
curl $SOKOL_ARGS > $DIR/sokol_audio.h
curl $SOKOL_GFX > $DIR/sokol_gfx.h
curl $SOKOL_GL > $DIR/sokol_gl.h
curl $SOKOL_GLUE > $DIR/sokol_glue.h
curl $SOKOL_LOG > $DIR/sokol_log.h