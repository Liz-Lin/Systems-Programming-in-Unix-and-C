//
//  main-parse.c
//  parse
//
//  Created by Liz LIn on 6/1/18.
//  Copyright © 2018 liz. All rights reserved.
//

#include "wayne_parse.h"
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/wait.h>
/*
 ** A very simple main program that re-prints the line after it's been scanned and parsed.
 */

// readable and writable check for respective operation
#define MAX 256
typedef char buffer[MAX];
int exit_status=0;


int readable(char * file_name)
{
    struct stat buf;
    if (stat(file_name, &buf) < 0) return 0;
    return buf.st_mode & S_IRUSR;
}

int writable(char * file_name)
{ // actually, this function is wrong, but it will work for now
    struct stat buf;
    if (stat(file_name, &buf) < 0) return 1;
    return buf.st_mode & S_IWUSR;
}
// verify command line checks to besure pipes make sense

int verify_command_line(struct commandLine * cmd)
{

    if (cmd->infile && !readable(cmd->infile))
    {
        fprintf(stderr, "Unable to open input file '%s'\n", cmd->infile);
        return 0;
    }
    if (cmd->outfile && !writable(cmd->outfile))
    {
        fprintf(stderr, "Unable to open output file '%s'\n", cmd->outfile);
        return 0;
    }

    if (cmd->append && !writable(cmd->outfile))
    {
        fprintf(stderr, "Unable to open output file for appending '%s'\n", cmd->outfile);
        return 0;
    }
    return 1;
}

int set_child_io(struct commandLine * cmd, int * in, int * out)
{
    if (cmd->infile)
    {
        *in = open(cmd->infile, O_RDONLY);
        if(*in == -1)
        {
            perror ("wrong input file");
	    return 0;
        }
        //printf("in is: %d\n", *in);
        dup2(*in, 0);
        close(*in);
    }
    if (cmd->outfile)
    {
        if(cmd->append ==0)
            *out = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        else
            *out = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(*out == -1)
        {
            perror("wrong output file");
            return 0;
        }
        //printf("out is: %d\n", *out);
        dup2(*out, 1);
        close(*out);
    }
}

void run_command_with_pipes(struct commandLine* cmd) {
    int in = 0, out = 1,num_cmd = cmd->numCommands;
    int num_pipe = num_cmd -1;
    int status;
    int i = 0;
    pid_t pid;
    
    int fd[2*num_pipe];
    
    for(i = 0; i < (num_pipe); i++){
        if(pipe(fd + i*2) < 0) {
            perror("couldn't pipe");
            return;
            //exit(EXIT_FAILURE);
        }
    }
    //ls -l | grep o | foo
    int index =0,j = 0;
    while(index < num_cmd) {//maybe
        
        
        pid = fork();
        if(pid < 0)
        {
            perror("fork faild");
            return;
            //exit(EXIT_FAILURE);
        }
         else if(pid == 0)
         {//child work
             if(index ==0 && cmd->infile)//first command might have input redirect
             {
                 in = open(cmd->infile, O_RDONLY);
                 if(in == -1)
                 {
                     printf("wrong input file\n");
                     return;
                 }
                 dup2(in, 0);
                 close(in);
             }
             else if((index == num_pipe) && cmd->outfile) //last command might have output redirect
             {
                 if(cmd->append ==0)
                     out = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                 else
                     out = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                 if(out == -1)
                 {
                     printf("wrong output file\n");
                     return;
                 }
                 dup2(out, 1);
                 close(out);
             }
            //first command no read end dup,
            //if not last command,
            if(index != num_pipe)
            {//last command no write end dup
                if(dup2(fd[j + 1], 1) < 0){//need to dup wirte end pipe to stdout
                    perror("dup2");
                    return;
                    //exit(EXIT_FAILURE);
                }
            }
            
            if(index != 0 )
            {
                if(dup2(fd[j-2], 0) < 0){//need to dup read end pipe to stdin except the first one
                    perror(" dup2");
                    return;
                    //exit(EXIT_FAILURE);
                }
            }
            
            for(i = 0; i < 2*num_pipe; i++){//close all ends
                close(fd[i]);
            }
            
            if( execvp(cmd->argv[cmd->cmdStart[index]], &cmd->argv[cmd->cmdStart[index]]) < 0 )
            {
                perror(cmd->argv[cmd->cmdStart[index]]);
                return;
                //exit(EXIT_FAILURE);
            }
        }
        
        index++;
        j+=2;
    }
    /**Parent closes the pipes and wait for children*/
    
    for(i = 0; i < 2 * num_pipe; i++){
        close(fd[i]);
    }
    
    for(i = 0; i < num_cmd; i++)
        wait(&status);
    if (WIFEXITED(status))
    {
        exit_status = WEXITSTATUS(status);
    }
}

