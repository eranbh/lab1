all:
	g++ -std=gnu++0x -g -Wall -D__TESTING_MODE -Iinc -I../shr -I../nw/inc  -I/usr/include/ -L/usr/lib -L ../nw/lib \
        ../../ut-fw/src/main_ut.cpp  src/nwUT.cpp -lnw -lcppunit  -o \
           bin/utNW 

pl: stub
	g++ -g -Wall -Iinc -I/usr/include/ ../../ut-fw/src/main_ut.cpp src/plUT.cpp -lcppunit -o bin/utPL

stub:
	g++ -g -Wall -fPIC -I/usr/include/ src/liboit_stubs.cpp -shared -o lib/liboit_stubs.so
