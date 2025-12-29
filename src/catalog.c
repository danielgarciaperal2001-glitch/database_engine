#include "catalog.h"
#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE* catalog_file = NULL;
static TableSchema* schemas[32];
static int num_schemas = 0;

int catalog_init() {
    catalog_file = fopen(CATALOG_FILE, "r+b");
    if (!catalog_file) {
        catalog_file = fopen(CATALOG_FILE, "wb");
        if (catalog_file) fclose(catalog_file);
        catalog_file = fopen(CATALOG_FILE, "r+b");
    }
    return catalog_file != NULL;
}

int catalog_create_table(const TableSchema* schema) {
    if (!catalog_file) return -1;
    
    // Escribe schema directamente al final
    fseek(catalog_file, 0, SEEK_END);
    fwrite(schema->table_name, 32, 1, catalog_file);
    fwrite(&schema->num_cols, 4, 1, catalog_file);
    fwrite(schema->columns, sizeof(Column), schema->num_cols, catalog_file);
    
    // Cache en memoria
    schemas[num_schemas++] = malloc(sizeof(TableSchema));
    *schemas[num_schemas-1] = *schema;
    schemas[num_schemas-1]->columns = malloc(sizeof(Column) * schema->num_cols);
    memcpy(schemas[num_schemas-1]->columns, schema->columns, 
           sizeof(Column) * schema->num_cols);
    
    fclose(catalog_file);
    catalog_file = fopen(CATALOG_FILE, "r+b");
    return 0;
}

TableSchema* catalog_get_schema(const char* table_name) {
    for (int i = 0; i < num_schemas; i++) {
        if (strcmp(schemas[i]->table_name, table_name) == 0)
            return schemas[i];
    }
    return NULL;
}

void catalog_close() {
    if (catalog_file) fclose(catalog_file);
    for (int i = 0; i < num_schemas; i++) {
        free(schemas[i]->columns);
        free(schemas[i]);
    }
}

