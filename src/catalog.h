#ifndef CATALOG_H
#define CATALOG_H

#include "types.h"

int catalog_init();
int catalog_create_table(const TableSchema* schema);
TableSchema* catalog_get_schema(const char* table_name);
void catalog_close();

#endif

