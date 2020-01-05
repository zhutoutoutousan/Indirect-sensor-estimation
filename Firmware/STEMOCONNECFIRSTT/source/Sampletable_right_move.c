#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NR(x) (sizeof(x)/sizeof(x[0]))

int Sampletable_right_move(int *sample, int sam_len)
{
	int i;
	char tmp = sample[sam_len - 1];
	for(i=sam_len;i>0;i--)
	{
		sample[i] = sample[i-1];
	}
	sample[0] = tmp;
}
