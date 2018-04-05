FROM ubuntu:xenial
LABEL maintainer="Antoine Albertelli <antoine.albertelli@epfl.ch>"
LABEL description="Build environment for Hydrocontest EPFL team flight controller"

ADD .ci /ci
RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        git \
        python-pip \
        python3-pip \
        unzip \
        wget


RUN cd / && \
    wget https://github.com/google/protobuf/releases/download/v3.5.1/protoc-3.5.1-linux-x86_64.zip && \
    unzip protoc*.zip && \
    pip install --upgrade protobuf

RUN pip3 install PyYAML==3.12 cvra-packager

RUN /ci/install_cpputest.sh
RUN /ci/install_crosscompiler.sh
