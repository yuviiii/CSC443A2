# Makefile
CC = g++ 
 
library.o: library.cc
    $(CC) -o $@ -c $<