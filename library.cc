
#include "library.h"
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <strings.h>


/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record){
	return record->size()*10;
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf){
	for (int i = 0; i<record->size(); i++){
		char *my_buf = (char*)buf + 10*i;
		memcpy(my_buf, record->at(i), strlen(record->at(i)));
	}
}

/**
* Deserializes `size` bytes from the buffer, `buf`, and
* stores the record in `record`.
*/
void fixed_len_read(void *buf, int size, Record *record){
	int record_size = size/10;
	for (int i = 0; i<record_size; i++){
		char *my_buf = (char *)malloc(11) ;
		strncpy(my_buf, (char *)buf, 10);
		record->push_back(my_buf);
		buf = ((char*)buf) + 10;
	}
}

/**
* Initializes a page using the given slot size
*/
void init_fixed_len_page(Page *page, int page_size, int slot_size){
	int my_capacity = page_size/slot_size;
	page->page_size = page_size;
	page->slot_size = slot_size;
	page->data = (char*) malloc(page_size);
    	for(int i = 0; i < page_size; i++){
        	((char *)page->data)[i] = '\0';
	}
}

/**
* Calculates the maximal number of records that fit in a page
*/
int fixed_len_page_capacity(Page *page){
	return page->page_size/page->slot_size;
}

/**
* Calculate the free space (number of free slots) in the page
*/
int fixed_len_page_freeslots(Page *page){
	int my_capacity = page->page_size/page->slot_size;
	int free_num = 0;
	for (int i = 0; i < my_capacity; i++){
		if (((char *)page->data)[i*page->slot_size] == '\0'){
			free_num += 1;
		}	
	}
	return free_num;
}

/**
* Add a record to the page
* Returns:
*   record slot offset if successful,
*   -1 if unsuccessful (page full)
*/
int add_fixed_len_page(Page *page, Record *r){
	int my_capacity = fixed_len_page_capacity(page);
	if (fixed_len_page_freeslots(page) > 0){
		for (int i = 0; i < my_capacity; i++){
			if (((char *)page->data)[i*page->slot_size] == '\0'){
				write_fixed_len_page(page,i,r);
				return i;
			}
		}
	}
	return -1;
}
/**
* Write a record into a given slot.
*/
void write_fixed_len_page(Page *page, int slot, Record *r){
	char *buf = ((char *)page->data + (slot * page->slot_size));
	fixed_len_write(r, buf);
}

/**
* Read a record from the page from a given slot.
*/
void read_fixed_len_page(Page *page, int slot, Record *r){
	if(((char *)page->data)[slot*page->page_size] == '\0'){
        char *buf = ((char * )page->data + (slot * (page->slot_size)));
        fixed_len_read(buf, (page->slot_size), r);
    }
}

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file){	
    for (int i = 0; i < page_size/8; ++i) {
    	int offset = i;
        fwrite(&offset, sizeof(int), 1, file);
        fwrite(&page_size, sizeof(int), 1, file);
    }
    heapfile->file_ptr=file;
	heapfile->page_size = page_size;
    // fflush(file);
}

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile){
	fseek(heapfile->file_ptr,0,SEEK_SET);
	int page_offset,freespace,lastdirpageid = 0;
	while(1){
		fseek(heapfile->file_ptr,lastdirpageid*heapfile->page_size,SEEK_SET);
		fread(&page_offset, sizeof(int), 1, heapfile->file_ptr);
		if(page_offset==lastdirpageid) break;
		lastdirpageid = page_offset;
	}

	fseek(heapfile->file_ptr,lastdirpageid*heapfile->page_size,SEEK_SET);
    fread(&page_offset, sizeof(int), 1, heapfile->file_ptr);
    fread(&freespace, sizeof(int), 1, heapfile->file_ptr);

    for (int i = 1; i < heapfile->page_size/8; ++i) {
        fread(&page_offset, sizeof(int), 1, heapfile->file_ptr);
        fread(&freespace, sizeof(int), 1, heapfile->file_ptr);
        if (freespace == heapfile->page_size) {
            return page_offset;
        }
    }

    int lastdatapage=page_offset;
    fseek(heapfile->file_ptr,lastdirpageid*heapfile->page_size,SEEK_SET);
    fwrite(&lastdatapage+1,sizeof(int),1,heapfile->file_ptr);

    fseek(heapfile->file_ptr,(lastdatapage+1)*heapfile->page_size,SEEK_SET);
    for (int i = 0; i <heapfile->page_size/8; ++i) {
        fwrite(&i+lastdatapage+1, sizeof(int), 1, heapfile->file_ptr);
        fwrite(&heapfile->page_size, sizeof(int), 1, heapfile->file_ptr);
    }

    return lastdatapage+2;
}

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page){
	init_fixed_len_page(page,heapfile->page_size,1000);
	fseek(heapfile->file_ptr, pid*heapfile->page_size, SEEK_SET);
	fread(page->data,heapfile->page_size,1,heapfile->file_ptr);
}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid){
	fseek(heapfile->file_ptr, pid * heapfile->page_size, SEEK_SET);
	fwrite(page->data, heapfile->page_size, 1, heapfile->file_ptr);
	int page_offset=0,freespace;
	int pageleft = pid;
	while(pageleft>(page->page_size/8-1)){
		fseek(heapfile->file_ptr, page_offset*page->page_size, SEEK_SET);
		fread(&page_offset,sizeof(int),1,heapfile->file_ptr);
		pageleft-=(page->page_size/8-1);
	}
	fseek(heapfile->file_ptr, page_offset*page->page_size, SEEK_SET);
	while(1){
		fread(&page_offset,sizeof(int),1,heapfile->file_ptr);
		if (page_offset==pid){
			break;
		}
		fread(&freespace,sizeof(int),1,heapfile->file_ptr);
	}
	
	freespace = 1000*fixed_len_page_freeslots(page);
	fseek(heapfile->file_ptr,0,SEEK_CUR);
	fwrite(&freespace, sizeof(int), 1, heapfile->file_ptr);
	// fflush(heapfile->file_ptr);
}