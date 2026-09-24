all: ServeurTest ClientTest

ServeurTest: ServeurTest.o TCP.o
	gcc ServeurTest.o TCP.o -o ServeurTest

ClientTest: ClientTest.o TCP.o
	gcc ClientTest.o TCP.o -o ClientTest

TCP.o: TCP.c TCP.h
	gcc -c TCP.c -o TCP.o

ServeurTest.o: ServeurTest.c TCP.h
	gcc -c ServeurTest.c -o ServeurTest.o

ClientTest.o: ClientTest.c TCP.h
	gcc -c ClientTest.c -o ClientTest.o

clean:
	rm -f *.o ServeurTest ClientTest