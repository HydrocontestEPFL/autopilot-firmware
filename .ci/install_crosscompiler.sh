#!/bin/bash

set -eu

apt-get install -y \
  python-software-properties \
  software-properties-common

add-apt-repository ppa:team-gcc-arm-embedded/ppa
apt-get update
apt-get install -y gcc-arm-embedded
