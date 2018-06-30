//
//  main.c
//  parse
//
//  Created by Liz LIn on 5/20/18.
//  Copyright © 2018 liz. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "parse.h"

int main(int argc, const char * argv[])
{
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
   
    printf("? ");
    while ((nread = getline(&line, &len, stdin)) != -1) {
        get_command_line(line);
        printf("? ");
    }

    return 0;
}
