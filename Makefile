CC:=arm-linux-gnueabihf-gcc
#CC:=gcc

all: socket_client_tf

socket_client_tf.o : socket_client_tf.c
	$(CC) -c socket_client_tf.c 

socket_client_tf : socket_client_tf.o
	$(CC) -Wall -o2 -o socket_client_tf socket_client_tf.o -lpthread


clean:
	-rm -f *.o
	-rm -f socket_client_tf
