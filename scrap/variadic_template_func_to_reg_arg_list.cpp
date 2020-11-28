void func1(int, int){}


template<typename ... ARGS>
void func(ARGS&& ... args){func1((args) ...);}    
    
int
main(int aaa, char** bbb)
{
   func(2,2);
    return 0;
}
