pl: stub
	g++ -g -Wall -fpic -O0 -Iinc  -I/usr/include/ ../../ut-fw/src/main_ut.cpp src/plUT.cpp -m64 -lcppunit -o bin/utPL

stub:
	g++ -g -Wall -z defs -z interpose -O0  -fpic -I/usr/include/ src/liboit_stubs.cpp -shared -m64 -o lib/liboit_stubs.so
