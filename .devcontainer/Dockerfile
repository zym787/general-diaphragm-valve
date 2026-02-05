FROM ubuntu:20.04

RUN apt-get update && \
    apt-get install wget -y \
    apt-get clean

RUN cd /tmp && \
    wget -q https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/6-2017q2/gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2 && \
    tar xf gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2 && \
    mv gcc-arm-none-eabi-6-2017-q2-update /opt/gcc-arm-none-eabi && \
    rm gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2

ENV PATH="${PATH}:/opt/gcc-arm-none-eabi/bin"