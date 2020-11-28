#include <stdlib.h>
#include <stdio.h>
	
int
main(int argc, char* argv[])
{
	unsigned int* p1 = (unsigned int*) malloc(0);
	unsigned int* p2 = (unsigned int*) malloc(128);

	// save pointers
	unsigned int* q1 = p1, *q2 = p2;
    	
	// place value
	*p1 = 10;
	*p2 = 41;	

	printf("ptr is: %p\n", p1);
	printf("ptr is: %p\n", p2);

	printf("val is: %u\n", *p1);
	printf("ptr is: %u\n", *p2);

        // next int

	p1+= sizeof(unsigned int); 
        p2+= sizeof(unsigned int); 

	*p1 = 77;
	*p2 = 88;
       
	printf("val is: %u\n", *p1);
	printf("ptr is: %u\n", *p2);


	p1+= sizeof(unsigned int); 
        p2+= sizeof(unsigned int); 

	*p1 = 66;
	*p2 = 99;


	printf("val is: %u\n", *p1);
	printf("ptr is: %u\n", *p2);


        free(q1); // this is valid 
	free(q2);

	return 0;
}
