#!/bin/bash

for i in {1...1000}
do
	./wclient localhost 10000 /
	sleep 20
done
