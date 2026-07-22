FROM python:3.9

RUN apt-get update -q \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    curl \
    git \
    libgl1-mesa-dev \
    libgl1-mesa-glx \
    libglew-dev \
    libosmesa6-dev \
    software-properties-common \
    net-tools \
    vim \
    virtualenv \
    wget \
    xpra \
    xserver-xorg-dev \
    fontconfig \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN virtualenv --python=python3.9 env

RUN ln -s /env/bin/python3.9 /usr/bin/python

ENV LANG C.UTF-8

RUN pip install --no-cache-dir pygame

COPY . /app
WORKDIR /app
CMD ["python", "main.py"]
