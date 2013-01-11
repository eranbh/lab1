/*
* Contains all the layout tests
* Remember to compile this with -D__HAS_SHM_RPRT_
*/
#include <stdarg.h>
#include "layouts.h"
#include "layouts_tests.h"

CPPUNIT_TEST_SUITE_REGISTRATION(LayoutsUT); 

LayoutsUT::LayoutsUT()
{
}

void LayoutsUT::setUp()
{
}
void LayoutsUT::tearDown()
{
  // clear any shm that might be out there
}

void  LayoutsUT::test_create_layout()
{
  UserInp inp[3] = {{0,{4,"col1"}, mem_db::INT} , 
                    {1,{4,"col2"}, mem_db::FLOAT},
                    {2,{4,"col3"}, mem_db::DOUBLE} };
  
  LayoutHeader head;
  prepareLyoutHdr(3, &head, inp);

  // BUILDING THE LAYOUT
  ArrayLayout layout(head, 10);
  
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_create_layout layout.m_totTypSz",
			       g_type_ref[mem_db::INT]+
                               g_type_ref[mem_db::FLOAT]+
                               g_type_ref[mem_db::DOUBLE],
                               layout.m_totTypSz);

  // make sure the total is cool
  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_create_layout mem_allocator::getSz",
                               (unsigned int)sizeof(ArrayLayoutData)+10,
                               mem_allocator::getSz(layout.m_vpHeader));

  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_create_layout mem_allocator::getActSz",
                               layout.m_totTypSz,
                               mem_allocator::getActSz(layout.m_vpHeader));
}


int  LayoutsUT::
prepareLyoutHdr(unsigned int i_num, 
                LayoutHeader* o_head,
                const UserInp* i_pUserInp)
{
  unsigned int i=0;
  for(;i<i_num;++i)
  {
    ColumnDef colDef(i_pUserInp[i].name, i_pUserInp[i].loc, i_pUserInp[i].typ);
    o_head->addColumnDef(colDef);
 
  }
  o_head->sz=i;

  return 0;
}
