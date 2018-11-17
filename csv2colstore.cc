#include "library.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>
int main(int argc, char **argv){
	if (argc!=4){
		printf("Usage: csv2colstore <csv_file> <heap_dir> <page_size>\n");
		exit(1);
	}
	int page_size = atoi(argv[3]);
	int record_size = 1000,total_records = 0,total_pages = 0;
    Page *page = (Page *)malloc(sizeof(Page));
    page->data = (char *)malloc(page_size);
    init_fixed_len_page(page, page_size, record_size);
    Heapfile *heap_file = (Heapfile *)malloc(sizeof(Heapfile));
    std::string line,strtowrite="";
    struct timeb t;
    mkdir(argv[2],S_IRWXU);
    long now_in_ms1,now_in_ms2,total_time = 0;
    ftime(&t);
    now_in_ms1 = t.time*1000 + t.millitm;
    std::vector<Record> v;
    std::ifstream ifs;
    ifs.open(argv[1],std::ifstream::in);
    int num_r = 0;
    while (std::getline(ifs, line)){
        Record r;
        line.erase(std::remove(line.begin(), line.end(), ','), line.end());
        fixed_len_read((char *)line.c_str(),1000,&r);
        num_r+=1;
        v.push_back(r);
    }
    ifs.close();
    for (int i=0;i<100;i++){
        char path[100];
        strcpy(path,argv[2]);
        strcat(path,"/column");
        strcat(path,(std::to_string(i)).c_str());

        FILE *open_heap_file = fopen(path, "wb+");
        init_fixed_len_page(page, page_size, record_size);
        init_heapfile(heap_file,page_size,open_heap_file);
        strtowrite="";
        int numline = 0;
        while (numline!=num_r){
        	Record record;
            record.clear();
            strtowrite = strtowrite+v.at(numline).at(i);
            numline+=1;
            if (numline%100==0){
                fixed_len_read((char*)strtowrite.c_str(), strtowrite.length(), &record);
                int slot_index = add_fixed_len_page(page, &record);
                if (slot_index==-1){
                    PageID newpid = alloc_page(heap_file);
                    total_pages+=1;
                    write_page(page,heap_file,newpid);
                    init_fixed_len_page(page, page_size, record_size);
                    add_fixed_len_page(page, &record);
                }
                strtowrite="";
                record.clear();
            }
        }
        if (numline%100!=0){
            Record record;
            record.clear();
            fixed_len_read((char*)strtowrite.c_str(), strtowrite.length(), &record);
            int slot_index = add_fixed_len_page(page, &record);
            if (slot_index==-1){
                PageID newpid = alloc_page(heap_file);
                total_pages+=1;
                write_page(page,heap_file,newpid);
                init_fixed_len_page(page, page_size, record_size);
                add_fixed_len_page(page, &record);
            }
            record.clear();
        }
        int free_slot = fixed_len_page_freeslots(page);
        int capacity = fixed_len_page_capacity(page);
        if(free_slot != capacity){
        	PageID newpid = alloc_page(heap_file);
        	total_pages+=1;
        	write_page(page,heap_file,newpid); 
            init_fixed_len_page(page, page_size, record_size);
        }
        fclose(open_heap_file);
    }

    ftime(&t);
	now_in_ms2 = t.time*1000 + t.millitm;
	total_time += now_in_ms2 - now_in_ms1;


    fprintf(stdout, "NUMBER OF RECORDS: %d\n", num_r);
    fprintf(stdout, "NUMBER OF DATA PAGES: %d\n", total_pages);
    fprintf(stdout, "TIME: %ld milliseconds\n", total_time);
    return 0;

}