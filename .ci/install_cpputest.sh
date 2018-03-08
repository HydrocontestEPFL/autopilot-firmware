#!/bin/bash

set -eu

apt-get install -y \
    autoconf \
    automake \
    libtool \

echo "Building cpputest"
pushd /
wget https://github.com/cpputest/cpputest/releases/download/v3.8/cpputest-3.8.zip
unzip cpputest-3.8.zip
rm cpputest-3.8.zip
cd cpputest-3.8
autoreconf . -i
./configure
make
make install
popd
