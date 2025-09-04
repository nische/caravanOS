# .devcontainer/Dockerfile
#
# Minimal, predictable environment on Debian + ESP-IDF installed in-image.
# No special user assumptions. Works the same on Windows/macOS/Linux hosts.
#
FROM debian:12-slim

ARG DEBIAN_FRONTEND=noninteractive
ARG IDF_VERSION=v5.2.2

# Base tools and build deps
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates git curl wget xz-utils unzip \
    python3 python3-pip python3-venv \
    cmake ninja-build build-essential ccache pkg-config \
    libffi-dev libssl-dev \
    usbutils udev dfu-util libusb-1.0-0 \
    vim less nano \
    libgtest-dev libgmock-dev && \
    rm -rf /var/lib/apt/lists/*

# # Build & install GoogleTest/GoogleMock libraries (Debian ships sources in /usr/src)
# RUN cmake -S /usr/src/googletest -B /tmp/googletest-build -G Ninja && \
#     cmake --build /tmp/googletest-build --config Release && \
#     cmake --install /tmp/googletest-build --prefix /usr/local && \
#     rm -rf /tmp/googletest-build

# Install ESP-IDF into /opt/esp/idf and tools into /opt/espressif (inside the image)
ENV IDF_PATH=/opt/esp/idf
ENV IDF_TOOLS_PATH=/opt/espressif

RUN git clone --depth 1 -b ${IDF_VERSION} https://github.com/espressif/esp-idf.git ${IDF_PATH} && \
    ${IDF_PATH}/install.sh esp32

# Make ESP-IDF available in all interactive shells
RUN echo 'source ${IDF_PATH}/export.sh >/dev/null 2>&1 || true' > /etc/profile.d/esp-idf.sh

# Quality-of-life
ENV LANG=C.UTF-8 LC_ALL=C.UTF-8
SHELL ["/bin/bash", "-lc"]

# Quick smoke-test during build (shows IDF version)
RUN source ${IDF_PATH}/export.sh && idf.py --version
