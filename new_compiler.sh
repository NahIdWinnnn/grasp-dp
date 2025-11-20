#!/bin/bash

mkdir source/build
cd source/build
cmake ..
make
cd ..
rm -r build