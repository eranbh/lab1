all: COM_RCV COM_SND SHM_CRT LAY_OUT 

COM_RCV:
	g++ -Wall -I /home/eranbh/work/SHARKS/mem_db/common/inc ./src/com_rcv_tests.cpp -L /home/eranbh/work/SHARKS/mem_db/common -lnet_intf -o COM_RCV_TEST


COM_SND:
	g++ -Wall -I /home/eranbh/work/SHARKS/mem_db/common/inc ./src/com_send_tests.cpp -L /home/eranbh/work/SHARKS/mem_db/common -lnet_intf -o COM_SEND_TEST


SHM_CRT:
	g++ -Wall -I /home/eranbh/work/SHARKS/mem_db/common/inc ./src/shm_crt.cpp  -L /home/eranbh/work/SHARKS/mem_db/common -lshm_intf -o SHM_CRT_TEST

LAY_OUT:
	g++ -Wall -g -D__HAS_SHM_RPRT_  -Iinc -I/home/eranbh/work/SHARKS/mem_db/server/layouts/inc -I /home/eranbh/work/SHARKS/mem_db/common/inc ./src/layouts_tests.cpp  ./src/main_ut.cpp -L/home/eranbh/work/SHARKS/mem_db/common  -L /usr/lib -lcppunit -lshm_intf   -o LAY_TEST


DAT_STRCT:
	g++ -Wall -I/usr/include/cppunit -Iinc -I/home/eranbh/work/SHARKS/mem_db/common/inc  ./src/main_ut.cpp  ./src/data_struct_tests.cpp  -L/usr/lib -lcppunit -o DAT_STRCT 
