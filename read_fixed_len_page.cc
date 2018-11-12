#include "library.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
int main(int argc, char **argv){
	if (argc!=3){
		printf("Usage:read_fixed_len_page <page_file> <page_size>\n");
        exit(1);
	}
	int page_size = atoi(argv[2]);
	FILE *page_file = fopen(argv[1], "rb");
	int record_size = 1000,total_records = 0,total_pages = 0;;
	struct timeb t;
    long now_in_ms1,now_in_ms2,total_time = 0;
    Page *page = (Page *)malloc(sizeof(Page));
    page->data = (char *)malloc(page_size);

    ftime(&t);
    now_in_ms1 = t.time*1000 + t.millitm;
    init_fixed_len_page(page, page_size, record_size);
    int a=fread(page->data, 1, page->page_size, page_file);
    while (!feof(page_file)) {
    	total_pages+=1;
    	for (int i=0;i<fixed_len_page_capacity(page);i++){
    		Record record;
    		read_fixed_len_page(page,i,&record);
    		total_records++;
    		for (int j=0;j<record.size();j++){
    			printf("%s", record[j]);
    			if (j==record.size()-1)printf("\n");
    			else printf(",");
    		}
    	}
    	init_fixed_len_page(page, page_size, record_size);
    	fread(page->data, 1, page->page_size, page_file);
    }
    int free_slot = fixed_len_page_freeslots(page);
    char *last_page = (char *)page->data;
    if (strlen(last_page)>0){
    	total_pages+=1;
    	for (int i=0;i<fixed_len_page_capacity(page)-free_slot;i++){
    		Record record;
    		total_records++;
    		read_fixed_len_page(page,i,&record);
    		for (int j=0;j<record.size();j++){
    			printf("%s", record[j]);
    			if (j==record.size()-1)printf("\n");
    			else printf(",");
    		}
    	}
    }

    ftime(&t);
	now_in_ms2 = t.time*1000 + t.millitm;
	total_time += now_in_ms2 - now_in_ms1;
    fclose(page_file);
    fprintf(stdout, "NUMBER OF RECORDS: %d\n", total_records);
    fprintf(stdout, "NUMBER OF PAGES: %d\n", total_pages);
    fprintf(stdout, "TIME: %ld milliseconds\n", total_time);
    return 0;
}