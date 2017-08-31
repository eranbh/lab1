

class outer
{
    private:
     using ARR=int;
        class inner
        {
            public:
                outer::ARR i;
            void inner_func(){func();}
        };

        friend class outer::inner;

        void func(){}

    public:
        inner in;
};


int
main(int aa, char* bb[])
{
    outer out;
    out.in.inner_func(); 
}
