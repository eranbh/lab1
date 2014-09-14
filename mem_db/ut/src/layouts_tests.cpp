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
                               (unsigned int)sizeof(ArrayLayoutData),
                               mem_allocator::getActSz(layout.m_vpHeader));

  layout.destroy();
}

void
LayoutsUT::test_create_add_entry()
{
	struct MyType
	{
		int i;
		float f;
		double d;
	}myType;

	myType.i=1;
	myType.f=5.5;
	myType.d=4.566;

	UserInp inp[3] = {{0,{4,"col1"}, mem_db::INT} ,
	                    {1,{4,"col2"}, mem_db::FLOAT},
	                    {2,{4,"col3"}, mem_db::DOUBLE} };

	LayoutHeader head;
	prepareLyoutHdr(3, &head, inp);

	// BUILDING THE LAYOUT [room for 3]
	ArrayLayout layout(head, sizeof(MyType)*3);
	layout.addEntry(&myType);
	// make sure the total is cool
	  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_create_add_entry mem_allocator::getSz",
	                               (unsigned int)sizeof(ArrayLayoutData)+(unsigned int)sizeof(MyType)*3,
	                               mem_allocator::getSz(layout.m_vpHeader));

	  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_create_add_entry mem_allocator::getActSz",
	                                 (unsigned int)sizeof(ArrayLayoutData),
	                                 mem_allocator::getActSz(layout.m_vpHeader));

	  // ok - there's only one entry, and the first bit to get alloc'd is 31
	  char* vdata = layout.m_vpData+ 31*layout.m_totTypSz;

	  int shmi = *(reinterpret_cast<int*>(vdata));
	  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_create_add_entry mem_allocator::getActSz",
	                                 myType.i,
	                                 shmi);

	  vdata+=sizeof(int);
	  float shmf = *(reinterpret_cast<float*>(vdata));
	  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_create_add_entry mem_allocator::getActSz",
	                                 myType.f,
	                                  shmf);

	  vdata+=sizeof(float);
	  double shmd = *(reinterpret_cast<double*>(vdata));
	  CPPUNIT_ASSERT_EQUAL_MESSAGE("test_create_add_entry mem_allocator::getActSz",
	 	 	 	                                 myType.d,
	 	 	 	                                 shmd);
	layout.destroy();
}


void LayoutsUT::test_find_entry()
{
	typedef struct tMyType
	{
		int i;
		float f;
		double d;
	}MyType;

	MyType myType;

	myType.i=1;
	myType.f=5.5;
	myType.d=4.566;

	UserInp inp[3] = {{0,{4,"col1"}, mem_db::INT} ,
						{sizeof(int),{4,"col2"}, mem_db::FLOAT},
						{sizeof(int)+sizeof(float),{4,"col3"}, mem_db::DOUBLE} };

	LayoutHeader head;
	prepareLyoutHdr(3, &head, inp);

	// BUILDING THE LAYOUT [room for 3]
	ArrayLayout layout(head, sizeof(MyType)*3);
	layout.addEntry(&myType);

	// build comparable to match desired criteria
	ColName colNms[2];
	strncpy(colNms[0].buff, "col2", 4);
	colNms[0].sz=4;
	strncpy(colNms[1].buff, "col3", 4);
	colNms[1].sz=4;
	RawVal vals[2];
	vals[0]= *(reinterpret_cast<RawVal*>(&myType.f));
	vals[1]= *(reinterpret_cast<RawVal*>(&myType.d));

	Comparable comp(colNms, vals, 2, &head);
	ResultBuff buff;
	// now attempt to find it
	layout.findEntry(comp, &buff);

	// now for the big moment!!!
	MyType* type2 = reinterpret_cast<MyType*>(buff.resData[0].buff);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("test_find_entry mem_allocator::getActSz",
		                          type2->i,
		                           myType.i);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_find_entry mem_allocator::getActSz",
	                                 myType.f,
	                                 type2->f);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("test_find_entry mem_allocator::getActSz",
    	                                 myType.d,
    	                                 type2->d);


	layout.destroy();

}


void LayoutsUT::test_update_entry()
{
	typedef struct tMyType
	{
		int i;
		float f;
		double d;
	}MyType;

	MyType myType;

	myType.i=1;
	myType.f=5.5;
	myType.d=4.566;

	UserInp inp[3] = {{0,{4,"col1"}, mem_db::INT} ,
						{sizeof(int),{4,"col2"}, mem_db::FLOAT},
						{sizeof(int)+sizeof(float),{4,"col3"}, mem_db::DOUBLE} };

	LayoutHeader head;
	prepareLyoutHdr(3, &head, inp);

	// BUILDING THE LAYOUT [room for 3]
	ArrayLayout layout(head, sizeof(MyType)*3);
	layout.addEntry(&myType);

	// build comparable to match desired criteria
	ColName colNms[2];
	strncpy(colNms[0].buff, "col2", 4);
	colNms[0].sz=4;
	strncpy(colNms[1].buff, "col3", 4);
	colNms[1].sz=4;
	RawVal vals[2];
	vals[0]= *(reinterpret_cast<RawVal*>(&myType.f));
	vals[1]= *(reinterpret_cast<RawVal*>(&myType.d));

	// compare criteria
	Comparable comp(colNms, vals, 2, &head);

	// data with which to update - we know its gonna be the last 2 fields ...
	mem_db::BufferSz buffsz[2];
	float newval=4545;
	memcpy(buffsz[0].buff, &newval, sizeof(float));
	buffsz[0].sz=sizeof(float);

	double newvald=5454;
	memcpy(buffsz[1].buff, &newvald, sizeof(double));
	buffsz[1].sz=sizeof(double);

	// work your magic --> 2 recs should get updated!!!
	CPPUNIT_ASSERT_EQUAL_MESSAGE("test_find_entry mem_allocator::getActSz",
			                     layout.updateEntry(comp, buffsz),
			                     (unsigned int)1);

	// ok - there's only one entry, and the first bit to get alloc'd is 31
    char* vdata = layout.m_vpData+ 31*layout.m_totTypSz;

	MyType* ptyp = reinterpret_cast<MyType*>(vdata);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("test_create_add_entry mem_allocator::getActSz",
		                          ptyp->f,
		                          (float)4545);

	CPPUNIT_ASSERT_EQUAL_MESSAGE("test_create_add_entry mem_allocator::getActSz",
			                          ptyp->d,
			                        (double)5454);

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
