#include <vector>
typedef const char* V;
typedef std::vector<V> Record;
typedef struct {
	void *data;
        int page_size;
        int slot_size;
   	} Page;

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record){
	return record.size()*10;
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf){
	for (int i = 0; i<record.size(); i++){
		char *my_buf = (char*)buf + 10*i;
		memcpy(my_buf, record[i], strlen(record[i]+1));
	}
}

/**
* Deserializes `size` bytes from the buffer, `buf`, and
* stores the record in `record`.
*/
void fixed_len_read(void *buf, int size, Record *record){
	int record_size = size/10;
	for (int i = 0; i<record_size; i++){
		char *my_buf ;
		strncpy(my_buf, (char *)buf, 10);
		record.push_back(my_buf);
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
    	for(int i = 0; i < my_capacity; i++){
        	page->data[i] = "";
	}
}

/**
* Calculates the maximal number of records that fit in a page
*/
int fixed_len_page_capacity(Page *page){
	return page->page_size/page->page_slot;
}

/**
* Calculate the free space (number of free slots) in the page
*/
int fixed_len_page_freeslots(Page *page){
	int my_capacity = page->page_size/page->page_slot;
	int free_num = 0;
	for (int i = 0; i < my_capacity; i++){
		if (page->data[i] == ""){
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
			if (page->data[i] == ""){
				write_fixed_len_page(page,i,r);
				return i;
			}
		}
	}
	else{
		return -1;
	}
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
	if(page->data[i] == ""){
        char *buf = ((char * )page->data + (slot * (page->slot_size)));
        fixed_len_read(buf, (page->slot_size), r);
    }
}

}
