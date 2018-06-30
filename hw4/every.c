#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>



int main (int argc, char *argv[], char *envp[])
{
	
	if(argc == 1)
	{
		printf ("missing paranmeter, exit\n");
		exit (0);
	}

	int N=1, M=1, start = 2;
	
	if (argv[1][0] == '-' )//there is option
	{
		char* option=argv[1];
		const char s[3] = "-,";
		char *token;
		token = strtok (option, s);
		if (token != NULL)
			N=atoi(token);
		token = strtok(NULL, s);
		if (token != NULL)
			M= atoi(token);
	}
	else//no option , check envp
	{
		char* env_op = getenv("EVERY");
		if(env_op != NULL)//not defined
		{
			const char s[3] = "-,";
			
			char *env_token = strtok (env_op, s);
			if (env_token != NULL)
				N=atoi(env_token);
			env_token = strtok(NULL, s);
			if (env_token != NULL)
				M= atoi(env_token);
			start = 1;
		}
		
	}
	
	//printf("N: %d    M:%d\n", N,M );
    //printf("start: %d   argc:%d\n", start,argc );
    
	
	if(start ==  argc)
	{
		//read from terminal
        char* buff;
        
        int line_count=0, nread=0;
        size_t len=0;
        printf("Please enter your input:\n");
        while((nread=getline(&buff,&len,stdin )) != -1)
        {
            if ((line_count % N) < M)
                printf("%d: %s",line_count, buff);
            line_count++;
        }
	}
	else
	{
		for (int i= start; i < argc; ++i )
		{
			FILE *file = fopen(argv[i], "r");
			if (file == NULL){
				printf("cannot open file %s exit\n",argv[i] );
				exit(0);
			}
			else{
				int line =0;
				ssize_t nread=0;
				size_t len=0;
				char* buff;
				
				while ((nread = getline(&buff,&len, file )) != -1)
				{
					if ((line % N) < M)
						printf("%d %s\n", line, buff);
					line++;
				}

			}
			fclose(file);
		}
	
	}
	return 0;
}




















