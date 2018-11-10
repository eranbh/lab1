
void func();

int
main(int aa, char* bb[])
{
    func();
    return 0;
}

void func()
{
    char* b = nullptr;
    main(0, &b);
}
 
