#!/usr/bin/env bash

SE_enforcing=`getenforce` || true

sudo setenforce Permissive || true

docker run -ti --rm \
       -e DISPLAY \
       -v /tmp/.X11-unix:/tmp/.X11-unix \
       --device /dev/dri \
       edubart/otclient 

sudo setenforce $SE_enforcing || true
