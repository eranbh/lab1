#include <stdlib.h>

void reverse(char* str, int n)
{
	char* end = str+n;
	char* p = str;

	while(end != p) 
	{
		int len_char = mblen(p, end-p);
		reverse(p, len_char);
		p+=len_char;
	}
	reverse(str, n);
}

int
main(int argc, char* argv[])
{
	char buff[4] = {'e', 'r', 'a', 'n'};
	reverse(buff, 4);
	return 0;
}
