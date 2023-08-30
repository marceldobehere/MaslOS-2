#!/bin/bash

find . -type f -name '*.o' -delete
make clean --silent
 
./RUN.sh


make clean2 --silent
