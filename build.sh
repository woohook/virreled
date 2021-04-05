#!/bin/sh
gcc src/simcar2.c -o simcar2 `sdl2-config --cflags --libs` -lm -ldl -O3 -fomit-frame-pointer -ffast-math -funroll-loops -funsafe-math-optimizations -Wall -W
