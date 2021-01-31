#!/bin/bash

cd ../src
make
cp AC2 ../benchmark-ref/primates
cd ../benchmark-ref/primates
unzip ds.zip
./run.py hs/ ggpapt/

