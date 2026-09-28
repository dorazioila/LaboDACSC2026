all: ClientBookEncoder

ClientBookEncoder: main.o mainwindowclientbookencoder.o moc_mainwindowclientbookencoder.o TCP.o
	g++ -fPIC -o ClientBookEncoder main.o mainwindowclientbookencoder.o moc_mainwindowclientbookencoder.o TCP.o -lQt5Widgets -lQt5Gui -lQt5Core -lpthread

main.o: main.cpp mainwindowclientbookencoder.h ui_mainwindowclientbookencoder.h OBEP.h
	g++ -c -fPIC -I. -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore main.cpp -o main.o

mainwindowclientbookencoder.o: mainwindowclientbookencoder.cpp mainwindowclientbookencoder.h ui_mainwindowclientbookencoder.h TCP.h OBEP.h
	g++ -c -fPIC -I. -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore mainwindowclientbookencoder.cpp -o mainwindowclientbookencoder.o

moc_mainwindowclientbookencoder.o: moc_mainwindowclientbookencoder.cpp mainwindowclientbookencoder.h
	g++ -c -fPIC -I. -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets -I/usr/include/qt5/QtGui -I/usr/include/qt5/QtCore moc_mainwindowclientbookencoder.cpp -o moc_mainwindowclientbookencoder.o

TCP.o: TCP.cpp TCP.h
	g++ -c -fPIC TCP.cpp -o TCP.o

clean:
	rm -f *.o ClientBookEncoder