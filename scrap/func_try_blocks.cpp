#include <exception>

// /THEY ALWAYS RE-THROW!!

struct A
{
        A(){throw std::exception();}
};

struct B
{
        B(A aa) try : a(aa){} catch(std::exception& ee){return;}

        A a;
};

int
main(int aa, char* bb[])
{
        A a;
        B b(a);
}
