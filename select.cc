#include "library.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>

int main(int argc, char **argv){
	if (argc<6 || argc>7){
		printf("Usage: ./select <heapfile> <attribute_id> <start> <end> <page_size>\n");
		exit(1);
	}
    bool stdoutFlag = true;
    if (argc == 7 && strcmp(argv[6], "no-stdout") == 0) {
        stdoutFlag = false;
    }
    if(argc == 7 && !(strcmp(argv[6], "no-stdout") == 0) ){
        printf("Usage: ./select <heapfile> <attribute_id> <start> <end> <page_size> 'no-stdout'\n");
        exit(1);
    }
	int page_size = atoi(argv[5]),aid = atoi(argv[2]),record_size = 1000,total_records = 0,total_pages = 0;
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
    while(cur_page<=(last_dir_page+numberofentry)){
        int entoff = cur_page;
        if (cur_page%(numberofentry+1)==0){
            fseek(heap_file->file_ptr, cur_page*page_size, SEEK_SET);
            entoff = cur_page*page_size+8;
            fread(&last_dir_page,sizeof(int),1,heap_file->file_ptr);
            cur_page+=1;
        }
        int pageoff,freespace;
        while(cur_page%(numberofentry+1)!=0){
            fseek(heap_file->file_ptr,entoff,SEEK_SET);
            fread(&pageoff,sizeof(int),1,heap_file->file_ptr);
            fread(&freespace,sizeof(int),1,heap_file->file_ptr);
            if (freespace<page_size){
                read_page(heap_file,pageoff,page);
                total_pages+=1;
                for (int i = 0; i<fixed_len_page_capacity(page);i++){
                    Record record;
                    read_fixed_len_page(page,i,&record);
                    // printf("%d\n", record.size());
                    if (record.size()>0 && record[0][0]!='0'){
                         if (strncmp(record[aid],argv[3],10)>=0 and strncmp(record[aid],argv[4],10)<=0){
                            if(stdoutFlag){
                                std::string stoprint = record[aid];
                                printf("%s\n", stoprint.substr(0,5).c_str());       
                            }
                        total_records++;
                        }
                    }
                }
            }
            cur_page+=1;
            entoff+=8;
        }
        init_fixed_len_page(page, page_size, record_size);
    }


    // while(cur_page!=(last_dir_page+numberofentry)){
    // 	if (cur_page%(numberofentry+1)==0){
    // 		fseek(heap_file->file_ptr, cur_page*page_size, SEEK_SET);
    // 		fread(&last_dir_page,sizeof(int),1,heap_file->file_ptr);
    // 		cur_page+=1;
    // 	}
    // 	read_page(heap_file,cur_page,page);
    // 	int free_slot = fixed_len_page_freeslots(page);
    // 	if (free_slot!=fixed_len_page_capacity(page)) total_pages+=1;
    //     else break;
    // 	for (int i = 0; i<fixed_len_page_capacity(page);i++){
    // 		Record record;
    // 		read_fixed_len_page(page,i,&record);

    // 		if (record.size()>0 && record[0][0]!='0'){
    //             if (strncmp(record[aid],argv[3],10)>=0 and strncmp(record[aid],argv[4],10)<=0){
    //                 if(stdoutFlag){
    //                     std::string stoprint = record[aid];
    //                     printf("%s\n", stoprint.substr(0,5).c_str());       
    //                 }
    //                 total_records++;
    //             }
	   //  	}
    // 	}
    // 	cur_page+=1;
    // 	init_fixed_len_page(page, page_size, record_size);
    // }

    ftime(&t);
	now_in_ms2 = t.time*1000 + t.millitm;
	total_time += now_in_ms2 - now_in_ms1;
    fclose(open_heap_file);
    fprintf(stdout, "NUMBER OF RECORDS: %d\n", total_records);
    fprintf(stdout, "NUMBER OF DATA PAGES: %d\n", total_pages);
    fprintf(stdout, "TIME: %ld milliseconds\n", total_time);
    return 0;
}