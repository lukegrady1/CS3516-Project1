#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

//error message for strings
void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){

     int sock, newsockfd, portNumber;
     socklen_t clilen;
     char buffer[256]; //buffer array
     struct sockaddr_in server_address, client_address;
     int n;

     //check args
     if (argc < 2) {
         fprintf(stderr,"ERROR, port not provided\n");
         exit(1);
     }
    //makes socket
     sock = socket(AF_INET, SOCK_STREAM, 0); 
     if (sock < 0){
        error("ERROR while opening socket");
      }

    //sets up server info
     bzero((char *) &server_address, sizeof(server_address));
     portNumber = atoi(argv[1]); 
     server_address.sin_family = AF_INET;
     server_address.sin_addr.s_addr = INADDR_ANY;
     server_address.sin_port = htons(portNumber);
     
     if (bind(sock, (struct sockaddr *) &server_address,sizeof(server_address)) < 0){
              error("ERROR when binding");
            }
     listen(sock,5);
		 int clientsocket;

		 while(1){
			clientsocket = accept(sock,(struct sockaddr *) &client_address,&clilen); 

      FILE* html_data = fopen("index.html","r");
      char response_data[64000]; 
      //send http reponse to client
      char http_header[21] = "HTTP/1.1 200 OK\r\n\n"; 
      while (fgets(response_data,sizeof(response_data), html_data) != NULL){
          //sends the file
          send(clientsocket,http_header,sizeof(http_header),0); 
          //prints out the response data
          printf("\"%s\"\n", response_data); 
        }
        //closes socket
			  close(clientsocket); 
		}
    return 0;
}