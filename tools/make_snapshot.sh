#!/bin/bash
# generates otclient zip package for win32
# by edubart :)

gitroot="git://github.com/edubart/otclient.git"
name="otclient"
workdir="$HOME/$name-builds"
mingwplatform="i486-mingw32"
mingwbin="/usr/$mingwplatform/bin"
makejobs=4

replace=false
rebuild=true
for arg in "$@"; do
    case "$arg" in
        --no-rebuild)
            rebuild=false
            ;;
        --replace)
            replace=true
            ;;
        *)
            echo "usage: $0 [--replace] [--no-rebuild]"
            exit
            ;;
    esac
done

# setup work directory
mkdir -p $workdir
cd $workdir

# update otclient
if [ -d otclient ]; then
    cd otclient
    git pull || exit
else
    git clone $gitroot otclient || exit
fi

cd $workdir/otclient
revision=`git rev-list --all | wc -l`
commit=`git describe --always`
version=`cat CMakeLists.txt | grep "set(VERSION" | sed 's/.*"\([^"]*\)".*/\1/'`


if $rebuild; then
    rm -rf build.win32
    rm -rf build.win64
    rm -rf build.win32dx9
    rm -rf build.win64dx9
    rm -rf build.linux32
    rm -rf build.linux64
fi

WIN32_EXTRA_LIBS="-Wl,-Bstatic -lgcc -lstdc++ -lpthread -Wl,-Bdynamic"


# compile for win64
mkdir -p build.win64
cd build.win64
if $rebuild; then
    x86_64-w64-mingw32-cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DBOT_PROTECTION=OFF \
        -DBUILD_REVISION=$revision \
        -DBUILD_COMMIT=$commit \
        -DEXTRA_LIBS="$WIN32_EXTRA_LIBS" \
        .. || exit
fi
make -j$makejobs || exit
cd ..

# compile for win32
mkdir -p build.win32
cd build.win32
if $rebuild; then
    i686-w64-mingw32-cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DBOT_PROTECTION=OFF \
        -DBUILD_REVISION=$revision \
        -DBUILD_COMMIT=$commit \
        -DEXTRA_LIBS="$WIN32_EXTRA_LIBS" \
        .. || exit
fi
make -j$makejobs || exit
cd ..

## compile for win64dx9
#mkdir -p build.win64dx9
#cd build.win64dx9
#if $rebuild; then
    #x86_64-w64-mingw32-cmake \
        #-DCMAKE_BUILD_TYPE=Release \
        #-DBOT_PROTECTION=OFF \
        #-DOPENGLES=2.0 \
        #-DBUILD_REVISION=$revision \
        #-DBUILD_COMMIT=$commit \
        #-DEXTRA_LIBS="$WIN32_EXTRA_LIBS" \
        #.. || exit
#fi
#make -j$makejobs || exit
#cd ..

## compile for win32dx9
#mkdir -p build.win32dx9
#cd build.win32dx9
#if $rebuild; then
    #i686-w64-mingw32-cmake \
        #-DCMAKE_BUILD_TYPE=Release \
        #-DBOT_PROTECTION=OFF \
        #-DOPENGLES=2.0 \
        #-DBUILD_REVISION=$revision \
        #-DBUILD_COMMIT=$commit \
        #-DEXTRA_LIBS="$WIN32_EXTRA_LIBS" \
        #.. || exit
#fi
#make -j$makejobs || exit
#cd ..

# compile for linux64
mkdir -p build.linux64
cd build.linux64
if $rebuild; then
    cmake -DCMAKE_BUILD_TYPE=Release \
        -DBOT_PROTECTION=OFF \
        -DBUILD_REVISION=$revision \
        -DBUILD_COMMIT=$commit \
        .. || exit
fi
make -j$makejobs || exit
cd ..

# compile for linux32
export CFLAGS="-march=i686 -m32"
export CXXFLAGS="-march=i686 -m32"
export LDFLAGS="-march=i686 -m32"

