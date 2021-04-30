# framework flags
# FRAMEWORK_SOUND
# FRAMEWORK_GRAPHICS
# FRAMEWORK_NET
# FRAMEWORK_XML
# FRAMEWORK_SQL

# add framework cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(framework_LIBRARIES "")
set(framework_DEFINITIONS "")
set(framework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/..")
set(framework_SOURCES ${framework_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/const.h
    ${CMAKE_CURRENT_LIST_DIR}/global.h
    ${CMAKE_CURRENT_LIST_DIR}/pch.h
    ${CMAKE_CURRENT_LIST_DIR}/luafunctions.cpp

    ${CMAKE_CURRENT_LIST_DIR}/util/color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/util/color.h
    ${CMAKE_CURRENT_LIST_DIR}/util/crypt.cpp
    ${CMAKE_CURRENT_LIST_DIR}/util/crypt.h
    ${CMAKE_CURRENT_LIST_DIR}/util/databuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/util/matrix.h
    ${CMAKE_CURRENT_LIST_DIR}/util/point.h
    ${CMAKE_CURRENT_LIST_DIR}/util/rect.h
    ${CMAKE_CURRENT_LIST_DIR}/util/size.h

    # stdext
    ${CMAKE_CURRENT_LIST_DIR}/stdext/any.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/boolean.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/cast.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/compiler.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/demangle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stdext/demangle.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/dumper.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/dynamic_storage.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/exception.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/format.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/math.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stdext/math.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/net.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stdext/net.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/packed_any.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/packed_storage.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/shared_object.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/shared_ptr.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/stdext.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/string.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stdext/string.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/time.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stdext/time.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/traits.h
    ${CMAKE_CURRENT_LIST_DIR}/stdext/types.h

    # core
    ${CMAKE_CURRENT_LIST_DIR}/core/application.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/application.h
    ${CMAKE_CURRENT_LIST_DIR}/core/adaptativeframecounter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/adaptativeframecounter.h
    ${CMAKE_CURRENT_LIST_DIR}/core/asyncdispatcher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/asyncdispatcher.h
    ${CMAKE_CURRENT_LIST_DIR}/core/binarytree.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/binarytree.h
    ${CMAKE_CURRENT_LIST_DIR}/core/clock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/clock.h
    ${CMAKE_CURRENT_LIST_DIR}/core/config.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/config.h
    ${CMAKE_CURRENT_LIST_DIR}/core/configmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/configmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/core/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/core/event.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/event.h
    ${CMAKE_CURRENT_LIST_DIR}/core/eventdispatcher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/eventdispatcher.h
    ${CMAKE_CURRENT_LIST_DIR}/core/filestream.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/filestream.h
    ${CMAKE_CURRENT_LIST_DIR}/core/inputevent.h
    ${CMAKE_CURRENT_LIST_DIR}/core/logger.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/logger.h
    ${CMAKE_CURRENT_LIST_DIR}/core/module.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/module.h
    ${CMAKE_CURRENT_LIST_DIR}/core/modulemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/modulemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/core/resourcemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/resourcemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/core/scheduledevent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/scheduledevent.h
    ${CMAKE_CURRENT_LIST_DIR}/core/timer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/timer.h

    # luaengine
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/luabinder.h
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/luaexception.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/luaexception.h
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/luainterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/luainterface.h
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/luaobject.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/luaobject.h
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/luavaluecasts.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/luavaluecasts.h
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/lbitlib.cpp
    ${CMAKE_CURRENT_LIST_DIR}/luaengine/lbitlib.h

    # otml
    ${CMAKE_CURRENT_LIST_DIR}/otml/declarations.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmldocument.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmldocument.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlemitter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlemitter.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlexception.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlexception.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otml.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlnode.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlnode.h
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlparser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/otml/otmlparser.h

    # crash handler
    ${CMAKE_CURRENT_LIST_DIR}/platform/crashhandler.h
    ${CMAKE_CURRENT_LIST_DIR}/platform/unixcrashhandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform/win32crashhandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform/win32platform.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform/unixplatform.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform/platform.cpp
    ${CMAKE_CURRENT_LIST_DIR}/platform/platform.h
)

if(NOT MSVC)
    set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/luafunctions.cpp
                                PROPERTIES LANGUAGE CXX COMPILE_FLAGS "-g0 -Os")
endif()

# some build options
option(LUAJIT "Use lua jit" OFF)
option(USE_STATIC_LIBS "Don't use shared libraries (dlls)" ON)
if(NOT APPLE)
    option(CRASH_HANDLER "Generate crash reports" ON)
    option(USE_LIBCPP "Use the new libc++ library instead of stdc++" OFF)
    option(USE_LTO "Use link time optimizations" OFF)
else()
    set(CRASH_HANDLER OFF)
    set(USE_LIBCPP ON)
endif()
set(BUILD_COMMIT "devel" CACHE STRING "Git commit string (intended for releases)")
set(BUILD_REVISION "0" CACHE STRING "Git revision string (intended for releases)")

# default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(COMPILER_CXX_EXTRA_FLAGS       "-Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-unused-result -pipe")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O1 -g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE        "-O2")
if (WIN32 AND MSVC)
    set(COMPILER_CXX_EXTRA_FLAGS       "/bigobj")
    set(CMAKE_CXX_FLAGS_DEBUG          "/Od /DEBUG:FULL")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O12 /DEBUG:FASTLINK")
    set(CMAKE_CXX_FLAGS_RELEASE        "/Ot")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_CXX_EXTRA_FLAGS} ${ARCH_FLAGS} ${CPP2011_FLAGS}")

