/*
 * this lets you send arbitrary types to print
 * */
#include<iostream>



// implementation details
namespace
{
    // base case 
    template<typename Type> 
    void print(Type arg_for_print)
    {
        std::cout << arg_for_print;
    } 
    
    // recursive step
    template<typename Type, typename ... Types>
    void print(Type arg_for_print, Types ... types)
    {
        std::cout << arg_for_print;
        std::cout << " ";
        print(std::forward<Types>(types) ... );
    }
}


// this function is a __very__ naiive attempt at 
// replacing the terrible var args family (printf etc)
// the idea is to use the c++ type system in order
// to catch issues at __compile time__ rather than
// get all sorts of issues at runtime.    
template<typename ... Types>
void print_variadic_type_msg(Types ... types)
{
    print(std::forward<Types>(types) ... );
    std::cout << std::endl;
}



////// EXAMPLES ////////

struct eran
{
    int member = 67;
};

// this will need to be a friend in case you
// want eran to be a full fledged class (all 
// members are by default private)
std::ostream& operator<<(std::ostream& stream, eran e)
{
   stream <<  e.member;
   return stream;
}


int
main(int aaa, char* bbb[])
{
    // only primitives
    print_variadic_type_msg(4, 3.8, "stam");


    // some stl types mixed in
    print_variadic_type_msg(4, 3.8, std::string{"stam"});


    // my own type!!! this might require some more code
    // to make your type comply to the stream you're using
    eran e;
    print_variadic_type_msg(777, std::move(e));
}
