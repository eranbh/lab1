#ifndef __MEM_DB_TEST_INC_DATA_STRUCT_H_
#define __MEM_DB_TEST_INC_DATA_STRUCT_H_
/*
* This class encapsulates all the TCs
* for the data structures the DB uses
*/
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"
#include "data_structs.h"

class DataStructsUT : public CppUnit::TestFixture
{
 
  private:

    CPPUNIT_TEST_SUITE(DataStructsUT);
    /*BIT_MAP TCs*/
    CPPUNIT_TEST(test_first_bit_on);
    CPPUNIT_TEST(test_2_18_bits_on);
    CPPUNIT_TEST(test_max_min_bits_on);
    CPPUNIT_TEST(test_3_20_30_on_off_one_by_one);

    /*STACK TCs*/
    CPPUNIT_TEST(test_push_1_val);
    CPPUNIT_TEST(test_push_3_vals);
    CPPUNIT_TEST(test_push_full);
    CPPUNIT_TEST(test_pop_1_val);
    CPPUNIT_TEST(test_pop_3_vals);
    CPPUNIT_TEST(test_pop_empty);
    CPPUNIT_TEST(test_3_push_1_pop_2_push);
    

    CPPUNIT_TEST_SUITE_END();

    BitMap m_bMap;
    Stack m_stack;

    
  public:
    DataStructsUT();
    ~DataStructsUT(){}
    virtual void setUp();
    virtual void tearDown();

    void test_first_bit_on();
    void test_2_18_bits_on();
    void test_max_min_bits_on();
    void test_3_20_30_on_off_one_by_one();
    void test_push_1_val();
    void test_push_3_vals();
    void test_push_full();
    void test_pop_1_val();
    void test_pop_3_vals();
    void test_pop_empty();
    void test_3_push_1_pop_2_push();

};




#endif // __MEM_DB_TEST_INC_DATA_STRUCT_H_

