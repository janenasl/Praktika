#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dump_file(char *fileName)
{
	FILE *fp;
	char ch;
	
	fp = fopen(fileName, "r");
	if ( fp == NULL) {
		return 1;
	}
	
	printf("The contents of %s file are:\n", fileName);
	
	while((ch = fgetc(fp)) != EOF) {
      		printf("%c", ch);
      	}

   	fclose(fp);
	return 0;
}

int get_first_line(char *fileName, char **holder)
{
	FILE *fp;
	char fileText[256];
	int lenght;
	
	fp = fopen(fileName, "r");
	if ( fp == NULL) {
		return 1;
	}
	
	fgets (fileText, 256, fp);
	lenght = strlen(fileText);
	
	*holder = malloc(sizeof(char) * lenght);
	if (*holder == NULL) {
		printf("Error\n");
		return 2;
	}
	fclose(fp);
	
	strncpy(*holder, fileText, lenght);
	printf("%s\n", *holder);
	
	return 0;
}
