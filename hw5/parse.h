//
//  parse.h
//  parse
//
//  Created by Liz LIn on 5/20/18.
//  Copyright © 2018 liz. All rights reserved.
//

#ifndef parse_h
#define parse_h
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>

#define MAX 256
typedef char buffer[MAX];
typedef char * vector[MAX];


enum Mode
{
    PLAIN,
    BACKGROUND,
    PIPE
};

struct command
{
    //buffer name;//name of the command
    char argv[20][50];
    
    int input_redirect;
    int output_redirect;
    int output_append;
    enum Mode mode;//for implmentting part2 , background jobs

};

struct line_of_command
{
    struct command comds[10];
    int num;//for pipe
    char ifile[100];//potential input file for command
    char ofile[100];//potential input file for command
    char command_input[300];
    //int is_in_re;
    //int is_out_re;
    int is_out_append;
};

void get_command_line(char* line);

#endif /* parse_h */
