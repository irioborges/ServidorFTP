#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 21 

typedef struct State {
  /* Connection mode: NORMAL, SERVER, CLIENT */
  int mode;

  /* Is user loggd in? */
  int logged_in;

  /* Is this username allowed? */
  int username_ok;
  char *username;
  
  /* Response message to client e.g. 220 Welcome */
  char *message;

  /* Commander connection */
  int connection;

  /* Socket for passive connection (must be accepted later) */
  int sock_pasv;

  /* Transfer process id */
  int tr_pid;

} State;


typedef struct Command {
  char command[5];
  char arg[1024];
} Command;


void parse_command(char *cmdstring, Command *cmd)
{
  sscanf(cmdstring,"%s %s",cmd->command,cmd->arg);
}

void response(Command *cmd, State *state) {
/*  switch(lookup_cmd(cmd->command)){
    case USER: ftp_user(cmd,state); break;
    case PASS: ftp_pass(cmd,state); break;
    case PASV: ftp_pasv(cmd,state); break;
    case LIST: ftp_list(cmd,state); break;
    case CWD:  ftp_cwd(cmd,state); break;
    case PWD:  ftp_pwd(cmd,state); break;
    case MKD:  ftp_mkd(cmd,state); break;
    case RMD:  ftp_rmd(cmd,state); break;
    case RETR: ftp_retr(cmd,state); break;
    case STOR: ftp_stor(cmd,state); break;
    case DELE: ftp_dele(cmd,state); break;
    case SIZE: ftp_size(cmd,state); break;
    case ABOR: ftp_abor(state); break;
    case QUIT: ftp_quit(state); break;
    case TYPE: ftp_type(cmd,state); break;
    case NOOP:
      if(state->logged_in){
        state->message = "200 Nice to NOOP you!\n";
      }else{
        state->message = "530 NOOB hehe.\n";
      }
      write_state(state);
      break;
    default: 
      state->message = "500 Unknown command\n";
      write_state(state);
      break;
  } */


if(strcmp(cmd->command, "USER") == 0){
  printf("\n%s\n", cmd->command);
  state->message = "331 Usuario Ok\n";
}

if(strcmp(cmd->command, "PASS") == 0){
  printf("\n%s\n", cmd->command);
  state->message = "230 Login realizado com sucesso!\n";
}

if(strcmp(cmd->command, "SYST") == 0){
  printf("\n%s\n", cmd->command);
  state->message = "Linux\n";
}

if(strcmp(cmd->command, "QUIT") == 0){
  printf("\n%s\n", cmd->command);
  state->message = "221 Tchau!!\n";
  close(state->connection);
  //exit(0);
}


write(state->connection, state->message, strlen(state->message));

}

int main(int argc, char const *argv[]) { 
  int server_fd, new_socket, valread; 
  struct sockaddr_in address; 
  int addrlen = sizeof(address); 
  char buffer[1024] = {0}; 
  Command *cmd = malloc(sizeof(Command));
  State *state = malloc(sizeof(State));
  char *hello = "220 Seja bem vindo\n"; 
	
  //Cria o socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
    perror("socket failed"); 
    exit(EXIT_FAILURE); 
  } 
	
  address.sin_family = AF_INET; 
  address.sin_addr.s_addr = INADDR_ANY; 
  address.sin_port = htons( PORT ); 
	
  //Associa o socket a porta
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { 
    perror("bind failed"); 
    exit(EXIT_FAILURE); 
  } 

  //Escuta o socket na porta
  if (listen(server_fd, 3) < 0) { 
    perror("listen"); 
    exit(EXIT_FAILURE); 
  } 
                                                                                                                            
  printf("Servidor escutando na porta %i\n", PORT);
	
  while(1) {
    //Espera por requisição de conexão entrante		
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) { 
      perror("accept"); 
      exit(EXIT_FAILURE); 
    }
 
    printf("Realizada requisição de conexão entrante\n");
    close(server_fd);

    //Faz envio de mensagem de boas vindas ao cliente que fez a requisição de conexão entrante	
    write(new_socket, hello, strlen(hello)); 

    //Aguarda por comando do cliente
    while (valread = read(new_socket, buffer, 1024)){
      printf("%s", buffer);
      //signal(SIGCHLD,my_wait);
      if(!(valread > 1024)){
        buffer[1023] = '\0';
        printf("Usuario %s enviou o comando: %s\n",(state->username==0)?"unknown":state->username,buffer);
        parse_command(buffer,cmd);
        state->connection = new_socket;
          
        /* Ignore non-ascii char. Ignores telnet command */
        if(buffer[0]<=127 || buffer[0]>=0){
          response(cmd,state);
        }
        //    memset(buffer,0,BSIZE);
        //    memset(cmd,0,sizeof(cmd));
        //  }else{
        /* Read error */
        //   perror("server:read");
              // }
      }
    }//Fecha colchete do segundo loop
    printf("Cliente disconectado.\n");
    exit(0);	

    close(new_socket);     
          
  }//Fecha colchete do loop principal

}//Fecha colchete do main 

