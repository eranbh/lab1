#include<iostream>
void func();

int
main(int aa, char* bb[])
{
    func();
    std::cout << "in main" << std::endl;
    return 0;
}

void func()
{
    main(0, nullptr);
}
 