if(USE_LTO AND NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwhole-program -flto")
    if(WIN32)
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-O1,--gc-sections,--sort-common,--relax")
    else()
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-O1,--gc-sections,--sort-common,--relax,-z,relro")
    endif()
endif()

# process options
if(USE_STATIC_LIBS)
    if(NOT APPLE)
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -static-libgcc -static-libstdc++")
    endif()
    message(STATUS "Link to static libraries: ON")
else()
    message(STATUS "Link to static libraries: OFF")
endif()

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
add_definitions(-D"BUILD_TYPE=\\\"${CMAKE_BUILD_TYPE}\\\"")
message(STATUS "Build commit: ${BUILD_COMMIT}")
add_definitions(-D"BUILD_COMMIT=\\\"${BUILD_COMMIT}\\\"")
message(STATUS "Build revision: ${BUILD_REVISION}")
add_definitions(-D"BUILD_REVISION=\\\"${BUILD_REVISION}\\\"")

# find boost
set(REQUIRED_BOOST_COMPONENTS system thread filesystem)
if(WIN32)
    set(Boost_THREADAPI win32)
    set(framework_DEFINITIONS ${framework_DEFINITIONS} -DBOOST_THREAD_USE_LIB) # fix boost thread linkage
endif()
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ${USE_STATIC_LIBS})
find_package(Boost 1.48.0 COMPONENTS ${REQUIRED_BOOST_COMPONENTS} REQUIRED)

#find lua
if(LUAJIT)
    find_package(LuaJIT REQUIRED)
    set(LUA_INCLUDE_DIR ${LUAJIT_INCLUDE_DIR})
    set(LUA_LIBRARY ${LUAJIT_LIBRARY})

    if(APPLE)
      set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -pagezero_size 10000 -image_base 100000000")
    endif()
else()
    find_package(Lua REQUIRED)
endif()
message(STATUS "LuaJIT: " ${LUAJIT})

find_package(PhysFS REQUIRED)
find_package(ZLIB REQUIRED)

set(framework_LIBRARIES ${framework_LIBRARIES}
    ${Boost_LIBRARIES}
    ${LUA_LIBRARY}
    ${PHYSFS_LIBRARY}
    ${ZLIB_LIBRARY}
)

set(framework_INCLUDE_DIRS ${framework_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIR}
    ${PHYSFS_INCLUDE_DIR}
    ${framework_INCLUDE_DIRS}
)

find_package(OpenSSL QUIET)

if(NOT OPENSSL_FOUND)
    find_package(GMP REQUIRED)
    set(framework_LIBRARIES ${framework_LIBRARIES} ${GMP_LIBRARY})
    set(framework_INCLUDE_DIRS ${framework_INCLUDE_DIRS} ${GMP_INCLUDE_DIR})
    set(framework_DEFINITIONS ${framework_DEFINITIONS} -DUSE_GMP)
