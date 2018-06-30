#include "parse.h"
buffer arg_buffer[MAX];
char* where;
char buff[50];
void clean_buff()
{
    for(int i=0; i < 50; ++i)
        buff[i] = 0;
}
void print_command (struct line_of_command lc)
{
    //printf("input: %s", lc->command_input);
    
    printf ("%d: ", ((lc.num)+1));
    //printf ("%c: ", (lc->ifile)[0] );
    if(strlen(lc.ifile) >0) //((lc.ifile)[0] != '\0')
        printf("< '%s' ",lc.ifile);
    
    for (int i=0; i< (lc.num); ++i)
    {
        //printf("\ncommand name %d: %s",i, lc->comds[i].name);
        for (int j=0;lc.comds[i].argv[j][0] != 0; ++j )
            printf(" '%s' ",  lc.comds[i].argv[j]);
        printf(" | ");
    }
    for (int j=0;lc.comds[lc.num].argv[j][0] != 0; ++j )
        printf(" '%s' ",  lc.comds[lc.num].argv[j]);
    
    if (strlen(lc.ofile) >0)//((lc.ofile)[0] != '\0')
    {
        if (lc.is_out_append ==1)
            printf(">>'%s' ",lc.ofile);
        else
            printf(">'%s' ",lc.ofile);
    }
    printf("\n");
}

const char delim[]="<>&|;";
char * get_word(char * word, char* line)
{
    
    char ch;
    char * p = word;
    for (;;)//get rid of white spaces
    {
        if (line ==NULL)
            return NULL;
        ch = (*line);
        ++line;
        if (ch != ' ' && ch != '\t')
            break;
        
    }
    if (ch == '\n' || ch == '\0')
        return NULL;
    
    *p = ch;
    //printf("%c", *p);
    p++;
    if (strchr(delim, ch) == NULL)//ch is not &, |, <, >, ;
    {
        for (;;)
        {
            if (line ==NULL)
                return NULL;
            ch =*line;
            line++;
            
            if (isspace(ch) || strchr(delim, ch))//
                break;
            *p = ch;
            p++;
            
            
        }
    }
    
    if (ch == '&' && isdigit(*line))// need to check if the next one is digit
    {
        *p++ = *line++;
    }
    *p = 0;
    where = line;
    return word;
}

void get_command_line(char* line)
{
    
    where = line;
    struct line_of_command lc;
    lc.num = 0;
    lc.is_out_append=0;
    lc.ifile[0] = '\0';
    lc.ofile[0] = '\0';
    lc.command_input[0] = '\0';
    strcpy(lc.command_input, line);
    buffer * arg_buf_ptr = arg_buffer ;
    
    bool is_done= false;
    bool is_cmd=false;
    while (!is_done)
    {
        clean_buff();
        int idx=0;
        line = where;
        //printf("start of while %d\n", idx); //for different comds
        char* token = buff;

        if ((token = get_word(*arg_buf_ptr,line )) == NULL)//no command
            break;
        is_cmd=true;
        //strcpy(lc.comds[lc.num].name,token);
        strcpy(lc.comds[lc.num].argv[idx],token);
        lc.comds[lc.num].input_redirect=0;
        lc.comds[lc.num].output_redirect=0;
        lc.comds[lc.num].output_append=0;
        
        //printf("argv %d %s:\n", idx, lc.comds[lc.num].argv[idx]);
        idx++;
        line = where;
        clean_buff();
        for (; (token = get_word(*arg_buf_ptr,line));  arg_buf_ptr++, line = where)
        {
            line= where;
            if (!strcmp(token, "<"))
            {
                lc.comds[lc.num].input_redirect = 1;
                
                if (*line == '<')//means << or  treat it like <
                    where++;
                clean_buff();
                line = where;
                token= get_word(*arg_buf_ptr,line);
                if (token == NULL)
                    strcpy(lc.ifile, " ");
                else if (!strcmp(token, "|"))
                {
                    strcpy(lc.ifile, " ");
                    lc.comds[lc.num].mode=PIPE;
                    lc.num++;
                    break;
                }
                else
                {
                    strcpy(lc.ifile,token);
                    
                }
                clean_buff();
               
            }//
            else if (!strcmp(token, ">"))
            {
                if (*line == '>')//means << or  treat it like <
                {
                    where++;
                    lc.is_out_append =1;
                    lc.comds[lc.num].output_append =1;
                }
                lc.comds[lc.num].output_redirect=1;
                clean_buff();
                line = where;
                token= get_word(*arg_buf_ptr,line);
                if (token == NULL)
                    strcpy(lc.ofile, " ");
                else if (!strcmp(token, "|"))
                {
                    strcpy(lc.ofile, " ");
                    lc.comds[lc.num].mode=PIPE;
                    lc.num++;
                    break;
                }
                else
                    strcpy(lc.ofile,token);
             
                clean_buff();

            }
            else if (!strcmp(token,"&"))
            {
                
                lc.comds[lc.num].mode = BACKGROUND;
                strcpy(lc.comds[lc.num].argv[idx], "&");
                idx++;
            }
            else if (!strcmp(token, "|"))
            {
                lc.comds[lc.num].mode=PIPE;
                lc.num++;
                break;
            }
            else if (!strcmp(token, ";"))
            {
                strcpy(lc.comds[lc.num].argv[idx], token);
                idx++;
            }
            else
            {
                strcpy(lc.comds[lc.num].argv[idx], token);
                idx++;
            }
            clean_buff();
        }//end of for
        lc.comds[lc.num].argv[idx][0] = '\0';
  
    }//end of while

    if(!is_cmd)
        printf("0: \n");
    else{
        
        print_command(lc);
    }
    for (int i=0; i<=lc.num; ++i)
    {
        lc.comds[i].input_redirect=0;
        lc.comds[i].output_redirect=0;
        lc.comds[i].output_append=0;
        lc.comds[i].mode =PLAIN ;
        for(int j=0; j < 20; ++j)
            lc.comds[i].argv[j][0] = '\0';
    }
    lc.ifile[0] = '\0';
    lc.ofile[0] = '\0';
    lc.command_input[0] ='\0';
    lc.num =0;
    lc.is_out_append=0;
    return;
}

