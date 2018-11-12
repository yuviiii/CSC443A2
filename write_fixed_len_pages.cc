#include "library.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Usage: ./write_fixed_len_pages <csv_file> <page_file> <page_size>");
        exit(1);
    }

    char *csv_file_name = argv[1];
    char *page_file_name = argv[2];
    int page_size = atoi(argv[3]);
    std::ifstream ifs;
    FILE *page_file = fopen(page_file_name, "w+b");
    int record_size = 1000;
    Page *page = (Page *)malloc(sizeof(Page));
    page->data = (char *)malloc(page_size);
    init_fixed_len_page(page, page_size, record_size);
    int total_records = 0,total_pages = 0;
    std::string line;
    struct timeb t;
    long now_in_ms1,now_in_ms2,total_time = 0;

    ifs.open(csv_file_name, std::ifstream::in);
    ftime(&t);
    now_in_ms1 = t.time*1000 + t.millitm;
    while (std::getline(ifs, line)) {
        Record record;
        line.erase(std::remove(line.begin(), line.end(), ','), line.end());
        fixed_len_read((char*)line.c_str(), record_size, &record);
        int index = add_fixed_len_page(page, &record);
        if (index == -1) {
            total_pages++;
            fwrite(page->data, 1, page->page_size, page_file);
            fflush(page_file);
            init_fixed_len_page(page, page_size, record_size);
            add_fixed_len_page(page, &record);
        }
        total_records++;
    }

    int free_slot = fixed_len_page_freeslots(page);
    int capacity = fixed_len_page_capacity(page);
    if(free_slot != capacity){
            total_pages++;
            fwrite(page->data, 1, record_size*(capacity-free_slot), page_file);
    }

    ftime(&t);
	now_in_ms2 = t.time*1000 + t.millitm;
	total_time += now_in_ms2 - now_in_ms1;
    ifs.close();
    fclose(page_file);

    fprintf(stdout, "NUMBER OF RECORDS: %d\n", total_records);
    fprintf(stdout, "NUMBER OF PAGES: %d\n", total_pages);
    fprintf(stdout, "TIME: %ld milliseconds\n", total_time);
    return 0;
}