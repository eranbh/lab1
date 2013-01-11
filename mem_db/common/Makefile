all: net shm

net:
	g++  -Wall  -I ./inc ./src/network_interface.cpp -shared -fPIC  -o libnet_intf.so 

shm:
	g++  -Wall  -I ./inc ./src/shm_handler.cpp -shared -fPIC -o libshm_intf.so
