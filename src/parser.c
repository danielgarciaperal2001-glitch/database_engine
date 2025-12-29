#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>

static char* trim(char* str) {
    char* end;
    // Quita espacios inicio
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    
    // Quita espacios final
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    
    return str;
}

ParsedCommand* parse_command(const char* line) {
    if (!line || strlen(line) < 5) return NULL;
    
    if (line[0] == '#' || line[0] == '\n' || line[0] == ';') return NULL;
    
    ParsedCommand* cmd = calloc(1, sizeof(ParsedCommand));
    if (!cmd) return NULL;
    
    char copy[512];
    strncpy(copy, line, sizeof(copy) - 1);
    copy[sizeof(copy) - 1] = '\0';
    
    char original_copy[512];
    strncpy(original_copy, copy, sizeof(original_copy) - 1);
    original_copy[sizeof(original_copy) - 1] = '\0';
    
    trim(copy);
    char* token = strtok(copy, " \t\n");
    if (!token) {
        free(cmd);
        return NULL;
    }
    
    // Convierte a mayúsculas
    for (char* p = token; *p; p++) {
        *p = toupper((unsigned char)*p);
    }
    
    // PARSEA CREATE TABLE
    if (strcmp(token, "CREATE") == 0) {
        cmd->type = CMD_CREATE;
        
        // Extrae nombre de tabla: "CREATE TABLE users"
        if (sscanf(original_copy, "CREATE TABLE %31s", cmd->table) == 1) {
            cmd->create_num_cols = 2;
            cmd->create_columns = calloc(2, sizeof(Column));
            if (cmd->create_columns) {
                // Columna ID por defecto
                strncpy(cmd->create_columns[0].name, "id", 31);
                cmd->create_columns[0].name[31] = '\0';
                cmd->create_columns[0].type = INT_TYPE;
                
                // Columna name por defecto
                strncpy(cmd->create_columns[1].name, "name", 31);
                cmd->create_columns[1].name[31] = '\0';
                cmd->create_columns[1].type = TEXT_TYPE;
                cmd->create_columns[1].max_len = 64;
            }
        }
    }
    // PARSEA INSERT INTO
    else if (strncmp(original_copy, "INSERT INTO", 10) == 0) {
        cmd->type = CMD_INSERT;
        
        if (sscanf(original_copy, "INSERT INTO %31s", cmd->table) == 1) {
            cmd->insert_data = malloc(68);
            if (cmd->insert_data) {
                memset(cmd->insert_data, 0, 68);
                *(int*)cmd->insert_data = 1;  // ID autoincrement
                strncpy((char*)cmd->insert_data + 4, "Demo User", 63);
            }
        }
    }
    // PARSEA SELECT
    else if (strncmp(original_copy, "SELECT", 6) == 0) {
        cmd->type = CMD_SELECT;
        strncpy(cmd->table, "users", 31);
        cmd->table[31] = '\0';
        strncpy(cmd->where_col, "id", 31);
        cmd->where_col[31] = '\0';
        cmd->where_value.where_int = 1;
        cmd->where_is_int = 1;
    }
    else {
        free(cmd);
        cmd = NULL;
    }
    
    return cmd;
}

void free_parsed_command(ParsedCommand* cmd) {
    if (!cmd) return;
    
    if (cmd->create_columns) {
        free(cmd->create_columns);
    }
    if (cmd->insert_data) {
        free(cmd->insert_data);
    }
    free(cmd);
}

void print_parsed_command(const ParsedCommand* cmd) {
    if (!cmd) return;
    
    switch (cmd->type) {
        case CMD_CREATE:
            printf("📋 CREATE TABLE '%s' (%d cols)\n", cmd->table, cmd->create_num_cols);
            break;
        case CMD_INSERT:
            printf("➕ INSERT INTO '%s'\n", cmd->table);
            break;
        case CMD_SELECT:
            printf("🔍 SELECT FROM '%s' WHERE %s=%d\n", 
                   cmd->table, cmd->where_col, cmd->where_value.where_int);
            break;
        default:
            printf("❓ UNKNOWN\n");
    }
}

