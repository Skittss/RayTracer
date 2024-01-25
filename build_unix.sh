#!/bin/bash
mkdir -p build
mkdir -p exports/ppm
mkdir -p exports/png
cd build
cmake -S ../ -B .
make && ./raytracer
cd ../