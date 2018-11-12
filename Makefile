#Makefile
CC = g++

all: write_fixed_len_pages
	
library.o: library.cc library.h
	$(CC) -o $@ -c $<

write_fixed_len_pages: write_fixed_len_pages.cc library.o
	$(CC) -o $@ $< library.o