#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef enum {
    INT_TYPE,
    TEXT_TYPE
} ColumnType;

typedef struct {
    char name[32];
    ColumnType type;
    int max_len;  // Para TEXT
} Column;

typedef struct {
    char table_name[32];
    Column* columns;
    int num_cols;
    int record_size;
} TableSchema;

typedef enum {
    CMD_CREATE,
    CMD_INSERT,
    CMD_SELECT,
    CMD_UNKNOWN
} CommandType;

typedef struct {
    CommandType type;
    char table[32];
    
    // CREATE
    Column* create_columns;
    int create_num_cols;
    
    // INSERT
    void* insert_data;
    
    // SELECT
    char where_col[32];
    union {
        int where_int;
        char where_text[64];
    } where_value;
    int where_is_int;
} ParsedCommand;

#define PAGE_SIZE 4096
#define CATALOG_FILE "catalog.db"

#endif

