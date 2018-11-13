#include "library.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>

// Print out all records in a heap file

int main(int argc, char **argv){
	if (argc!=3){
		printf("Usage: ./scan <heapfile> <page_size>\n");
		exit(1);
	}
	int page_size = atoi(argv[2]),record_size = 1000,total_records = 0,total_pages = 0;
	FILE *open_heap_file = fopen(argv[1], "rb");
	struct timeb t;
    long now_in_ms1,now_in_ms2,total_time = 0;
    Heapfile *heap_file = (Heapfile *)malloc(sizeof(Heapfile));
    heap_file->page_size = page_size;
    heap_file->file_ptr = open_heap_file;
    int cur_page = 0,last_dir_page=0;
    Page *page = (Page *)malloc(sizeof(Page));
    page->data = (char *)malloc(page_size);
    init_fixed_len_page(page, page_size, record_size);
    int numberofentry = page_size/8-1;

    ftime(&t);
    now_in_ms1 = t.time*1000 + t.millitm;
    while(cur_page!=(last_dir_page+numberofentry)){
    	if (cur_page%(numberofentry+1)==0){
    		fseek(heap_file->file_ptr, cur_page*page_size, SEEK_SET);
    		fread(&last_dir_page,sizeof(int),1,heap_file->file_ptr);
    		cur_page+=1;
    	}
    	read_page(heap_file,cur_page,page);
    	int free_slot = fixed_len_page_freeslots(page);
    	if (free_slot!=fixed_len_page_capacity(page)) total_pages+=1;
    	for (int i = 0; i<fixed_len_page_capacity(page);i++){
    		Record record;
    		read_fixed_len_page(page,i,&record);
    		// printf("%d\n", record.size());
    		if (record.size()>0 && record[0][0]!='0'){
    			total_records++;
	    		for (int j=0;j<record.size();j++){
	    			printf("%s", record[j]);
	    			if (j==record.size()-1)printf("\n");
	    			else printf(",");
	    		}
	    	}
    	}
    	cur_page+=1;
    	init_fixed_len_page(page, page_size, record_size);
    }

    ftime(&t);
	now_in_ms2 = t.time*1000 + t.millitm;
	total_time += now_in_ms2 - now_in_ms1;
    fclose(open_heap_file);
    fprintf(stdout, "NUMBER OF RECORDS: %d\n", total_records);
    fprintf(stdout, "NUMBER OF DATA PAGES: %d\n", total_pages);
    fprintf(stdout, "TIME: %ld milliseconds\n", total_time);
}