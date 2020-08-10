#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

int main(int argc, char* argv[])
{
    printf("START\n");
    assert(2 <= strlen(argv[0]));
    printf("argc: %d\n", argc);
    printf("argv[0]: %s\n", argv[0]);
    int arglen = strlen(argv[0]);
    memset(argv[0], 0, arglen);
    strcpy(argv[0], "aa");
    printf("argv[0]: %s\n", argv[0]);
    printf("getpid: %d\n", getpid());
    sleep(60);
    return 0;
}
