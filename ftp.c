// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 21 

int main(int argc, char const *argv[]) 
{ 
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char buffer[1024] = {0}; 
	char *hello = "220 Hello from server \n"; 
	
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	
        while(1) {
          if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
	    perror("accept"); 
	    exit(EXIT_FAILURE); 
	  } 
  	  close(server_fd);
	  write(new_socket, hello, strlen(hello)); 
          while (valread = read(new_socket, buffer, 1024)){
            printf("%s", buffer);
            //signal(SIGCHLD,my_wait);
            //  if(!(bytes_read>BSIZE)){
            //    /* TODO: output this to log */
            //    buffer[BSIZE-1] = '\0';
            //    printf("User %s sent command: %s\n",(state->username==0)?"unknown":state->username,buffer);
            //    parse_command(buffer,cmd);
            //    state->connection = connection;
          
            /* Ignore non-ascii char. Ignores telnet command */
            //    if(buffer[0]<=127 || buffer[0]>=0){
            //      response(cmd,state);
            //    }
            //    memset(buffer,0,BSIZE);
            //    memset(cmd,0,sizeof(cmd));
            //  }else{
            /* Read error */
            //   perror("server:read");
            // }
           }
  	    
	    valread = read(new_socket, buffer, 1024);
     	    
            hello = "";
            write(new_socket, hello, strlen(hello)); 
   	    valread = read(new_socket, buffer, 1024);
    	    printf("%s", buffer);
          //}
        }
	return 0; 
} 

