#!/bin/sh

cd src
g++ -std=c++17 main.cpp -o../profile
cd ..
./profile