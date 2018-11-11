#Makefile
CC = g++

library.o: library.cc library.h write_fixed_len_pages.cc
    $(CC) -o $@ -c $<
    $(CC) -g write_fixed_len_pages.cc