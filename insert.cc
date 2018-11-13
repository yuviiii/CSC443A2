#include "library.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>

// Insert all records in the CSV file to a heap file

int main(int argc, char **argv){
	if (argc!=4){
		printf("Usage: ./insert <heapfile> <csv_file> <page_size>\n");
		exit(1);
	}
	int page_size = atoi(argv[3]),record_size = 1000;
    std::ifstream ifs;
    ifs.open(argv[2]);
    FILE *open_heap_file = fopen(argv[1], "rb+");
    Page *page = (Page *)malloc(sizeof(Page));
    page->data = (char *)malloc(page_size);
    init_fixed_len_page(page, page_size, record_size);
	struct timeb t;
    long now_in_ms1,now_in_ms2,total_time = 0;
    Heapfile *heap_file = (Heapfile *)malloc(sizeof(Heapfile));
    heap_file->page_size = page_size;
    heap_file->file_ptr = open_heap_file;
    std::string line;

    ftime(&t);
    now_in_ms1 = t.time*1000 + t.millitm;
    while (std::getline(ifs, line)){
    	Record record;
    	line.erase(std::remove(line.begin(), line.end(), ','), line.end());
    	fixed_len_read((char*)line.c_str(), record_size, &record);
    	int slot_index = add_fixed_len_page(page, &record);
    	if (slot_index==-1){
    		PageID newpid = alloc_page(heap_file);
    		write_page(page,heap_file,newpid);
    		init_fixed_len_page(page, page_size, record_size);
    		add_fixed_len_page(page, &record);
    	}
    }
    int free_slot = fixed_len_page_freeslots(page);
    int capacity = fixed_len_page_capacity(page);
    if(free_slot != capacity){
    	PageID newpid = alloc_page(heap_file);
    	write_page(page,heap_file,newpid); 
    }

    ftime(&t);
	now_in_ms2 = t.time*1000 + t.millitm;
	total_time += now_in_ms2 - now_in_ms1;
    ifs.close();
    fclose(open_heap_file);
    fprintf(stdout, "TIME: %ld milliseconds\n", total_time);
    return 0;
}