server : server.o DJson.o
	g++ -o server server.o DJson.o -L/usr/local/lib/json/ -ljson_linux-gcc-5.4.0_libmt -L/usr/lib/mysql/ -lmysqlclient

server.o : server.cpp
	g++ -c server.cpp -I/usr/include/mysql/

DJson.o : DJson.cpp
	g++ -c DJson.cpp -I/usr/include/mysql/

clean:
	rm *.o server
