#!/usr/bin/env bash

SE_enforcing=`getenforce` || true

sudo setenforce Permissive || true

# Enable any host to connect on X Org
xhost +

docker run -ti --rm \
       -e DISPLAY \
       -v /tmp/.X11-unix:/tmp/.X11-unix \
       --device /dev/dri \
       edubart/otclient 

# Enable any host to connect on X Org
xhost -

sudo setenforce $SE_enforcing || true
