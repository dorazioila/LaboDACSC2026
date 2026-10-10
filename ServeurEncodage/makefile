ALL: ServeurEncodage

ServeurEncodage: ServeurEncodage.o TCP.o OBEP.o
	g++ -o ServeurEncodage ServeurEncodage.o TCP.o OBEP.o -L/usr/lib64/mysql -L/usr/lib64 -lmysqlclient -lpthread

ServeurEncodage.o: ServeurEncodage.cpp TCP.h OBEP.h
	g++ -c -I. -I/usr/include/mysql ServeurEncodage.cpp -o ServeurEncodage.o

OBEP.o: OBEP.cpp OBEP.h
	g++ -c -I. -I/usr/include/mysql OBEP.cpp -o OBEP.o

TCP.o: TCP.cpp TCP.h
	g++ -c -I. TCP.cpp -o TCP.o

clean:
	rm -f *.o ServeurEncodage