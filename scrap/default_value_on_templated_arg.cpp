#include<iostream>

template<typename T>
void func(T&& t = 4){
    std::cout << t << std::endl;
}

struct stam{stam(int){}};
std::ostream& operator<<(std::ostream& os, stam){return os;}

int
main(int aa, char* bb[])
{
    func<stam>();
}
