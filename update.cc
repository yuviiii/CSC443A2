#include "library.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>

// Insert all records in the CSV file to a heap file

int main(int argc, char **argv){
	if (argc!=6){
		printf("Usage: ./update <heapfile> <record_id> <attribute_id> <new_value> <page_size>\n");
		exit(1);
	}
	int page_size = atoi(argv[5]),record_size = 1000,rid = atoi(argv[2]),aid = atoi(argv[3]),slotid,finalpid,freespace;
	FILE *open_heap_file = fopen(argv[1], "rb+");
    Page *page = (Page *)malloc(sizeof(Page));
    page->data = (char *)malloc(page_size);
    init_fixed_len_page(page, page_size, record_size);
	struct timeb t;
    long now_in_ms1,now_in_ms2,total_time = 0;
    Heapfile *heap_file = (Heapfile *)malloc(sizeof(Heapfile));
    heap_file->page_size = page_size;
    heap_file->file_ptr = open_heap_file;
    int numberofentry = page_size/8-1,numofrecord = page_size/1000;
    // int slotid = (rid+1)%numofrecord-1;
    // if (slotid==-1) slotid = numofrecord-1;
    // int pid = (rid)/numofrecord;
    // int finalpid =pid + (pid)/numberofentry+1;

    int curdir=0,nextdir,rleft = rid,notfind=1;
    while(notfind){
        fread(&nextdir,sizeof(int),1,heap_file->file_ptr);
        fread(&freespace,sizeof(int),1,heap_file->file_ptr);
        int iter = 0;
        // printf("nextdir%d\n", nextdir);
        while(iter<numberofentry and notfind){
            iter+=1;
            // printf("%d\n", finalpid);
            fread(&finalpid,sizeof(int),1,heap_file->file_ptr);
            fread(&freespace,sizeof(int),1,heap_file->file_ptr);
            int recordnum = (page_size-freespace)/1000;
            if (rleft<recordnum){
                notfind = 0;
            }
            else rleft-=recordnum;
            // printf("%d  %d\n", finalpid,rleft);
        }
        fseek(heap_file->file_ptr,nextdir*page_size,SEEK_SET);
    }
    read_page(heap_file, finalpid, page);
    // printf("%d. %d\n", finalpid,rleft);
    
    for (int i=0;i<fixed_len_page_capacity(page);i++){
        Record record;
        read_fixed_len_page(page,i,&record);
        if (record.size()>0 && record[0][0]!='0'){
            if(rleft==0){
                for (int j=0;j<record.size();j++){
                    printf("%s", record[j]);
                    // memset((char *)record[j],'0',10);
                    if (j==record.size()-1)printf("\n");
                    else printf(",");
                }
                memset((char *)record[aid],'0',10);
                strncpy((char *)record[aid],argv[4],10);
                write_fixed_len_page(page,slotid,&record);
                write_page(page,heap_file,finalpid);
                return 0;
            }else rleft-=1;
        }
    }

 //    read_page(heap_file, finalpid, page);
 //    Record record;
 //    read_fixed_len_page(page,slotid,&record);
 //    memset((char *)record[aid],'0',10);
 //    strncpy((char *)record[aid],argv[4],10);
	// write_fixed_len_page(page,slotid,&record);
	// write_page(page,heap_file,finalpid);
    return 0;
}