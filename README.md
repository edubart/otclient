[![Build Status](https://secure.travis-ci.org/edubart/otclient.svg?branch=master)](http://travis-ci.org/edubart/otclient) [![Join the chat at https://gitter.im/edubart/otclient](https://img.shields.io/badge/GITTER-join%20chat-green.svg)](https://gitter.im/edubart/otclient?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)\

### What is otclient?

Otclient is an alternative Tibia client for usage with otserv. It aims to be complete and flexible,
for that it uses LUA scripting for all game interface functionality and configurations files with a syntax
similar to CSS for the client interface design. Otclient works with a modular system, this means
that each functionality is a separated module, giving the possibility to users modify and customize
anything easily. Users can also create new mods and extend game interface for their own purposes.
Otclient is written in C++11 and heavily scripted in lua.

For a server to connect to, you can build your own with the [forgottenserver](https://github.com/otland/forgottenserver)
or connect to one listed on [otservlist](https://otservlist.org/).

### Where do I download?

Compiled for Windows can be found here (but can be outdated):
* [Windows Builds](http://otland.net/threads/otclient-builds-windows.217977/)

**NOTE:** You will need to download spr/dat files on your own and place them in `data/things/VERSION/` (i.e: `data/things/1098/Tibia.spr`)

### Features

Beyond of it's flexibility with scripts, otclient comes with tons of other features that make possible
the creation of new client side stuff in otserv that was not possible before. These include,
sound system, graphics effects with shaders, modules/addons system, animated textures,
styleable user interface, transparency, multi language, in game lua terminal, an OpenGL 1.1/2.0 ES engine that make possible
to port to mobile platforms. Otclient is also flexible enough to
create tibia tools like map editors just using scripts, because it wasn't designed to be just a
client, instead otclient was designed to be a combination of a framework and tibia APIs.

### Compiling

In short, if you need to compile OTClient, follow these tutorials:
* [Compiling on Windows](https://github.com/edubart/otclient/wiki/Compiling-on-Windows)
* [Compiling on Linux](https://github.com/edubart/otclient/wiki/Compiling-on-Linux)
* [Compiling on OS X](https://github.com/edubart/otclient/wiki/Compiling-on-Mac-OS-X)


### Need help?

Try to ask questions in [otland](http://otland.net/f494/), now we have a board for the project there,
or talk with us at the gitter chat.

### Bugs

Have found a bug? Please create an issue in our [bug tracker](https://github.com/edubart/otclient/issues)

### Contributing

We encourage you to contribute to otclient! You can make pull requests of any improvement in our github page, alternatively, see [Contributing Wiki Page](https://github.com/edubart/otclient/wiki/Contributing).

### Contact

Talk directly with us at the gitter chat [![Join the chat at https://gitter.im/edubart/otclient](https://img.shields.io/badge/GITTER-join%20chat-green.svg)](https://gitter.im/edubart/otclient?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge).

### License

Otclient is made available under the MIT License, thus this means that you are free
to do whatever you want, commercial, non-commercial, closed or open.
