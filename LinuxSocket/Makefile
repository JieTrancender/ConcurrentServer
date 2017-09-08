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

echoPacketSelectClient: echoPacketSelectClient.o
	$(bin) -o echoPacketSelectClient echoPacketSelectClient.o

echoPacketSelectClient.o: echoPacketSelectClient.cpp helper.h
	$(bin) -c echoPacketSelectClient.cpp

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

testFDSETSIZE: testFDSETSIZE.o tcp_server.o tcp_socket.o tcp_client.o
	$(bin) -o testFDSETSIZE testFDSETSIZE.o tcp_server.o tcp_socket.o tcp_client.o

testFDSETSIZE.o: testFDSETSIZE.cpp socket_exception.h tcp_socket.h tcp_server.h helper.h tcp_client.h
	$(bin) -c testFDSETSIZE.cpp

testPoll: testPoll.o tcp_server.o tcp_socket.o tcp_client.o
	$(bin) -o testPoll testPoll.o tcp_server.o tcp_socket.o tcp_client.o

testPoll.o: testPoll.cpp socket_exception.h tcp_socket.h tcp_server.h helper.h tcp_client.h
	$(bin) -c testPoll.cpp

testEpoll: testEpoll.o tcp_server.o tcp_socket.o tcp_client.o
	$(bin) -g -o testEpoll testEpoll.o tcp_server.o tcp_socket.o tcp_client.o

testEpoll.o: testEpoll.cpp socket_exception.h tcp_socket.h tcp_server.h helper.h tcp_client.h
	$(bin) -c testEpoll.cpp

.PHONY: clean

clean:
	-rm echoSocketClient $(objects1) echoSocketServer.o tcp_server.o echoPacketSelectServer.o echoPacketSelectServer \
		echoPacketSelectClient.o echoPacketSelectClient
