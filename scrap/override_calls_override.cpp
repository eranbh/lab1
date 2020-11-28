#include<iostream>
struct base
{
    void foo(){std::cout << "base" << std::endl;}
};

struct derived : public base
{
    void foo(){std::cout << "derived" << std::endl;}
};
    
    int
main(int aa, char** bb)
{
    derived d;
    d.foo();
}
