#include <library.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("./write_fixed_len_pages <csv_file> <page_file> <page_size>");
        exit(1);
    }
    char *csv_file = argv[1];
    char *page_file = argv[2];
    int page_size = atoi(argv[3]);
    printf("%s %s %d\n", csv_file,page_file,page_size);
}