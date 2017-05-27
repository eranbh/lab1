#include<stdio.h>

int glob = 1;

struct stam
{
	long a;
	long f;
	long x;
};

void func()
{
        //long a=4;
	stam s{2,5,6};
	printf("glob: %d %ld %ld %ld\n", glob, s.a, s.f, s.x);
	glob++;
	func();
}	
	
int
main(int a, char* b[])
{
        func();
	return 0;
}
