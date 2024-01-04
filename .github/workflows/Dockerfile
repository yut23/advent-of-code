# based on https://hub.docker.com/layers/ldionne/libcxx-builder/latest/images/sha256-1dff4680e3a08c06befade387b3d95f8944dd949532eea3ddd7b9e66fea48dde?context=explore
ARG UBUNTU_VERSION
ARG LLVM_VERSION
FROM ubuntu:$UBUNTU_VERSION
RUN apt-get update && \
    apt-get install -y \
      git \
      build-essential \
      lsb-release \
      wget \
      software-properties-common \
      gnupg
RUN wget https://apt.llvm.org/llvm.sh -O /tmp/llvm.sh
# install compilers and libc++
RUN bash /tmp/llvm.sh $LLVM_VERSION
RUN apt-get install -y \
      libc++-$LLVM_VERSION-dev libc++abi-$LLVM_VERSION-dev \
      clang \
      g++
CMD ["/bin/bash"]
