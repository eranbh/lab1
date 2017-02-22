
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


int
main(int a, char* b[])
{
    int fd = open("file", O_CREAT);
    printf("%d", fd);   
    return 0;
}
