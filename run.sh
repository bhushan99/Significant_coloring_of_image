#!/bin/bash
if [ "$1" != "" ]; then
	g++ -c main.cpp image.cpp
	g++ main.o image.o
	./a.out $1 $2
	rm main.o image.o a.out
else
    echo "Please specify input image"
    echo "./run.sh <input_image_location> (<output_image_location> : optional)"
fi
