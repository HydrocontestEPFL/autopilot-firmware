#!/bin/sh

set -eu

echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" > /etc/apt/sources.list.d/bazel.list
wget https://bazel.build/bazel-release.pub.gpg -O - | apt-key add -

apt-get update
apt-get install -y openjdk-8-jdk bazel


