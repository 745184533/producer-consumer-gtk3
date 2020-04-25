#!/bin/bash
clang `pkg-config --cflags gtk+-3.0` -o producer-consumer-gtk3 src/main.c `pkg-config --libs gtk+-3.0` -rdynamic -O3