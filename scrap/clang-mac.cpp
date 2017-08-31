#include<iostream>
#include<thread>
int
main(int a, char* b[])
{
    std::thread th;
    auto lam = [](int c){std::cout << "supporting!!" << std::endl;};
    lam(4);
}
