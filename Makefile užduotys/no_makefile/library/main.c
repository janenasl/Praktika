#include <stdio.h>
#include "file_reader.h"

int main(void)
{
	char *line;
	dump_file("./example_file.txt");
	get_first_line("./example_file.txt", &line);
	printf("First line:%s\n", line);
	return 0;
}
