#ifndef __MEM_DB_TEST_INC_LAYOUTS_H_
#define __MEM_DB_TEST_INC_LAYOUTS_H_
/*
* This class encapsulates all the TCs
* for the data structures the DB uses
*/
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"



typedef struct tUserInp
{
  unsigned int loc;
  ColName name;
  mem_db::ColTyp typ;
}UserInp;

class LayoutsUT : public CppUnit::TestFixture
{
 
  private:

    CPPUNIT_TEST_SUITE(LayoutsUT);
    
    CPPUNIT_TEST(test_create_layout);
    CPPUNIT_TEST(test_create_add_entry);
    CPPUNIT_TEST(test_find_entry);
    CPPUNIT_TEST(test_update_entry);

    CPPUNIT_TEST_SUITE_END();

    /*
    * This prepares the header layout
    * For now it gets a set of type enums
    * and builds the header from them
    * This should change, once a more robust
    * type mechanism is in place  
    */
    int prepareLyoutHdr(unsigned int i_num, 
                        LayoutHeader* o_head,
                        const UserInp* i_pUserInp);

 public:

     LayoutsUT();
    ~LayoutsUT(){}
    virtual void setUp();
    virtual void tearDown();

    void test_create_layout();
    void test_create_add_entry();
    void test_find_entry();
    void test_update_entry();
};

#endif // __MEM_DB_TEST_INC_LAYOUTS_H_
