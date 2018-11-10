#include<iostream>
#include<stdio.h>
    
int
main ( int aa, char* bb[] )
{
    FILE* fp = ::popen("top -b -n 1 -c", "r");
    if (fp == NULL) {
        std::cout << strerror(errno) << std::endl;
    }
    return 0;
}
