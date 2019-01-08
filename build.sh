#! /bin/bash
mkdir ./build
cd ./build
cmake ../source
make
echo "build complete."
