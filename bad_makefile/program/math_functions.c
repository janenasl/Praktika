#include <stdio.h>
#include <stdlib.h>

extern float add(float a, float b)
{
	float sum = a + b;
	return sum;
}

extern float subtract(float a, float b)
{
	float sub = a - b;
	return sub;
}

extern float divide(float a,float b)
{
	float div = a / b;
	return div;
}

extern float multi(float a, float b)
{
	float mul = a * b;
	return mul;
}
