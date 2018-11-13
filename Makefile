#Makefile
CC = g++
FLAGS = -std=c++11

all: write_fixed_len_pages read_fixed_len_pages csv2heapfile scan

library.o: library.cc library.h
	$(CC) -o $@ -c $<

write_fixed_len_pages: write_fixed_len_pages.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

read_fixed_len_pages: read_fixed_len_page.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

csv2heapfile: csv2heapfile.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

scan: scan.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

clean:
	rm *.o write_fixed_len_pages read_fixed_len_pages csv2heapfile 
