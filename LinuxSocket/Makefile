objects1 = echoSocketClient.o tcp_socket.o tcp_client.o
objects2 = echoSocketServer.o tcp_socket.o tcp_client.o tcp_server.o

bin = g++ -std=c++11

echoSocketClient: $(objects1)
	$(bin) -o echoSocketClient $(objects1)

echoSocketServer: $(objects2)
	$(bin) -o echoSocketServer $(objects2)

echoPacketSelectServer: echoPacketSelectServer.o
	$(bin) -o echoPacketSelectServer echoPacketSelectServer.o

echoPacketSelectServer.o: echoPacketSelectServer.cpp helper.h
	$(bin) -c echoPacketSelectServer.cpp

echoSocketClient.o: echoSocketClient.cpp socket_exception.h tcp_socket.h tcp_client.h
	$(bin) -c echoSocketClient.cpp

echoSocketServer.o: echoSocketServer.cpp socket_exception.h tcp_socket.h tcp_client.h tcp_server.h
	$(bin) -c echoSocketServer.cpp

tcp_socket.o: tcp_socket.cpp tcp_socket.h
	$(bin) -c tcp_socket.cpp

tcp_client.o: tcp_client.cpp tcp_client.h socket_exception.h tcp_socket.h
	$(bin) -c tcp_client.cpp

tcp_server.o: tcp_server.cpp tcp_server.h socket_exception.h tcp_client.h tcp_socket.h
	$(bin) -c tcp_server.cpp

.PHONY: clean

clean:
	-rm echoSocketClient $(objects1) echoSocketServer.o tcp_server.o echoPacketSelectServer.o echoPacketSelectServer
