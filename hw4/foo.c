#include <stdio.h>
#include <stdlib.h>
int main()
{
	

	FILE* fptr= fopen("/home/lizhenl/cs146/hw4/a4.txt", "r");
	if (fptr ==NULL)
	{
		printf("%s\n","file not exist" );
		exit(0);
	}
	char c= fgetc(fptr);
	while (c != EOF){
		printf("%c", c);
		c=fgetc(fptr);
	}
	return 0;

}
