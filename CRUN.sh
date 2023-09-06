#!/bin/bash

trap "make clean2 --silent; echo" INT

make clean --silent
 
./RUN.sh


make clean2 --silent

