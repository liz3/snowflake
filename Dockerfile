FROM ubuntu:latest AS build
WORKDIR /usr/src/build
RUN apt-get update && apt-get install -y build-essential \
    sudo \
    cmake \
    libboost-all-dev \
    libprotobuf-dev \
    clang-3.6 \
    wget \
    git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
COPY ./CMakeLists.txt ./
COPY ./src ./src
COPY ./main.c ./main.c
RUN cmake .
RUN make
RUN ls
FROM node:latest AS release
EXPOSE 8080
WORKDIR /usr/src/app
COPY --from=build /usr/src/build/snowflake ./snowflake
RUN npm i -g yarn
COPY ./context/package.json ./
COPY ./context/yarn.lock ./
RUN yarn
COPY ./context/src ./src
CMD ["npm", "start"]