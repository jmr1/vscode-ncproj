FROM node:lts-bookworm-slim

WORKDIR /usr/app

RUN apt-get update
RUN apt-get install -y g++
RUN apt-get install -y make
RUN apt-get install -y cmake
RUN apt-get install -y ccache
RUN apt-get install -y valgrind
RUN apt-get install -y rapidjson-dev
RUN apt-get install -y libboost-dev libboost-program-options-dev
RUN apt-get install -y googletest google-mock
RUN mkdir -p /usr/src/gtest_build && cd /usr/src/gtest_build && \
    cmake /usr/src/googletest \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_INSTALL_PREFIX=/usr && \
    make -j$(nproc) && \
    make install && \
    ldconfig

RUN npm install -g vsce

COPY . .

CMD ["pwd"]
