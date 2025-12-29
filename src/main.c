#include "parser.h"
#include "catalog.h"
#include "storage.h"
#include "executor.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("Usage: ./bin/tinydb [script.sql]\n");
        return 0;
    }
    
    catalog_init();
    
    FILE* input = stdin;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror("Error opening file");
            return 1;
        }
    }
    
    char line[256];
    while (fgets(line, sizeof(line), input)) {
        line[strcspn(line, "\n")] = 0; // Remove \n
        
        ParsedCommand* cmd = parse_command(line);
        if (cmd && cmd->type != CMD_UNKNOWN) {
            execute_command(cmd);
            free_parsed_command(cmd);
        }
    }
    
    if (input != stdin) fclose(input);
    catalog_close();
    return 0;
}

