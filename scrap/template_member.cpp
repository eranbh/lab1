struct stam
{
	template<typename T>
	T member;
};



int main(int argc, char* argv[])
{
        stam s;
	s.member<int>;
	return 0;
}
