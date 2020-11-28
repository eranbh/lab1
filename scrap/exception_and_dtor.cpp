#include<iostream>
class a
{
public:
    ~a(){std::cout << "~a" << std::endl;}
};

class c
{
public:
    ~c(){std::cout << "~c" << std::endl;}
};


class b : public a
{
public:
    b(){throw;}
    ~b(){std::cout << "~b" << std::endl;}
    c m_c;
};

    
int
main(int aa, char* bb[])
{
    b bbb;
}
