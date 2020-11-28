#include<iostream>

struct stam
{
    void func(){std::cout << "non const" << std::endl;}
    void func()const{std::cout << "const" << std::endl;}
};

    
int
main(int aa, char* bb[])
{
    stam st;
    st.func();


    const stam* pst = &st;
    pst->func();
}