else()
    set(framework_LIBRARIES ${framework_LIBRARIES} ${OPENSSL_LIBRARIES})
    set(framework_INCLUDE_DIRS ${framework_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "Debug information: ON")
else()
    message(STATUS "Debug information: OFF")
    if(NOT CMAKE_BUILD_TYPE STREQUAL "CompileSpeed")
        set(framework_DEFINITIONS ${framework_DEFINITIONS} -DNDEBUG) # NDEBUG disable asserts
    endif()
    if(NOT APPLE)
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,-s") # strip all debug information
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,--as-needed") # only link needed libraries
    endif()
endif()

if(CRASH_HANDLER)
    set(framework_DEFINITIONS ${framework_DEFINITIONS} -DCRASH_HANDLER)
    message(STATUS "Crash handler: ON")
    if(WIN32)
        set(framework_LIBRARIES ${framework_LIBRARIES} imagehlp)
    endif()
else()
    message(STATUS "Crash handler: OFF")
endif()

if(USE_LIBCPP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wno-deprecated-declarations")
endif()

if(WIN32)
    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthreads")
    endif()

    set(framework_DEFINITIONS ${framework_DEFINITIONS} -D_WIN32_WINNT=0x0501 -DWIN32)
    #set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,--large-address-aware") # strip all debug information
    set(SYSTEM_LIBRARIES "")
else()
    if(APPLE)
        set(framework_DEFINITIONS ${framework_DEFINITIONS} -D_REENTRANT) # enable thread safe code
        set(SYSTEM_LIBRARIES "")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -rdynamic -Wl,-rpath,./libs") # rdynamic is needed by backtrace.h used in crash handler
        set(SYSTEM_LIBRARIES dl rt)
    endif()
endif()
set(framework_LIBRARIES ${framework_LIBRARIES} ${SYSTEM_LIBRARIES})

if(FRAMEWORK_THREAD_SAFE)
    set(framework_DEFINITIONS ${framework_DEFINITIONS} -DTHREAD_SAFE)
endif()

set(OpenGL_GL_PREFERENCE "GLVND")
if(FRAMEWORK_GRAPHICS)
    set(OPENGLES "OFF" CACHE STRING "Use OpenGL ES 1.0 or 2.0 (for mobiles devices)")
    if(OPENGLES STREQUAL "2.0")
        find_package(OpenGLES2 REQUIRED)
        find_package(EGL REQUIRED)
        set(framework_DEFINITIONS ${framework_DEFINITIONS} -DOPENGL_ES=2)
        set(framework_INCLUDE_DIRS ${framework_INCLUDE_DIRS} ${EGL_INCLUDE_DIR} ${OPENGLES2_INCLUDE_DIR})
        set(framework_LIBRARIES ${framework_LIBRARIES} ${EGL_LIBRARY} ${OPENGLES2_LIBRARY})
    elseif(OPENGLES STREQUAL "1.0")
        find_package(OpenGLES1 REQUIRED)
        find_package(EGL REQUIRED)
        set(framework_DEFINITIONS ${framework_DEFINITIONS} -DOPENGL_ES=1)
        set(framework_INCLUDE_DIRS ${framework_INCLUDE_DIRS} ${EGL_INCLUDE_DIR} ${OPENGLES1_INCLUDE_DIR})
        set(framework_LIBRARIES ${framework_LIBRARIES} ${EGL_LIBRARY} ${OPENGLES1_LIBRARY})
    else()
        ## TODO: CMake Documentation says that this is not the right
        # Thing for Mac OS X, but it works for now.
        if(APPLE)
            include_directories(/usr/X11R6/include/)
            link_directories(/usr/X11R6/lib/)
            SET(OPENGL_LIBRARIES GL)
        else()
            find_package(OpenGL REQUIRED)
        endif()
        find_package(GLEW REQUIRED)
        set(framework_LIBRARIES ${framework_LIBRARIES} ${GLEW_LIBRARY} ${OPENGL_LIBRARIES})
    endif()
    message(STATUS "OpenGL ES: " ${OPENGLES})

    if(WIN32)
        option(WINDOWS_CONSOLE "Enables console window on Windows platform" OFF)
        if(WINDOWS_CONSOLE)
            message(STATUS "Windows console: ON")
        else()
            set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -mwindows")
            message(STATUS "Windows console: OFF")
        endif()

        # native dx9 support while its under development
        option(DIRECTX "Use DX9 support" OFF)
        if(DIRECTX)
            find_package(DirectX REQUIRED)
            set(framework_DEFINITIONS ${framework_DEFINITIONS} -DDIRECTX)
            set(framework_INCLUDE_DIRS ${framework_INCLUDE_DIRS} ${DirectX_INCLUDE_DIR})
            set(framework_LIBRARIES ${framework_LIBRARIES} ${DirectX_LIBRARY} ${DirectX_LIBRARIES})
            set(framework_SOURCES ${framework_SOURCES}
                ${CMAKE_CURRENT_LIST_DIR}/graphics/dx/painterdx9.cpp
                ${CMAKE_CURRENT_LIST_DIR}/graphics/dx/painterdx9.h
            )
        endif()

    else()
        set(framework_LIBRARIES ${framework_LIBRARIES} X11)
    endif()

    set(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/graphics/animatedtexture.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/animatedtexture.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/cachedtext.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/cachedtext.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/coordsbuffer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/coordsbuffer.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/declarations.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/bitmapfont.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/bitmapfont.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/fontmanager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/fontmanager.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/framebuffer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/framebuffer.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/framebuffermanager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/framebuffermanager.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/glutil.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/graphics.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/graphics.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/hardwarebuffer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/hardwarebuffer.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/image.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/image.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/painter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/painter.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/ogl/painterogl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/ogl/painterogl.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/ogl/painterogl1.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/ogl/painterogl1.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/ogl/painterogl2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/ogl/painterogl2.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/ogl/painterogl2_shadersources.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/paintershaderprogram.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/paintershaderprogram.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particleaffector.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particleaffector.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particle.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particle.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particletype.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particletype.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particleemitter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particleemitter.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particleeffect.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particleeffect.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particlemanager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particlemanager.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particlesystem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/particlesystem.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/shader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/shader.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/shaderprogram.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/shaderprogram.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/texture.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/texture.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/texturemanager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/texturemanager.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/vertexarray.h
        ${CMAKE_CURRENT_LIST_DIR}/graphics/apngloader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/graphics/apngloader.h

        # ui
        ${CMAKE_CURRENT_LIST_DIR}/ui/declarations.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uianchorlayout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uianchorlayout.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiboxlayout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiboxlayout.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uigridlayout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uigridlayout.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/ui.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uihorizontallayout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uihorizontallayout.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uilayout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uilayout.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uimanager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uimanager.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiparticles.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiparticles.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uitextedit.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uitextedit.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uitranslator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uitranslator.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiverticallayout.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiverticallayout.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidgetbasestyle.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidget.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidget.h
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidgetimage.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ui/uiwidgettext.cpp

        # platform window
        ${CMAKE_CURRENT_LIST_DIR}/platform/platformwindow.cpp
        ${CMAKE_CURRENT_LIST_DIR}/platform/platformwindow.h
        ${CMAKE_CURRENT_LIST_DIR}/platform/win32window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/platform/win32window.h
        ${CMAKE_CURRENT_LIST_DIR}/platform/x11window.cpp
        ${CMAKE_CURRENT_LIST_DIR}/platform/x11window.h

        # window input
        ${CMAKE_CURRENT_LIST_DIR}/input/mouse.cpp
        ${CMAKE_CURRENT_LIST_DIR}/input/mouse.h

        ${CMAKE_CURRENT_LIST_DIR}/core/graphicalapplication.cpp
        ${CMAKE_CURRENT_LIST_DIR}/core/graphicalapplication.h
    )
    set(framework_DEFINITIONS ${framework_DEFINITIONS} -DFW_GRAPHICS)
else()
    set(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/core/consoleapplication.cpp
        ${CMAKE_CURRENT_LIST_DIR}/core/consoleapplication.h
    )
endif()

if(FRAMEWORK_SOUND)
    find_package(OpenAL REQUIRED)
    find_package(VorbisFile REQUIRED)
    find_package(Vorbis REQUIRED)
    find_package(Ogg REQUIRED)

    set(framework_INCLUDE_DIRS ${framework_INCLUDE_DIRS} ${OPENAL_INCLUDE_DIR} ${VORBISFILE_INCLUDE_DIR})
    set(framework_LIBRARIES ${OPENAL_LIBRARY} ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY} ${OGG_LIBRARY} ${framework_LIBRARIES})

    if(WIN32)
        set(framework_LIBRARIES ${framework_LIBRARIES} winmm)
    elseif(APPLE)
        set(framework_LIBRARIES ${framework_LIBRARIES} System)
    endif()

    set(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/sound/combinedsoundsource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sound/combinedsoundsource.h
        ${CMAKE_CURRENT_LIST_DIR}/sound/declarations.h
        ${CMAKE_CURRENT_LIST_DIR}/sound/oggsoundfile.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sound/oggsoundfile.h
        ${CMAKE_CURRENT_LIST_DIR}/sound/soundbuffer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sound/soundbuffer.h
        ${CMAKE_CURRENT_LIST_DIR}/sound/soundchannel.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sound/soundchannel.h
        ${CMAKE_CURRENT_LIST_DIR}/sound/soundfile.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sound/soundfile.h
        ${CMAKE_CURRENT_LIST_DIR}/sound/soundmanager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sound/soundmanager.h
        ${CMAKE_CURRENT_LIST_DIR}/sound/soundsource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sound/soundsource.h
        ${CMAKE_CURRENT_LIST_DIR}/sound/streamsoundsource.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sound/streamsoundsource.h
    )
    set(framework_DEFINITIONS ${framework_DEFINITIONS} -DFW_SOUND)
endif()

if(FRAMEWORK_NET)
    if(WIN32)
        set(NET_LIBRARIES ws2_32 mswsock)
    else()
        set(NET_LIBRARIES "")
    endif()
    set(framework_LIBRARIES ${framework_LIBRARIES} ${NET_LIBRARIES})

    set(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/net/connection.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/connection.h
        ${CMAKE_CURRENT_LIST_DIR}/net/declarations.h
        ${CMAKE_CURRENT_LIST_DIR}/net/inputmessage.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/inputmessage.h
        ${CMAKE_CURRENT_LIST_DIR}/net/outputmessage.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/outputmessage.h
        ${CMAKE_CURRENT_LIST_DIR}/net/protocol.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/protocol.h
        ${CMAKE_CURRENT_LIST_DIR}/net/protocolhttp.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/protocolhttp.h
        ${CMAKE_CURRENT_LIST_DIR}/net/server.cpp
        ${CMAKE_CURRENT_LIST_DIR}/net/server.h
    )
    set(framework_DEFINITIONS ${framework_DEFINITIONS} -DFW_NET)
endif()

if(FRAMEWORK_XML)
    set(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/xml/tinyxml.cpp
        ${CMAKE_CURRENT_LIST_DIR}/xml/tinyxml.h
        ${CMAKE_CURRENT_LIST_DIR}/xml/tinystr.cpp
        ${CMAKE_CURRENT_LIST_DIR}/xml/tinystr.h
        ${CMAKE_CURRENT_LIST_DIR}/xml/tinyxmlerror.cpp
        ${CMAKE_CURRENT_LIST_DIR}/xml/tinyxmlparser.cpp
    )
    set(framework_DEFINITIONS ${framework_DEFINITIONS} -DFW_XML)
endif()

if(FRAMEWORK_SQL)
    find_package(MySQL REQUIRED)

    set(framework_INCLUDE_DIRS ${framework_INCLUDE_DIRS} ${MYSQL_INCLUDE_DIR})
    set(framework_LIBRARIES ${MYSQL_LIBRARY} ${framework_LIBRARIES})

    set(framework_SOURCES ${framework_SOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/sql/declarations.h
        ${CMAKE_CURRENT_LIST_DIR}/sql/database.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sql/database.h
        ${CMAKE_CURRENT_LIST_DIR}/sql/mysql.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sql/mysql.h
    )
    set(framework_DEFINITIONS ${framework_DEFINITIONS} -DFW_SQL)
endif()

if(EXTRA_LIBS)
    set(framework_LIBRARIES ${framework_LIBRARIES} ${EXTRA_LIBS})
endif()

include_directories(${framework_INCLUDE_DIRS})
add_definitions(${framework_DEFINITIONS})
