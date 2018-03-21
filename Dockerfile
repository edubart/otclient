from ubuntu:latest

WORKDIR /app

RUN apt-get update; apt-get install -y \
  build-essential \
  cmake \
  git-core \
  libboost-all-dev \
  libglew-dev \
  liblua5.1-0-dev \
  libopenal-dev \
  libphysfs-dev \
  libssl-dev \
  libvorbis-dev \
  zlib1g-dev

RUN apt-get install -y \
  libncurses5-dev \
  mercurial; \
  hg clone -r stable-2.0 http://hg.icculus.org/icculus/physfs/; \
    cd physfs; \
    mkdir build && cd build && cmake .. && make && make install; \
    mv /usr/local/lib/libphysfs.a /usr/lib/x86_64-linux-gnu/.

ADD . /app

# Build application
RUN mkdir -p build && cd build && cmake .. && make -j$(grep -c ^process /proc/cpuinfo); 

CMD cd build; ./otclient 
