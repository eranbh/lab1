#include<cstddef>
#include<iostream>
#include<array>

struct group
{
    int mm;
    int id;

    struct process
    {
        int pp;
        const int getGrpId(int idx)const
        {

            const char* rawThis = reinterpret_cast<const char*>(this)-(sizeof(process)*idx);
            rawThis -= (offsetof(group, procVec) - offsetof(group, id));
            return  *reinterpret_cast<const int*>(rawThis);
        }
    };
    // process procVec[3];
    std::array<process,3> procVec;
}; 

int
main(int a, char* b[])
{
    group  st;
    st.id=11;
    st.mm=22;
    int idx=0;
    for(auto& proc : st.procVec)
    {
        std::cout << proc.getGrpId(idx++) << std::endl;
    }

}
