#Makefile
CC = g++
FLAGS = -std=c++11

all: write_fixed_len_pages read_fixed_len_pages csv2heapfile scan insert delete update select csv2colstore

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

insert: insert.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

delete: delete.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

update: update.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

select: select.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

csv2colstore: csv2colstore.cc library.o
	$(CC) $(FLAGS) -o $@ $< library.o

clean:
	rm *.o write_fixed_len_pages read_fixed_len_pages csv2heapfile 
