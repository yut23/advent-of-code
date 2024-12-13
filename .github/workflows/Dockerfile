# syntax=docker/dockerfile:1
# based on https://hub.docker.com/layers/ldionne/libcxx-builder/latest/images/sha256-1dff4680e3a08c06befade387b3d95f8944dd949532eea3ddd7b9e66fea48dde?context=explore
ARG UBUNTU_VERSION=24.04
FROM ubuntu:$UBUNTU_VERSION
# comma-separated version numbers
ARG GCC_VERSIONS
# comma-separated version numbers
ARG LLVM_VERSIONS
RUN apt-get update -qq && \
    apt-get install -qq -y \
      git \
      make \
      lsb-release
# install compilers and libc++
RUN IFS=,; \
    for gcc_version in $GCC_VERSIONS; do \
      apt-get install -y g++-$gcc_version; \
    done;
RUN IFS=,; \
    for llvm_version in $LLVM_VERSIONS; do \
      apt-get install -y \
        clang-${llvm_version} \
        libc++-${llvm_version}-dev libc++abi-${llvm_version}-dev; \
    done;
CMD ["/bin/bash"]
