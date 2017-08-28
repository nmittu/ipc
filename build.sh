#!/bin/bash

cd src
make

cd ../jni
make

cd ../cpp
make

cd ../examples/linux
make

cd ../java
make

cd ../cpp/linux
make
