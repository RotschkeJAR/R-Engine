@echo off

set SRC=src
set BIN=bin/MSVC

set OUT_LIB=%BIN%/libRE.a

set CC=g++
set CFLAG=/std=c++17 -Wall -march=x86-64 -m64
set LDFLAG=/l RE /I %SRC% /L %BIN%

call 
