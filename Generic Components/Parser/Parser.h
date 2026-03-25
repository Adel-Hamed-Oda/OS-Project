#ifndef PARSER_H
#define PARSER_H

// parses an entire file, and executes all the commands in it
void parse_file(char *filename);

// parses a single line, and executes the command in it
void parse_line(char *line);

// separates a string by spaces, and returns an array of strings alongside the count of how many strings there are
char** separate_by_spaces(char *input_string, int *count);

#endif