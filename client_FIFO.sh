#!/bin/bash
for i in {1..200}
do 
	./wclient localhost 10000  /hello.html
done
