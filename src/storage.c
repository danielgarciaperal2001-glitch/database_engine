#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int storage_create_table_file(const char* table_name) {
    char path[128];
    snprintf(path, sizeof(path), "%s.db", table_name);
    
    FILE* f = fopen(path, "wb");
    if (!f) return -1;
    
    uint64_t offset = 0;
    uint32_t records = 0, free_page = 1;
    fwrite(&offset, 8, 1, f);
    fwrite(&records, 4, 1, f);
    fwrite(&free_page, 4, 1, f);
    
    fclose(f);
    return 0;
}

int storage_insert(const char* table_name, void* data, int size) {
    char path[128];
    snprintf(path, sizeof(path), "%s.db", table_name);
    
    FILE* f = fopen(path, "r+b");
    if (!f) return -1;
    
    uint32_t num_records = 0;
    uint32_t free_page = 0;
    
    // Lee header (ignora warnings con chequeo)
    if (fread(&num_records, 4, 1, f) != 1) goto error;
    if (fread(&free_page, 4, 1, f) != 1) goto error;
    
    // Escribe record
    fseek(f, 16 + (num_records * 68), SEEK_SET);
    if (fwrite(data, size, 1, f) != 1) goto error;
    
    // Actualiza contador
    num_records++;
    fseek(f, 8, SEEK_SET);
    if (fwrite(&num_records, 4, 1, f) != 1) goto error;
    
    fclose(f);
    return num_records - 1;
    
error:
    fclose(f);
    return -1;
}

int storage_select_where(const char* table_name, const char* col, 
                        void* value, int is_int, void** results, int* num_results) {
    char path[128];
    snprintf(path, sizeof(path), "%s.db", table_name);
    
    FILE* f = fopen(path, "rb");
    if (!f) return -1;
    
    uint32_t num_records = 0;
    fseek(f, 8, SEEK_SET);
    if (fread(&num_records, 4, 1, f) != 1) {
        fclose(f);
        return -1;
    }
    
    *num_results = 0;
    char** result_array = calloc(num_records, sizeof(char*));
    if (!result_array) {
        fclose(f);
        return -1;
    }
    
    // Escaneo lineal
    for (uint32_t i = 0; i < num_records; i++) {
        fseek(f, 16 + (i * 68), SEEK_SET);
        char record[68] = {0};
        if (fread(record, 68, 1, f) != 1) continue;
        
        int match = 0;
        if (is_int && strcmp(col, "id") == 0) {
            match = (*(int*)record == *(int*)value);
        } else if (!is_int && strcmp(col, "name") == 0) {
            match = (strcmp(record + 4, (char*)value) == 0);
        }
        
        if (match) {
            char* record_copy = malloc(68);
            if (record_copy) {
                memcpy(record_copy, record, 68);
                result_array[(*num_results)++] = record_copy;
            }
        }
    }
    
    *results = result_array;
    fclose(f);
    return 0;
}

