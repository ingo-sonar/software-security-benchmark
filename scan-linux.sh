#!/bin/sh
rm -rf build 
mkdir -p build
build-wrapper-linux-x86-64 --out-dir wrapper bash -c  "cmake -B build -S . && cmake --build build"
sonar-scanner --define sonar.cfamily.build-wrapper-output=wrapper