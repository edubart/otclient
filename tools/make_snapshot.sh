#!/bin/bash
# generates otclient zip package for win32
# by edubart :)

gitroot="git://github.com/edubart/otclient.git"
name="otclient"
workdir="$HOME/$name-builds"
mingwplatform="i486-mingw32"
mingwbin="/usr/$mingwplatform/bin"
makejobs=4

[ "$1" == "--replace" ] && replace=true || replace=false

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

# get spr and dat
cd $workdir
if [ ! -d Tibia ]; then
    wget http://tibiaclient.com/files/tibia860.tar
    tar xf tibia860.tar
fi

cd $workdir/otclient
revision=`git rev-list --all | wc -l`
commit=`git describe --dirty --always`

# compile for win32
rm -rf build.win32
mkdir build.win32
cd build.win32
cmake -DCMAKE_TOOLCHAIN_FILE=$workdir/otclient/src/framework/cmake/${mingwplatform}_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DBOT_PROTECTION=OFF \
    -DBUILD_REVISION=$revision \
    -DBUILD_COMMIT=$commit \
    .. || exit
make -j$makejobs || exit
cd ..

# compile for win32
rm -rf build.win32dx9
mkdir build.win32dx9
cd build.win32dx9
cmake -DCMAKE_TOOLCHAIN_FILE=$workdir/otclient/src/framework/cmake/${mingwplatform}_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DBOT_PROTECTION=OFF \
    -DOPENGLES=2.0 \
    -DBUILD_REVISION=$revision \
    -DBUILD_COMMIT=$commit \
    .. || exit
make -j$makejobs || exit
cd ..

# compile for linux 64
rm -rf build.linux64
mkdir build.linux64
cd build.linux64
export CXXFLAGS="-march=x86-64 -mtune=generic -O2 -pipe -fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2"
export LDFLAGS="-Wl,-O1,--sort-common,--as-needed,-z,relro"
cmake -DCMAKE_BUILD_TYPE=Release \
    -DBOT_PROTECTION=OFF \
    -DBUILD_REVISION=$revision \
    -DBUILD_COMMIT=$commit \
    .. || exit
make -j$makejobs || exit
cd ..

##################################################
# create win32 package
pkg_suffix="-snapshot-linux64-`date +%Y%m%d`"
pkgname="$name$pkg_suffix"
pkgzip="$pkgname.tgz"
cd $workdir
rm -rf $pkgname
mkdir $pkgname
cd $pkgname

# copy otclient files
cp -R $workdir/otclient/mods .
cp -R $workdir/otclient/modules .
cp $workdir/otclient/build.linux64/otclient .
cp $workdir/otclient/build.linux64/otclient.map .
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

mkdir $pkgname/modules/game_tibiafiles/860/
cp $workdir/Tibia/*.spr $pkgname/modules/game_tibiafiles/860/
cp $workdir/Tibia/*.dat $pkgname/modules/game_tibiafiles/860/

##################################################
# create win32 package
pkg_suffix="-snapshot-win32-`date +%Y%m%d`"
pkgname="$name$pkg_suffix"
pkgzip="$pkgname.zip"
cd $workdir
rm -rf $pkgname
mkdir $pkgname
cd $pkgname

# copy otclient files
cp -R $workdir/otclient/mods .
cp -R $workdir/otclient/modules .
cp $mingwbin/libEGL.dll .
cp $mingwbin/libGLESv2.dll .
cp $mingwbin/d3dcompiler_43.dll .
cp $mingwbin/d3dx9_43.dll .
cp $workdir/otclient/build.win32/otclient.exe .
cp $workdir/otclient/build.win32/otclient.map .
cp $workdir/otclient/build.win32dx9/otclient.exe otclient_dx9.exe
cp $workdir/otclient/build.win32dx9/otclient.map otclient_dx9.map
cp $workdir/otclient/init.lua .
cp $workdir/otclient/otclientrc.lua .
cp $workdir/otclient/AUTHORS AUTHORS.txt
cp $workdir/otclient/BUGS BUGS.txt
cp $workdir/otclient/LICENSE LICENSE.txt
cp $workdir/otclient/README.md README.txt
unix2dos LICENSE.txt README.txt BUGS.txt AUTHORS.txt

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
mkdir otclient/modules/game_tibiafiles/860/
cp $workdir/Tibia/*.spr otclient/modules/game_tibiafiles/860/
cp $workdir/Tibia/*.dat otclient/modules/game_tibiafiles/860/


cd otclient
wine build.win32/otclient.exe
rm -f *.log

