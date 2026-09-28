all: ClientBookEncoder

ClientBookEncoder: main.o mainwindowclientbookencoder.o moc_mainwindowclientbookencoder.o TCP.o OBEP.o
	g++ -fPIC -o ClientBookEncoder main.o mainwindowclientbookencoder.o moc_mainwindowclientbookencoder.o TCP.o OBEP.o -lQt5Widgets -lQt5Gui -lQt5Core -lpthread

main.o: main.cpp mainwindowclientbookencoder.h ui_mainwindowclientbookencoder.h OBEP.h
	g++ -c -fPIC -I. -I/usr/include/mysql -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore main.cpp -o main.o

mainwindowclientbookencoder.o: mainwindowclientbookencoder.cpp mainwindowclientbookencoder.h ui_mainwindowclientbookencoder.h TCP.h OBEP.h
	g++ -c -fPIC -I. -I/usr/include/mysql -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore mainwindowclientbookencoder.cpp -o mainwindowclientbookencoder.o

moc_mainwindowclientbookencoder.o: moc_mainwindowclientbookencoder.cpp mainwindowclientbookencoder.h
	g++ -c -fPIC -I. -I/usr/include/mysql -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore moc_mainwindowclientbookencoder.cpp -o moc_mainwindowclientbookencoder.o

OBEP.o: OBEP.cpp OBEP.h
	g++ -c -fPIC -I. -I/usr/include/mysql OBEP.cpp -o OBEP.o

TCP.o: TCP.c TCP.h
	gcc -c -fPIC TCP.c -o TCP.o

clean:
	rm -f *.o ClientBookEncoder