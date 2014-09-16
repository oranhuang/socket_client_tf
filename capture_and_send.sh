#!/bin/bash

if [ -z "$1" ];
then
	echo "please type the Server's IP address"

elif [ "$1" -le 0 ];
then
	echo "take picture"
	./raspistill -o image.jpg -t 1000	
	./socket_client_tf -p "$1" -f image.jpg
	echo "finish ....."
fi
