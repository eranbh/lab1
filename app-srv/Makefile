all: main nw 
	g++  main.o -L nw/lib -lnw -o bin/app-srv

nw: acceptor.o
	g++ -shared nw/src/acceptor.o  -o nw/lib/libnw.so
	
main:
	g++ -Wall -c -fPIC  -Inw/inc main.cpp -o main.o

acceptor.o:
	g++ -Wall -c -fPIC  -Inw/inc -Icommon/inc nw/src/acceptor.cpp \
	-o nw/src/acceptor.o

nw_test:
	g++ -Wall -Icommon/inc  test/main.cpp -o test/bin/nw_test
