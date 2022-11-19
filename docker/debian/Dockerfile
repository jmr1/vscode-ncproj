FROM node:lts-bullseye-slim

WORKDIR /usr/app

RUN apt-get update
RUN apt-get install -y g++
RUN apt-get install -y make
RUN apt-get install -y cmake
RUN apt-get install -y ccache
RUN apt-get install -y valgrind
RUN apt-get install -y rapidjson-dev
RUN apt-get install -y libboost-dev libboost-program-options-dev
RUN apt-get install -y git
RUN cd .. && if [ ! -d googletest ]; then git clone https://github.com/google/googletest.git; fi && cd googletest && mkdir -p build && cd build && cmake .. -DBUILD_SHARED_LIBS=ON -DINSTALL_GTEST=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr && make -j && make install && ldconfig && cd /usr/app

RUN npm install -g vsce

COPY . .

CMD ["pwd"]
