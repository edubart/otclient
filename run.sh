#!/usr/bin/env bash

docker run -ti --rm \
       -e DISPLAY \
       -v /tmp/.X11-unix:/tmp/.X11-unix \
       --device /dev/dri \
       edubart/otclient 
