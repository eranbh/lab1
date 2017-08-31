

struct A
{
    virtual long get() = 0;
};

struct B : public A
{
    virtual int get() {return 9;}
};

struct C : public A
{
    virtual long get() {return 9;}
};

    
int
main(int a, char* b[])
{
    A* bb = new B;
    A* cc = new C;
    bb->get();
    cc->get();
    return 0;
}
