#include <stdio.h>
#include <usage.h>
#include <maths.h>

void actions(char operator)
{
	int a, b;
	
	switch(operator) {
			case '+':
				printf("Enter two operands separated by space: ");
				scanf("%d %d", &a, &b);
				printf("Add: %d\n", add(a, b));
				break;
			case '-':
				printf("Enter two operands separated by space: ");
				scanf("%d %d", &a, &b);
				printf("Subtract: %d\n", subtract(a, b));
				break;
			case '/':
				printf("Enter two operands separated by space: ");
				scanf("%d %d", &a, &b);
				printf("Divide: %d\n", divide(a, b));
				break;
			case '*':
				printf("Enter two operands separated by space: ");
				scanf("%d %d", &a, &b);
				printf("Multiply: %d\n", multiply(a, b));
				break;
			case 'h':
				print_usage();
				break;
			case '\n':
				break;
			default:
				printf("Command not found\n");
		}
}

  
int main(void)
{
	char operator;

	print_usage();
	printf("Enter command: ");
	while((operator = getchar()) != '0') {
		if (operator == 'q')
			break;
		if (operator != '\n') {
			actions(operator);
			printf("Enter command: ");
		}
	}

	return 0;
}
