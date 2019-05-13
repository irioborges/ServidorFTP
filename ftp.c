#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
//#include <string.h>
#define PORT 21 

int porta_port;

typedef enum conn_mode{ NORMAL, SERVER, CLIENT }conn_mode;

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

int calcularPorta(Command *cmd) {
  unsigned long a0, a1, a2, a3, p0, p1, addr;  

  sscanf(cmd->arg, "%lu,%lu,%lu,%lu,%lu,%lu", &a0, &a1, &a2, &a3, &p0, &p1);

  return (p0 * 256) + p1;

}

int abreConexaoCliente(){
struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char *hello = "Hello from client"; 
    char buffer[1024] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    //serv_addr.sin_port = htons(calcularPorta(cmd)); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    send(sock , hello , strlen(hello) , 0 ); 
    printf("Hello message sent\n"); 
    valread = read( sock , buffer, 1024); 
    printf("%s\n",buffer ); 

    //state->message = "200 Command okay.\n";
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
    state->message = "331 User name okay, need password.\n";
  }

  if(strcmp(cmd->command, "PASS") == 0){
    state->message = "230 User logged in, proceed.\n";
  }

  if(strcmp(cmd->command, "SYST") == 0){
    state->message = "215 Linux\n"; 
  }

  if(strcmp(cmd->command, "QUIT") == 0){
    state->message = "221 Tchau!!\n";
  }

  if(strcmp(cmd->command, "PORT") == 0){
    //printf("%i\n", calcularPorta(cmd));
    state->message = "200 Command okay.\n";
  }

  if(strcmp(cmd->command, "LIST") == 0){
      printf("\nTeste\n");
/*    if(state->logged_in==1){
      struct dirent *entry;
      struct stat statbuf;
      struct tm *time;
      char timebuff[80], current_dir[1024];
      int connection;
      time_t rawtime;
*/
      /* TODO: dynamic buffering maybe? */
/*      char cwd[1024], cwd_orig[1024];
      memset(cwd, 0, 1024);
      memset(cwd_orig, 0, 1024);
*/
    
      /* Later we want to go to the original path */
//      getcwd(cwd_orig, 1024);
    
      /* Just chdir to specified path */
/*      if(strlen(cmd->arg)>0&&cmd->arg[0]!='-'){
        chdir(cmd->arg);
      }
    
      getcwd(cwd, 1024);
      DIR *dp = opendir(cwd);
    } */
  }

  write(state->connection, state->message, strlen(state->message));

}
    /*
    if(!dp){
      state->message = "550 Failed to open directory.\n";
    }else{
      if(state->mode == SERVER){

        connection = accept_connection(state->sock_pasv);
        state->message = "150 Here comes the directory listing.\n";
        puts(state->message);

        while(entry=readdir(dp)){
          if(stat(entry->d_name,&statbuf)==-1){
            fprintf(stderr, "FTP: Error reading file stats...\n");
          }else{
            char *perms = malloc(9);
            memset(perms,0,9);
       */
            /* Convert time_t to tm struct */
/*
            rawtime = statbuf.st_mtime;
            time = localtime(&rawtime);
            strftime(timebuff,80,"%b %d %H:%M",time);
            str_perm((statbuf.st_mode & ALLPERMS), perms);
            dprintf(connection,
                "%c%s %5d %4d %4d %8d %s %s\r\n", 
                (entry->d_type==DT_DIR)?'d':'-',
                perms,statbuf.st_nlink,
                statbuf.st_uid, 
                statbuf.st_gid,
                statbuf.st_size,
                timebuff,
                entry->d_name);
          }
        }
        write_state(state);
        state->message = "226 Directory send OK.\n";
        state->mode = NORMAL;
        close(connection);
        close(state->sock_pasv);

      }else if(state->mode == CLIENT){
        state->message = "502 Command not implemented.\n";
      }else{
        state->message = "425 Use PASV or PORT first.\n";
      }
    }
    closedir(dp);
    chdir(cwd_orig);
  }else{
    state->message = "530 Please login with USER and PASS.\n";
  }
  state->mode = NORMAL;
  write_state(state);
} */
//}

int main(int argc, char const *argv[]) { 
  int server_fd, new_socket, valread; 
  struct sockaddr_in address; 
  int addrlen = sizeof(address); 
  Command *cmd = malloc(sizeof(Command));
  State *state = malloc(sizeof(State));
  char buffer[1024] = {0}; 

  char *hello = "220 Service ready for new user.\n"; 
	
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
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { 
      perror("accept"); 
      exit(EXIT_FAILURE); 
    }
 
    close(server_fd);

    //Faz envio de mensagem de boas vindas ao cliente que fez a requisição de conexão entrante	
    write(new_socket, hello, strlen(hello)); 

    //Aguarda por comando do cliente
    while (valread = read(new_socket, buffer, 1024)){
      if(!(valread > 1024)){
        buffer[1023] = '\0';
        printf("Usuario %s enviou o comando: %s\n",(state->username==0)?"unknown":state->username,buffer);
        parse_command(buffer,cmd);
        state->connection = new_socket;
          
        /* Ignore non-ascii char. Ignores telnet command */
        if(buffer[0]<=127 || buffer[0]>=0){
          response(cmd,state);
        }

        if(strcmp(cmd, "QUIT") == 0 || strcmp(cmd, "quit") == 0){
          response(cmd, state);
          close(new_socket);     
          return 0;
        }
        memset(buffer,0, 1024);
        memset(cmd,0,sizeof(cmd));
      }else{
        /* Read error */
        perror("server:read");
      }
    } //Fecha colchete do segundo loop
  } //Fecha colchete do loop principal
} //Fecha chave do main 

