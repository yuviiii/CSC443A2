#include "library.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>

int main(int argc, char **argv){
	if (argc<7 || argc>8){
		printf("Usage: ./select3 <colstore_name> <attribute_id> <return_aid> <start> <end> <page_size>\n");
		exit(1);
	}
    bool stdoutFlag = true;
    if (argc == 8 && strcmp(argv[7], "no-stdout") == 0) {
        stdoutFlag = false;
    }
    if(argc == 8 && !(strcmp(argv[7], "no-stdout") == 0) ){
        printf("Usage: ./select3 <colstore_name> <attribute_id> <return_aid> <start> <end> <page_size> 'no-stdout'\n");
        exit(1);
    }
    char path[100],path1[100];
    strcpy(path,argv[1]);
    strcat(path,"/column");
    strcat(path,(std::to_string(atoi(argv[2]))).c_str());
    strcpy(path1,argv[1]);
    strcat(path1,"/column");
    strcat(path1,(std::to_string(atoi(argv[3]))).c_str());
	int page_size = atoi(argv[6]),record_size = 1000,total_records = 0,total_pages = 0;
	FILE *open_heap_file = fopen(path, "rb");
    FILE *open_heap_file1 = fopen(path1, "rb");
	struct timeb t;
    long now_in_ms1,now_in_ms2,total_time = 0;
    Heapfile *heap_file = (Heapfile *)malloc(sizeof(Heapfile));
    Heapfile *heap_file1 = (Heapfile *)malloc(sizeof(Heapfile));
    heap_file->page_size = page_size;
    heap_file->file_ptr = open_heap_file;
    heap_file1->page_size = page_size;
    heap_file1->file_ptr = open_heap_file1;
    int cur_page = 0,last_dir_page=0;
    Page *page = (Page *)malloc(sizeof(Page));
    page->data = (char *)malloc(page_size);
    init_fixed_len_page(page, page_size, record_size);
    Page *page1 = (Page *)malloc(sizeof(Page));
    page1->data = (char *)malloc(page_size);
    init_fixed_len_page(page, page_size, record_size);
    init_fixed_len_page(page1, page_size, record_size);
    int numberofentry = page_size/8-1;

    ftime(&t);
    now_in_ms1 = t.time*1000 + t.millitm;
    while(cur_page!=(last_dir_page+numberofentry)){

    	if (cur_page%(numberofentry+1)==0){
    		fseek(heap_file->file_ptr, cur_page*page_size, SEEK_SET);
    		fread(&last_dir_page,sizeof(int),1,heap_file->file_ptr);
            // fseek(heap_file1->file_ptr, cur_page*page_size, SEEK_SET);
            // fread(&last_dir_page,sizeof(int),1,heap_file->file_ptr);
    		cur_page+=1;
    	}
    	read_page(heap_file,cur_page,page);
        read_page(heap_file1,cur_page,page1);
    	int free_slot = fixed_len_page_freeslots(page);
    	if (free_slot!=fixed_len_page_capacity(page)) total_pages+=1;
        else break;
    	for (int i = 0; i<fixed_len_page_capacity(page);i++){
    		Record record,record1;
    		read_fixed_len_page(page,i,&record);
            read_fixed_len_page(page1,i,&record1);
    		if (record.size()>0 && record[0][0]!='0'){
                for (int k=0;k<record.size();k++){
                    if (strncmp(record[k],argv[4],10)>=0 and strncmp(record[k],argv[5],10)<=0){
                        if(stdoutFlag){
                            std::string stoprint = record1[k];
                            printf("%s\n", stoprint.substr(0,5).c_str()); 
                        }
                        
                        total_records++;
                    }
                }
	    	}
    	}
    	cur_page+=1;
    	init_fixed_len_page(page, page_size, record_size);
        init_fixed_len_page(page1, page_size, record_size);
    }

    ftime(&t);
	now_in_ms2 = t.time*1000 + t.millitm;
	total_time += now_in_ms2 - now_in_ms1;
    fclose(open_heap_file);
    fprintf(stdout, "NUMBER OF RECORDS: %d\n", total_records);
    fprintf(stdout, "NUMBER OF DATA PAGES: %d\n", total_pages);
    fprintf(stdout, "TIME: %ld milliseconds\n", total_time);
    return 0;
}