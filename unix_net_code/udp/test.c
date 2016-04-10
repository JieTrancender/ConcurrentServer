/*************************************************************************
    > File Name: test.c
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Sat Apr  9 11:50:43 2016
 ************************************************************************/
#include<stdio.h>

int test(int a, int b)
{
	int result, diff;
	diff = a - b;
	result = a / diff;
	return result;
}

int main(int argc, char** argv)
{
	int value, div, result, i, total;
	value = 10;
	div = 6;
	total = 0;
	for (i = 0; i < 10; +i)
	{
		result = test(value, div);
		total += result;
		++div;
		--value;
	}
	printf("%d test by %d equals %d\n", value, div, total);
	return 0;
}
