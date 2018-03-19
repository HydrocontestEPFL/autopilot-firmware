#!/bin/bash

set -eu

apt-get update
apt-get install -y \
    build-essential \
    cmake \
    git \
    python-pip \
    python3-pip \
    unzip \
    wget


echo "Installing protobuf..."
pushd /
wget https://github.com/google/protobuf/releases/download/v3.5.1/protoc-3.5.1-linux-x86_64.zip
unzip protoc*.zip
popd
pip install --upgrade protobuf

echo "Fetching submodules"
git submodule update --init --recursive

echo "Installing nanopb extensions"
pushd lib/nanopb/nanopb/generator/proto
make
popd

echo "Installing cvra packager"
pip3 install PyYAML==3.12 cvra-packager
