#include<vector>

struct stam
{
	template<typename T>
	static void func(T arg){}

	template<typename U>
	using member = std::vector<U>; 
};

template<typename T>
void func1(T& t)
{
        // accessing a function template through template param
	T::template func(t);
	// accessing a variable template through template param
	typename T::template member<T> h;
}

int main(int argc, char* argv[])
{
    stam st;
    func1(st);
}
