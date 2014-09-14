/*
* Contains all the data struct tests
*/
#include "data_struct_tests.h"

CPPUNIT_TEST_SUITE_REGISTRATION(DataStructsUT); 

#define __CAST_TO_UINT32(VAL) \
  (mem_db::Uint32)VAL 

DataStructsUT::DataStructsUT()
{
  m_stack.init(5);
}

void DataStructsUT::setUp()
{
  m_bMap.bits=0;
  m_stack.clear();
}
void DataStructsUT::tearDown()
{
  //per test destruct
}


void DataStructsUT::test_first_bit_on()
{
  m_bMap.bit_on(1);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_first_bit_on",
                                __CAST_TO_UINT32(0x1),
                                 m_bMap.bits);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("test_first_bit_on",
                                true,
				m_bMap.is_on(0x1));
  
}

void DataStructsUT::test_2_18_bits_on()
{
  m_bMap.bit_on(__CAST_TO_UINT32(0x2));
  m_bMap.bit_on(__CAST_TO_UINT32(0x12));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_2_18_bits_on bit 0x2",
                                true,
				m_bMap.is_on(0x2));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_2_18_bits_on bit 0x12",
                                true,
				m_bMap.is_on(0x12));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_2_18_bits_on eq",
			       m_bMap.bits,
                               __CAST_TO_UINT32(0x20002U));
}

void DataStructsUT::test_max_min_bits_on()
{
  m_bMap.bit_on(__CAST_TO_UINT32(MIN_BIT));
  m_bMap.bit_on(__CAST_TO_UINT32(MAX_BIT));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_max_min_bits_on MAX_BIT",
                                true,
				m_bMap.is_on(MAX_BIT));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_max_min_bits_on MIN_BIT",
                                true,
				m_bMap.is_on(MIN_BIT));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_max_min_bits_on eq",
			       m_bMap.bits,
                               __CAST_TO_UINT32(0x80000001U));
}


void DataStructsUT::
test_3_20_30_on_off_one_by_one()
{
  m_bMap.bit_on(__CAST_TO_UINT32(3));
  m_bMap.bit_on(__CAST_TO_UINT32(20));
  m_bMap.bit_on(__CAST_TO_UINT32(30));


 CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_20_30_on_off_one_by_one 3",
                                true,
				m_bMap.is_on(3));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_20_30_on_off_one_by_one 20",
                                true,
				m_bMap.is_on(20));

   CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_20_30_on_off_one_by_one 30",
                                true,
				m_bMap.is_on(30));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_20_30_on_off_one_by_one eq",
			       m_bMap.bits,
                               __CAST_TO_UINT32(0x20080004U));

  // turn them off
  m_bMap.bit_off(__CAST_TO_UINT32(3));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_20_30_on_off_one_by_one off 3",
                                false,
				m_bMap.is_on(3));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_20_30_on_off_one_by_one off 3 eq",
			       m_bMap.bits,
                               __CAST_TO_UINT32(0x20080000U));  

  m_bMap.bit_off(__CAST_TO_UINT32(20));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_20_30_on_off_one_by_one off 20",
                                false,
				m_bMap.is_on(20));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_20_30_on_off_one_by_one off 20 eq",
			       m_bMap.bits,
                               __CAST_TO_UINT32(0x20000000U)); 

  m_bMap.bit_off(__CAST_TO_UINT32(30));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_20_30_on_off_one_by_one off 30",
                                false,
				m_bMap.is_on(30));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_20_30_on_off_one_by_one off 30 eq",
			       m_bMap.bits,
                               __CAST_TO_UINT32(0x0U));
}

void DataStructsUT::
test_push_1_val()
{
  m_stack.init(5);

  m_stack.push(0x4);
  
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_1_val array val",
			       m_stack.pstack[0],
                               __CAST_TO_UINT32(0x4));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_1_val array capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x1));

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_1_val array sz",
			       m_stack.sz,
				 __CAST_TO_UINT32(0x5));
}

void DataStructsUT::
test_push_3_vals()
{
    m_stack.push(0x4);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_3_vals val",
			       m_stack.pstack[0],
                               __CAST_TO_UINT32(0x4));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_3_vals array capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x1));

   m_stack.push(0x5);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_3_vals val",
			       m_stack.pstack[1],
                               __CAST_TO_UINT32(0x5));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_3_vals array capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x2));
   m_stack.push(0x6);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_3_vals val",
			       m_stack.pstack[2],
                               __CAST_TO_UINT32(0x6));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_3_vals array capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x3));

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_3_vals array sz",
			       m_stack.sz,
				 __CAST_TO_UINT32(0x5));
}

void DataStructsUT::
test_push_full()
{
  m_stack.push(0x0);
  m_stack.push(0x1);
  m_stack.push(0x2);
  m_stack.push(0x3);
  m_stack.push(0x4);

  // FULL STACK

  for(int i=4;i<=0;--i)
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_full val",
			       m_stack.pstack[i],
                               __CAST_TO_UINT32(i));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_full capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x5));

  // try illegal push
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_push_full illegal",
                               m_stack.push(0x9),
                               INV_LOCATION);
}


void DataStructsUT::
test_pop_1_val()
{
  m_stack.push(0x4);
  m_stack.push(0x5);
  m_stack.push(0x6);

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_1_val capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x3));


  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_1_val val",
			       m_stack.pop(),
                               __CAST_TO_UINT32(0x6));

  
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_1_val capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x2));
}


void DataStructsUT::
test_pop_3_vals()
{
  m_stack.push(0x4);
  m_stack.push(0x5);
  m_stack.push(0x6);
  m_stack.push(0x7);
  m_stack.push(0x8);

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_3_val capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x5));

  // pop #1

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_3_val val",
			       m_stack.pop(),
                               __CAST_TO_UINT32(0x8));

  
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_1_val capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x4));

  // pop #2

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_3_val val",
			       m_stack.pop(),
                               __CAST_TO_UINT32(0x7));

  
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_1_val capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x3));

  // pop #3
  
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_3_val val",
			       m_stack.pop(),
                               __CAST_TO_UINT32(0x6));

  
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_1_val capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x2));
}

void DataStructsUT::
test_pop_empty()
{
    m_stack.push(0x4);
    m_stack.push(0x5);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_empty capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x2));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_empty val",
			       m_stack.pop(),
                               __CAST_TO_UINT32(0x5));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_empty capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x1));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_empty val",
			       m_stack.pop(),
                              __CAST_TO_UINT32(0x4));

 CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_empty capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x0));

 // illegal pop

 CPPUNIT_ASSERT_EQUAL_MESSAGE("test_pop_empty val",
			       m_stack.pop(),
                               INV_LOCATION);
}

void  DataStructsUT::
test_3_push_1_pop_2_push()
{
    m_stack.push(0x4);
    m_stack.push(0x5);
    m_stack.push(0x6);

    m_stack.pop();

    m_stack.push(0x88);
    m_stack.push(0x99);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_push_1_pop_2_push capacity",
                               m_stack.getCapacity(),
                               __CAST_TO_UINT32(0x4));

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_push_1_pop_2_push val",
			       m_stack.pop(),
                               __CAST_TO_UINT32(0x99));

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_push_1_pop_2_push val",
			       m_stack.pop(),
                               __CAST_TO_UINT32(0x88));

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_push_1_pop_2_push val",
			       m_stack.pop(),
                               __CAST_TO_UINT32(0x5));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_3_push_1_pop_2_push val",
			         m_stack.pop(),
                                 __CAST_TO_UINT32(0x4));
}
