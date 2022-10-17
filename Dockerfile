FROM alpine:3.12

WORKDIR /usr/app/build

RUN apk add g++
RUN apk add make
RUN apk add cmake
RUN apk add ccache
RUN apk add valgrind-dev
RUN apk add rapidjson-dev
RUN apk add boost-dev
RUN apk add gtest-dev

CMD ["cmake", ".."]
