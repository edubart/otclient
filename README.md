[![Build Status](https://secure.travis-ci.org/edubart/otclient.svg?branch=master)](http://travis-ci.org/edubart/otclient) [![Join the chat at https://gitter.im/edubart/otclient](https://img.shields.io/badge/GITTER-join%20chat-green.svg)](https://gitter.im/edubart/otclient?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
### What is otclient?

Otclient is an alternative Tibia client for usage with otserv. It aims to be complete and flexible,
for that it uses LUA scripting for all game interface functionality and configurations files with a syntax
similar to CSS for the client interface design. Otclient works with a modular system, this means
that each functionality is a separated module, giving the possibility to users modify and customize
anything easily. Users can also create new mods and extend game interface for their own purposes.
Otclient is written in C++2011, the upcoming C++ standard and heavily scripted in lua.

### Where do I download?

The latest commits compiled for Windows can be found here.
* [Windows Builds](http://otland.net/threads/otclient-builds-windows.217977/)

Here is the latest v0.6.5 release compiled for both i686(32 bit) and x86_64(64 bit) machines, with OpenGL renderer.
This release is compatible with protocols ranges from 7.60 up to 10.31.
* [For Windows](https://www.dropbox.com/sh/se1okacemoqzjve/XFqFoSKLCg/otclient-win-0.6.5.zip)
* [For Linux](https://www.dropbox.com/sh/se1okacemoqzjve/xKJL7j6vEo/otclient-linux-0.6.5.tar.gz)

**NOTE:** You will need to download spr/dat files on your own and place them in `data/things/VERSION/` (i.e: `data/things/1041/Tibia.spr`)

Older releases can be downloaded from [here](https://www.dropbox.com/sh/se1okacemoqzjve/-oWK4YFm03)

### Features

Beyond of it's flexibility with scripts, otclient comes with tons of other features that make possible
the creation of new client side stuff in otserv that was not possible before. These include,
sound system, graphics effects with shaders, modules/addons system, animated textures,
styleable user interface, transparency, multi language, in game lua terminal, an OpenGL 1.1/2.0 ES engine that make possible
to port to mobile platforms. Otclient is also flexible enough to
create tibia tools like map editors just using scripts, because it wasn't designed to be just a
client, instead otclient was designed to be a combination of a framework and tibia APIs.

### Compiling

A package with all required libraries for compiling OTClient on Windows can be found here:
* [For MSVC 2013](https://www.dropbox.com/sh/se1okacemoqzjve/dI4ODbq7OT/otclient-msvc13-libs.zip)
* [For MingW32](https://www.dropbox.com/sh/se1okacemoqzjve/UAkRCiGXXR/otclient-libs_mingw32-dwarf2.zip)

In short, if you need to compile OTClient, follow these tutorials:
* [Compiling on Windows](https://github.com/edubart/otclient/wiki/Compiling-on-Windows)
* [Compiling on Linux](https://github.com/edubart/otclient/wiki/Compiling-on-Linux)
* [Compiling on OS X](https://github.com/edubart/otclient/wiki/Compiling-on-Mac-OS-X)



### Need help?

Try to ask questions in [otland](http://otland.net/f494/), now we have a board for the project there,
or talk with us at #otclient irc.freenode.net

### Bugs

Have found a bug? Please create an issue in our [bug tracker](https://github.com/edubart/otclient/issues)

### Contributing

We encourage you to contribute to otclient! You can make pull requests of any improvement in our github page, alternatively, see [Contributing Wiki Page](https://github.com/edubart/otclient/wiki/Contributing).

### Contact

Talk directly with us at #otclient irc.freenode.net or send an email directly to the project leader edub4rt@gmail.com

### License

Otclient is made available under the MIT License, thus this means that you are free
to do whatever you want, commercial, non-commercial, closed or open.