void do_cd (struct commandLine *cmd)
{
    if (cmd->argv[1] == NULL)
        chdir(getenv("HOME"));
    else{
        if(strcmp(cmd->argv[1], "~") == 0)
            chdir(getenv("HOME"));
        else{
            if (chdir(cmd->argv[1]) == -1){
                perror(cmd->argv[1]);
                exit_status = 1;
            }
        }
    }

}
int execute_given_command (struct commandLine *cmd, char* envp[])
{
    int status=0, child_pid=0, in=0, out=1,is_pipe =0;
    if(cmd->numCommands ==0)
        return 0;
    if (!verify_command_line(cmd))
        return 0;
  
    if(strcmp(cmd->argv[0],"exit") == 0)
    {
        if(cmd->argv[1] != NULL)
            exit_status = atoi(cmd->argv[1]);
        exit(exit_status);
    }
    if(strcmp(cmd->argv[0],"cd") == 0)
    {
        do_cd(cmd);
        return 0;
    }
    if(cmd -> numCommands >1)//take care of pipe
        run_command_with_pipes(cmd);
    else if (cmd ->numCommands == 1)//no pipe
    {
        child_pid = fork();
        if(child_pid ==-1)
        {
            perror("fork faild");
            exit_status =1;
            return exit_status;
            
        }
        else if (child_pid !=0 )//parent
        {
            wait(&status);
            if (in != 0)
                close(in);
            if (out != 1)
                close(out);
            if (WIFEXITED(status))
            {
                exit_status = WEXITSTATUS(status);
            }
        }
        else//child
        {
            set_child_io(cmd, &in, &out);
            execvp(cmd->argv[cmd->cmdStart[0]], &cmd->argv[cmd->cmdStart[0]] );
            perror(cmd->argv[cmd->cmdStart[0]]);
            return 1;
            
        }
    }
    return status;
}

int main(int argc, char *argv[], char *envp[])
{
    FILE *input;
    char line[MAX_LINE];
    
    if(argc == 2)
    {
        input = fopen(argv[1], "r");
        if(input != NULL)
        {
            char *line_file = NULL;
            size_t len =0;
            ssize_t read;
            while((read = getline(&line_file, &len,input)) != -1)
            {
                if (strlen(line_file) == 0)
                    continue;
                const char s[2] = ";";
                char *token;
                
                /* get the first token */
                token = strtok(line_file, s);
                
                /* walk through other tokens */
                while( token != NULL ) {
                    
                    struct commandLine cmdLine;
                    if(token[strlen(token)-1] == '\n')
                        token[strlen(token)-1] = '\0';   /* zap the newline */
                    else
                        token[strlen(token)]= '\0';
                    
                    Parse(token, &cmdLine);
                    
                    execute_given_command(&cmdLine, envp);
                    token = strtok(NULL, s);
                }
            }
        }
        else
            perror("can't open file");
    }
    else
    {
        assert(argc == 1);
        input = stdin;
        printf("? ");
        /* By default, printf will not "flush" the output buffer until
         * a newline appears.  Since the prompt does not contain a newline
         * at the end, we have to explicitly flush the output buffer using
         * fflush.
         */
        fflush(stdout);
        setlinebuf(input);
        while(fgets(line, sizeof(line), input))
        {
            
            if (strlen(line) == 0)
            {
                printf("? ");
                fflush(stdout);
                continue;
            }
            const char s[2] = ";";
            char *token;
            
            /* get the first token */
            token = strtok(line, s);
            
            /* walk through other tokens */
            while( token != NULL ) {
                
                struct commandLine cmdLine;
                if(token[strlen(token)-1] == '\n')
                    token[strlen(token)-1] = '\0';   /* zap the newline */
                else
                    token[strlen(token)]= '\0';
                
                Parse(token, &cmdLine);
                
                execute_given_command(&cmdLine, envp);
                token = strtok(NULL, s);
            }
            
            if(input == stdin)
            {
                printf("? ");
                fflush(stdout);
            }
        }
    }
    
    return 0;
}
