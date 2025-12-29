#ifndef STORAGE_H
#define STORAGE_H

#include "types.h"

int storage_create_table_file(const char* table_name);
int storage_insert(const char* table_name, void* data, int size);
int storage_select_where(const char* table_name, const char* col, void* value, int is_int, 
                        void** results, int* num_results);

#endif

