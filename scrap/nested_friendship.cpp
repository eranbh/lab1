struct outer
{
    friend struct tester;
    private:

        struct inner
        {
            int a;
        };
};

struct tester
{
    void func()
    {
        outer::inner in;
        in.a = 4;
    }
};



int
main(int aa, char* bb[])
{
    tester ts;
    ts.func();
    return 0;
}
