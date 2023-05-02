#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define OPTION "-p"

int main(int argc , char *argv[]) {
    int clientSock; 
    int get; 
    int option; 
    struct addrinfo thisAddress; 
    struct addrinfo  *serverInfo; 
    struct sockaddr_in *h; 
    const char* req1 = "GET /";
    const char* req2 = " HTTP/1.1\r\nHost: ";
    const char* req3 = "\r\nConnection: close\r\n\r\n";
    char* input;
    char* path; 
    char totalMessage[6000]; 
    struct timeval start, end; 
    unsigned long timeInMicroseconds;
    double timeInMilliseconds;
		char * line = NULL; 
		size_t length = 0; 
    in_addr_t data; 

    //check num of arg
    if (argc < 3) {
        //if less than 3 print an error
        fprintf(stderr,"ERROR, usage is the following: ./http_client [-options] server_url port_number\n");
        exit(1);
    }

    memset(&thisAddress, 0, sizeof (thisAddress));
    thisAddress.ai_family = AF_INET; 
    thisAddress.ai_socktype = SOCK_STREAM; 

    bzero((char *)&h, sizeof(h));

    // If user does not want RTT
    if (argc == 3) {
        option = 0; 

        input = strtok(argv[1], "/"); 

        get = getaddrinfo(input, argv[2], &thisAddress, &serverInfo); 

        if (get != 0) {
            fprintf(stderr, "Get Address Info: %s\n", gai_strerror(get));
            return 1;
        }
    }
    //If user wants RTT
    else if (argc == 4) {
        char *opt = (char*)malloc(sizeof(argv[1]));
        strcpy(opt, argv[1]);

        //check if -p is given
        if (strcmp(opt, OPTION)) {
            fprintf(stderr, "ERROR, option is not -p\n");
            exit(1);
        }
        option = 1; 

        input = strtok(argv[2], "/"); //parse for URL/IP
        get = getaddrinfo(input, argv[3], &thisAddress, &serverInfo); //addrinfo

        //valid?
        if (get != 0)  {
          //if fail return return
            fprintf(stderr, "Get Address Info: %s\n", gai_strerror(get));
            exit(1);
        }
    }

    // socket setup
    if ((clientSock = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol)) < 0) {
      //throw error less than 0
        close(clientSock);
        fprintf(stderr, "Error: socket\n");
        exit(1);
    }

    gettimeofday(&start, NULL);

    // connect to  server
    if (connect(clientSock , serverInfo->ai_addr, serverInfo->ai_addrlen) < 0) {
      //throw error less than 0
        close(clientSock);
        fprintf(stderr,"Error: connection\n");
        exit(1);
    }
    
    gettimeofday(&end, NULL);
    printf("Connected\n");

    freeaddrinfo(serverInfo); 

    //send REQUEST
    path = strtok(NULL, "/");
    if (path == NULL) {
      path = strdup("index.html");
			sprintf(totalMessage, "%s%s%s%s", req1, req2, input, req3);
    }else{
      //checks only if the path exists
			sprintf(totalMessage, "%s%s%s%s%s", req1, path, req2, input, req3);
		}


    printf("Sending Data\n\n");

    //receive data
		printf("Requested File -> %s\n",path);
		printf("IP/URL %s\n",input);

		FILE* stream = fdopen(clientSock,"r+w");
		FILE* output = fopen(path,"a"); 

  	fprintf(stream,"%s",totalMessage); 

		int send = 0; 

		while((getline(&line, &length, stream)) != -1 && line != NULL){
			if(strstr(line, "<") != NULL){
				send = 1; 
			}
			if(send == 0){
				printf("%s",line); 
			}
			else{
					fprintf(output, "%s\n",line ); 
			}

			if(strstr(line, "</html>") != NULL) {
				break;
		    }
			}
			printf("File %s is saved\n",path );


			fclose(output); 
			fclose(stream);
			free(line); 
			close (clientSock); 
    //if -p then print RTT
    if (option == 1) {
      timeInMicroseconds = ((end.tv_sec - start.tv_sec)*1000) + (end.tv_usec - start.tv_usec);
      timeInMilliseconds = (double) timeInMicroseconds/1000;
        printf("RTT in milliseconds: %f\n", timeInMilliseconds);
    }

    return 0; 
}