
#include <stdio.h>

#define LONGSIZE sizeof(long)			// 4
#define LONGLONGSIZE sizeof(long long)	// 8


int main(void)
{

	printf("sizeof(long) %u sizeof(long long) %u\n",LONGSIZE,LONGLONGSIZE);
}
