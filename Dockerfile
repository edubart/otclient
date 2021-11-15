FROM ubuntu@sha256:b88f8848e9a1a4e4558ba7cfc4acc5879e1d0e7ac06401409062ad2627e6fb58 AS builder

RUN apt-get update; \
  apt-get install -y \
    build-essential \
    cmake \
    git-core \
    libboost-atomic1.65-dev \
    libboost-chrono1.65-dev \
    libboost-date-time1.65-dev \
    libboost-filesystem1.65-dev \
    libboost-system1.65-dev \
    libboost-thread1.65-dev \
    libglew-dev \
    liblua5.1-0-dev \
    libncurses5-dev \
    libopenal-dev \
    libssl-dev \
    libvorbis-dev \
    zlib1g-dev; \
  apt-get clean && apt-get autoclean

WORKDIR /
RUN git clone --branch release-3.0.2 --depth 1 https://github.com/icculus/physfs.git
WORKDIR /physfs/build/
RUN cmake ..
RUN make -j$(nproc)
RUN make install

COPY ./src/ /otclient/src/.
COPY CMakeLists.txt /otclient/.
WORKDIR /otclient/build/
RUN cmake -DCMAKE_CXX_LINK_FLAGS=-no-pie -DCMAKE_BUILD_TYPE=Release ..
RUN make -j$(nproc)

FROM ubuntu@sha256:b88f8848e9a1a4e4558ba7cfc4acc5879e1d0e7ac06401409062ad2627e6fb58
RUN apt-get update; \
  apt-get install -y \
    libglew2.0 \
    libopenal1; \
  apt-get clean && apt-get autoclean
COPY --from=builder /otclient/build/otclient /otclient/bin/otclient
COPY ./data/ /otclient/data/.
COPY ./mods/ /otclient/mods/.
COPY ./modules/ /otclient/modules/.
COPY ./init.lua /otclient/.
WORKDIR /otclient
CMD ["./bin/otclient"]
