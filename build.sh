#!/bin/bash

cd src
make

cd ../jni
make

cd ../examples/linux
make

cd ../java
make
