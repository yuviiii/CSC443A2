#include "library.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("./write_fixed_len_pages <csv_file> <page_file> <page_size>");
        exit(1);
    }
    char *csv_file_name = argv[1];
    char *page_file_name = argv[2];
    int page_size = atoi(argv[3]);
	long total_time = 0;
	int record_size = 1000, total_record = 0;
	long now_in_ms1,now_in_ms2;
    struct timeb t;
	FILE *page_file = fopen(page_file_name, "w+b");
	std::ifstream ifs;
	std::string line;
	ifs.open(csv_file_name,std::ifstream::in);
	if (!ifs){
		printf("no such csv file\n");
		exit(1);
	}
	ftime(&t);
	now_in_ms1 = t.time*1000 + t.millitm;
	while (std::getline(ifs, line)) {
		total_record+=1;
		if (total_record<10)
			printf("%s\n", line.c_str());
	}

	ftime(&t);
	now_in_ms2 = t.time*1000 + t.millitm;
	total_time += now_in_ms2 - now_in_ms1;
    printf("%s %s %d %d\n", csv_file_name,page_file_name,page_size,total_record);
}