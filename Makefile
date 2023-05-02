all: http_client http_server

http_client: http_client.cpp
	g++ -o http_client http_client.cpp

http_server: http_server.cpp
	g++ -o http_server http_server.cpp

clean:
	rm http_client http_server 