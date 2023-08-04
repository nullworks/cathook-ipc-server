#!/bin/bash
if [ -d "build" ]; then
  rm -rf build
fi
mkdir bin
mkdir build
cd build || exit
cmake ..
make clean
make -j"$(nproc --all)"
sudo make install
cd ..
