
template<typename T>
class base
{
protected:
    int a;
};

class derived : protected base<derived>
{
public:
    void foo(){a=5;}
};
    
    int
main(int aaa, char** bbb)
{
    derived d;
    d.foo();
}

