# ipv6cidr
A simple cross-platform ipv6 cidr table implementation in c.

The source code is tested and should be no memory leaks.

## usage:
You can use it as static library or just copy the .h and .c files to your project.

## compile and test:
* `make` : compile static library called "libipv6cidr.a"
* `make test` : run test 

## Note:
1. This library use FreeBSD's queue implementation which is commonly used in C projects. 
The queue.h is slightly modified to make it cross-platform. 

2. The test can only be run on Linux for now. 
