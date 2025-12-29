#include "executor.h"
#include "parser.h"
#include "catalog.h"
#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int execute_command(ParsedCommand* cmd) {
    switch (cmd->type) {
        case CMD_CREATE: {
            TableSchema schema = {0};
            memcpy(schema.table_name, cmd->table, sizeof(schema.table_name) - 1);
            schema.table_name[sizeof(schema.table_name) - 1] = '\0';
            schema.columns = cmd->create_columns;
            schema.num_cols = cmd->create_num_cols;
            schema.record_size = 68; // id + name + padding
            
            if (catalog_create_table(&schema) == 0) {
                storage_create_table_file(cmd->table);
                printf("✅ Table '%s' created\n", cmd->table);
            } else {
                printf("❌ Error creating table '%s'\n", cmd->table);
            }
            break;
        }
        case CMD_INSERT: {
            int id = storage_insert(cmd->table, cmd->insert_data, 68);
            if (id >= 0) {
                printf("✅ Inserted record ID: %d\n", id);
            } else {
                printf("❌ Insert error\n");
            }
            break;
        }
        case CMD_SELECT: {
            void* results = NULL;
            int num_results = 0;
            if (storage_select_where(cmd->table, cmd->where_col, 
                                   cmd->where_is_int ? (void*)&cmd->where_value.where_int : (void*)cmd->where_value.where_text,
                                   cmd->where_is_int, &results, &num_results) == 0) {
                
                printf("+----+--------+\n");
                printf("| ID | Name   |\n");
                printf("+----+--------+\n");
                for (int i = 0; i < num_results; i++) {
                    char* record = ((char**)results)[i];
                    printf("| %2d | %-6s |\n", *(int*)record, record + 4);
                    free(record);
                }
                printf("+----+--------+\n");
            }
            free(results);
            break;
        }
        default:
            printf("❌ Unknown command\n");
    }
    return 0;
}

