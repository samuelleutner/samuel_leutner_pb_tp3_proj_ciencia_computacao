FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    libomp-dev \
    python3 \
    python3-pip \
    vim \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /project

CMD ["/bin/bash"]