LIBPATH=/usr/lib
if [ -d /usr/lib32 ]; then
    LIBPATH=/usr/lib32
fi

mkdir -p build.linux32
cd build.linux32
if $rebuild; then
    cmake -DCMAKE_BUILD_TYPE=Release \
        -DBOT_PROTECTION=OFF \
        -DBUILD_REVISION=$revision \
        -DBUILD_COMMIT=$commit \
        -DCMAKE_LIBRARY_PATH=$LIBPATH \
        .. || exit
fi
make -j$makejobs || exit
cd ..

##################################################
# create win32 package
pkg_suffix="-linux-$version"
pkgname="$name$pkg_suffix"
pkgzip="$pkgname.tar.gz"
cd $workdir
rm -rf $pkgname
mkdir $pkgname
cd $pkgname

# copy otclient files
mkdir mods
cp $workdir/otclient/mods/README.txt mods/
cp -R $workdir/otclient/modules .
cp -R $workdir/otclient/data .
cp $workdir/otclient/build.linux32/otclient otclient-32
cp $workdir/otclient/build.linux64/otclient otclient-64
cp $workdir/otclient/init.lua .
cp $workdir/otclient/otclientrc.lua .
cp $workdir/otclient/BUGS .
cp $workdir/otclient/AUTHORS .
cp $workdir/otclient/LICENSE .
cp $workdir/otclient/README.md README

# remove git files
find -name '.git*' -exec rm -rf {} \;
find -name '*.spr' -exec rm -f {} \;
find -name '*.dat' -exec rm -f {} \;

cd $workdir

# determine zip name
if ! $replace; then
    let i=1
    while [ -e $pkgzip ]; do
        pkgzip="$pkgname-$i.zip"
        let i=i+1
    done
fi

# compress to a zip file
rm -f $pkgzip
tar czf $pkgzip $pkgname

echo "Package generated to $pkgzip"

##################################################
# create windows package
pkg_suffix="-win-$version"
pkgname="$name$pkg_suffix"
pkgzip="$pkgname.zip"
cd $workdir
rm -rf $pkgname
mkdir $pkgname
cd $pkgname

# copy otclient files
mkdir mods
cp $workdir/otclient/mods/README.txt mods/
cp -R $workdir/otclient/modules .
cp -R $workdir/otclient/data .
#cp $mingwbin/libEGL.dll .
#cp $mingwbin/libGLESv2.dll .
#cp $mingwbin/d3dcompiler_43.dll .
#cp $mingwbin/d3dx9_43.dll .
#cp $mingwbin/wrap_oal.dll .
cp $workdir/otclient/build.win32/otclient.exe otclient-32.exe
cp $workdir/otclient/build.win64/otclient.exe otclient-64.exe
#cp $workdir/otclient/build.win32dx9/otclient.exe otclient_dx9-32.exe
#cp $workdir/otclient/build.winw64dx9/otclient.exe otclient_dx9-64.exe
cp $workdir/otclient/init.lua .
cp $workdir/otclient/otclientrc.lua .
cp $workdir/otclient/AUTHORS AUTHORS.txt
cp $workdir/otclient/BUGS BUGS.txt
cp $workdir/otclient/LICENSE LICENSE.txt
cp $workdir/otclient/README.md README.txt
unix2dos LICENSE.txt README.txt BUGS.txt AUTHORS.txt mods/README.txt

# remove git files
find -name '.git*' -exec rm -rf {} \;
find -name '*.spr' -exec rm -f {} \;
find -name '*.dat' -exec rm -f {} \;

cd $workdir

# determine zip name
if [ ! $replace ]; then
    let i=1
    while [ -e $pkgzip ]; do
        pkgzip="$pkgname-$i.zip"
        let i=i+1
    done
fi

# compress to a zip file
rm -f $pkgzip
zip -9 -qr $pkgzip $pkgname

echo "Package generated to $pkgzip"

# test win32 otclient
cd otclient
wine build.win32/otclient.exe
rm -f *.log

