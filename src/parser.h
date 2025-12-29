#ifndef PARSER_H
#define PARSER_H

#include "types.h"

ParsedCommand* parse_command(const char* line);
void free_parsed_command(ParsedCommand* cmd);
void print_parsed_command(const ParsedCommand* cmd);

#endif

