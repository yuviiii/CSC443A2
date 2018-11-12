#Makefile
CC = g++

all: write_fixed_len_pages read_fixed_len_pages csv2heapfile

library.o: library.cc library.h
	$(CC) -o $@ -c $<

write_fixed_len_pages: write_fixed_len_pages.cc library.o
	$(CC) -o $@ $< library.o

read_fixed_len_pages: read_fixed_len_page.cc library.o
	$(CC) -o $@ $< library.o

csv2heapfile: csv2heapfile.cc library.o
	$(CC) -o $@ $< library.o