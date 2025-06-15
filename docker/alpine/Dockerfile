FROM node:lts-alpine3.22

WORKDIR /usr/app

RUN apk add g++
RUN apk add make
RUN apk add cmake
RUN apk add ccache
RUN apk add valgrind valgrind-dev
RUN apk add rapidjson-dev
RUN apk add boost-dev

RUN npm install -g vsce

COPY . .

CMD ["pwd"]
