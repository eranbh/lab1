struct stam
{
	template<typename T>
	void func(T arg){}
};

int main(int argc, char* argv[])
{
    stam st;
    int i=0;
    st.func<int>(i);
}
