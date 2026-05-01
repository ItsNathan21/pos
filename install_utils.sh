#!/bin/bash

sudo apt update && sudo apt upgrade -y

sudo apt install -y \
    build-essential bison flex wget tar \
    libgmp-dev libmpc-dev libmpfr-dev libisl-dev \
    texinfo \
    grub-pc-bin grub-common xorriso mtools \
    qemu-system-x86 \
    gdb-multiarch \
    nasm

# --- Configuration ---
export TARGET=i686-elf
export PREFIX="$HOME/opt/cross"
export PATH="$PREFIX/bin:$PATH"

BINUTILS_VER=2.43
GCC_VER=14.2.0

mkdir -p "$HOME/src" && cd "$HOME/src"

# --- Download ---
wget https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VER}.tar.xz
wget https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VER}/gcc-${GCC_VER}.tar.xz

tar xf binutils-${BINUTILS_VER}.tar.xz
tar xf gcc-${GCC_VER}.tar.xz

# --- Build binutils (ld, as, objdump, etc.) ---
mkdir build-binutils && cd build-binutils
../binutils-${BINUTILS_VER}/configure \
    --target=$TARGET      \
    --prefix=$PREFIX      \
    --with-sysroot        \
    --disable-nls         \
    --disable-werror
make -j$(nproc)
make install
cd ..

# --- Build GCC (compiler + libgcc only — no libc) ---
mkdir build-gcc && cd build-gcc
../gcc-${GCC_VER}/configure \
    --target=$TARGET          \
    --prefix=$PREFIX          \
    --disable-nls             \
    --enable-languages=c,c++  \
    --without-headers
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make install-gcc
make install-target-libgcc
cd ..

echo 'export PATH="$HOME/opt/cross/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc

