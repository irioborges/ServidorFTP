#include <stdio.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <getopt.h>

#include <unistd.h>
#include <dirent.h>
#include <time.h>

//char *getcwd(char *buf, size_t size);

#define PORT 21 

//Porta utilizada para conexão de dados
int int_porta, int_quit = 0, int_taxa;

float flt_taxa_utilizada_up, flt_taxa_utilizada_down;

//TYPE 
char *type = "a"; //Inicializada temporariamente com "a" para ASC II
char *code = "n"; //Inicializada temporariamente com "n" para Non-print

//MODE
//S - Stream
//B - Block
//C - Compressed
char *mode = "S"; //Inicializada temporariamente com "S" Stream

//STRU
//F - File (no record structure)
//R - Record structure
//P - Page structure
char *stru = "F"; // Inicializada temporariamente com F File

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

typedef struct thread_argumento {
  int server_fd;
  int int_socket;
//  int teste;
} Thread_argumento;

char *PWD(){
  long path_max;
  char *cwd;
  size_t size;
  char *buf;
  char *ptr, *ptr2;

  ptr2 = "257 teste\n";
  printf("\n%s\n", ptr2);

  if (path_max == -1)
    size = 1024;
  else if (path_max > 10240)
    size = 10240;
  else
    size = path_max;
  
  for (buf = ptr = NULL; ptr == NULL; size *= 2) {
    if ((buf = realloc(buf, size)) == NULL) {
    }

    ptr = getcwd(buf, size);
    printf("\n%s\n", ptr);
    
  }
  
  printf("%s", ptr);

  ptr = strcat(ptr, "\n");

  return ptr;
}

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

  if(strcmp(cmd->command, "USER") == 0 || strcmp(cmd->command, "user") == 0){  
    state->message = "331 User name okay, need password.\n";
  }

  if(strcmp(cmd->command, "PASS") == 0 || strcmp(cmd->command, "pass") == 0){
    state->message = "230 User logged in, proceed.\n";
  }

  if(strcmp(cmd->command, "SYST") == 0 || strcmp(cmd->command, "syst") == 0){
    state->message = "215 Linux\n"; 
  }

  if(strcmp(cmd->command, "PWD") == 0 || strcmp(cmd->command, "pwd") == 0){
    state->message = PWD();  
  }

  if(strcmp(cmd->command, "QUIT") == 0 || strcmp(cmd->command, "quit") == 0){
    state->message = "221 Tchau!!\n";
    int_quit = -1;	
  }

  if(strcmp(cmd->command, "TYPE") == 0 || strcmp(cmd->command, "type") == 0) {
    //Possíveis retornos
    //200
    //500, 501, 504, 421, 530
    //*type = (char) cmd->arg[0];
    //type = cmd->arg[0];
    //code = cmd->arg[2];
    //printf("\n%s\n", cmd->arg);
    //sscanf(cmd->arg, "%s-%s", type, code);
    state->message = "200 Command okay.\n";
  }

  if(strcmp(cmd->command, "MODE") == 0 || strcmp(cmd->command, "mode") == 0) {
    //Possíveis retornos
    //200
    //500, 501, 504, 421, 530
    //*type = (char) cmd->arg[0];
    //type = cmd->arg[0];
    //code = cmd->arg[2];
    //printf("\n%s\n", cmd->arg);
    //sscanf(cmd->arg, "%s-%s", type, code);
    state->message = "200 Command okay.\n";
  }

  if(strcmp(cmd->command, "STRU") == 0 || strcmp(cmd->command, "stru") == 0) {
    //Possíveis retornos
    //200
    //500, 501, 504, 421, 530
    //*type = (char) cmd->arg[0];
    //type = cmd->arg[0];
    //code = cmd->arg[2];
    //printf("\n%s\n", cmd->arg);
    //sscanf(cmd->arg, "%s-%s", type, code);
    state->message = "200 Command okay.\n";
  }


  if(strcmp(cmd->command, "PORT") == 0 || strcmp(cmd->command, "port") == 0){
    //printf("%i\n", calcularPorta(cmd));
    int_porta = calcularPorta(cmd);
    state->message = "200 Command okay.\n";
  }

  if(strcmp(cmd->command, "RETR") == 0){
    
    struct sockaddr_in address2; 
    struct sockaddr_in serv_addr2; 
    int sock2 = 0, valread, soma_bytes; 
    char *primeiraMSG = "150 File status okay; about to open data connection.\n";
    char *segundaMSG = "226 Requested file action successful\n";  
    char buffer[1024] = {0}, linha[80], linha2; 

    FILE *fp;

    clock_t tInicio, tFim;
    clock_t tInicio2, tFim2;
    double tDecorrido, soma_tDecorrido = 0;
    
    if (!(fp = fopen(cmd->arg,"r+")))  /* Caso ocorra algum erro na abertura do arquivo..*/ 
  	{                           /* o programa aborta automaticamente */
  	  printf("Erro! Impossivel abrir o arquivo!\n");
  	  exit(1);
  	}

    if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
      printf("\n Socket creation error \n"); 
      return -1; 
    } 
   
    memset(&serv_addr2, '0', sizeof(serv_addr2)); 
   
    serv_addr2.sin_family = AF_INET; 
    serv_addr2.sin_port = htons(int_porta); //htons(calcularPorta(cmd)); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr2.sin_addr)<=0) { 
      printf("\nInvalid address/ Address not supported \n"); 
      return -1; 
    } 
   
    if (connect(sock2, (struct sockaddr *)&serv_addr2, sizeof(serv_addr2)) < 0) { 
      printf("\nConnection Failed %i\n", serv_addr2.sin_port); 
      return -1; 
    }
 
    write(state->connection, primeiraMSG, strlen(primeiraMSG));
    
    
    /*
    while(!feof(fp)){
      printf("%s", linha); 
      //strcat(linha, "\n");
      send(sock2, linha, strlen(linha), 0 ); 
      fscanf(fp, "%s", linha); 
    } 

    while((linha2=fgetc(fp) )!= EOF ){
      //printf("%s", linha2); 
      //strcat(linha, "\n");
      send(sock2, linha2, strlen(linha2), 0 ); 
    } */

    //tInicio2 = clock();
    while((fgets(linha, sizeof(linha), fp)) != NULL){
      tInicio = clock();
      send(sock2, linha, strlen(linha), 0 ); 
      tFim = clock();
      //tDecorrido = ((tFim - tInicio) / (CLOCKS_PER_SEC / 1000));﻿
      tDecorrido = ( (double) (tFim - tInicio) ) / CLOCKS_PER_SEC;

      soma_tDecorrido += tDecorrido;
      soma_bytes += strlen(linha);

      printf("\n%lf\n", tDecorrido);
    }
    //tFim2 = clock();
    printf("\n Outro tempo %d \n", tFim2 - tInicio2);
    printf("\n Total de tempo decorrido %lf \n", soma_tDecorrido);
    printf("\n Total de bytes enviados %d \n", soma_bytes);
    printf("\nTaxa de bytes enviados: %lf\n", (soma_bytes / soma_tDecorrido) / 1000000);

    close(sock2);
    state->message = segundaMSG;
    printf("\n%s\n", state->message);
  }


  if(strcmp(cmd->command, "LIST") == 0){
    struct dirent **namelist;
    int n;
    FILE *teste;
    char msg[1024];

    teste=popen("ls -ls","r");

    struct sockaddr_in address2; 
    struct sockaddr_in serv_addr2; 
    int sock2 = 0, valread; 
    char *primeiraMSG = "150 File status okay; about to open data connection.\n";
    char *segundaMSG = "226 Requested file action successful\n";  
    char buffer[1024] = {0}; 
    
    if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
      printf("\n Socket creation error \n"); 
      return -1; 
    } 
   
    memset(&serv_addr2, '0', sizeof(serv_addr2)); 
   
    serv_addr2.sin_family = AF_INET; 
    serv_addr2.sin_port = htons(int_porta); //htons(calcularPorta(cmd)); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr2.sin_addr)<=0) { 
      printf("\nInvalid address/ Address not supported \n"); 
      return -1; 
    } 
   
    if (connect(sock2, (struct sockaddr *)&serv_addr2, sizeof(serv_addr2)) < 0) { 
      printf("\nConnection Failed %i\n", serv_addr2.sin_port); 
      return -1; 
    }
 
   write(state->connection, primeiraMSG, strlen(primeiraMSG));
   printf("chegou acima do send\n"); 
   

   while(1){
    if(fread(msg,1023,1,teste)!=1)
      if(feof(teste))//testa se chegou no EOF
      msg[1023]='\0';
      printf("%s",msg);
      send(sock2, msg, strlen(msg), 0 ); 
      break;//Interrompe while
    } 
    
    
    printf("passou do send%s\n", primeiraMSG); 
    close(sock2);
    state->message = segundaMSG;
    printf("\n%s\n", state->message);
  }

  if(strcmp(cmd->command, "NOOP") == 0){
    state->message = "200 OK\n";
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

#define TRUE 1
#define FALSE 0

//Função de teste de permutação para 2 parâmetros. 2!(2 fatorial)
int isPermutationOf(const char *s1, const char *s2){
  return FALSE;
}

void *conexaoControle(void *arg){
  char buffer[1024] = {0}; 
  char *hello = "220 Service ready for new user.\n"; 

  int valread, server_fd, int_socket;

  Thread_argumento *thread_argumento;

  Command *cmd = malloc(sizeof(Command));
  State *state = malloc(sizeof(State));

  thread_argumento = (Thread_argumento *) arg;
  
  server_fd = thread_argumento->server_fd;
  int_socket = thread_argumento->int_socket;

  //Faz envio de mensagem de boas vindas ao cliente que fez a requisição de conexão entrante	
  write(int_socket, hello, strlen(hello)); 

  //Aguarda por comando do cliente
  while (valread = read(int_socket, buffer, 1024) && int_quit == 0){
    if(!(valread > 1024)){
      buffer[1023] = '\0';
      printf("Usuario %s enviou o comando: %s\n", (state->username==0) ? "anonymous" : state->username, buffer);
      parse_command(buffer,cmd);
      state->connection = int_socket;
          
      /* Ignore non-ascii char. Ignores telnet command */ 
      if(buffer[0]<=127 || buffer[0]>=0){
        response(cmd,state);
      }

      if(int_quit == -1){
        int_quit = 0;
	pthread_exit(NULL);
      }

      memset(buffer,0, 1024);
      memset(cmd,0,sizeof(cmd));
    }else{
      /* Read error */
      perror("server:read");
    }
  } //Fecha colchete do segundo loop
}

int main(int argc, char const *argv[]) { 
  //Declaração de variáveis locais ao método main 
  /* */ struct sockaddr_in address;   

  /* */ int server_fd, int_socket, pid, int_num_threads = 0, i = 0, int_opt, int_t = 0; 
  /* */ int addrlen = sizeof(address); 

  /* */ int flags = 0, opt, int_taxa = 0, int_controle_congestionamento = 0;
  /* */ int nsecs = 0, tfnd = 0;

  /* */ pthread_t thread[100];
  /* */ Thread_argumento thread_argumento[100];
  //Fim de declaração de variáveis


  //Tratamento dos argumentos passados no argv
  while ((opt = getopt(argc, argv, "ct:")) != -1) {
    switch (opt) {
      case 'c':
        int_controle_congestionamento = -1;
	break;
      case 't':
        int_taxa = atoi(optarg);
        break;
      default: /* '?' */
        fprintf(stderr, "Uso: %s [-t taxa mínima de envio em kbps] [-c ativa controle de congestionamento]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  //if (optind <= argc) {
  //  fprintf(stderr, "Uso: %s [-t taxa mínima de envio em kbps] [-c ativa controle de congestionamento]\n", argv[0]);
  //  exit(EXIT_FAILURE);
  //}
  //Fim de tratamento dos argumentos

  //Cria o socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
    perror("socket failed"); 
    exit(EXIT_FAILURE); 
  } 

  address.sin_family = AF_INET; 
  address.sin_addr.s_addr = INADDR_ANY; 
  address.sin_port = htons(PORT); 
	
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
                                                                                                                            
  printf("Servidor escutando com as seguintes especificacoes:\n");
  printf("Porta = %i; Taxa mínima de transferencia = %ikbps; Controle de congestionamento =", PORT, int_taxa);
  (int_controle_congestionamento == -1) ? printf(" ativado;\n") : printf(" desativado;\n");

  while(1) {
    int_quit = 0;    

    //Espera por requisição de conexão entrante		
    if ((int_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { 
      perror("accept"); 
      exit(EXIT_FAILURE); 
    }

    thread_argumento->server_fd = server_fd;
    thread_argumento->int_socket = int_socket;

    pthread_create(&(thread[int_num_threads]), NULL, conexaoControle, &thread_argumento[0]);

    int_num_threads++;

    /* Esse bloco de código é o antigo com fork(), pode ser implementado uma opção com ele futuramente	 
    pid = fork();

    if(pid == 0){
      close(server_fd);
      //Faz envio de mensagem de boas vindas ao cliente que fez a requisição de conexão entrante	
      write(int_socket, hello, strlen(hello)); 

      //Aguarda por comando do cliente
      while (valread = read(int_socket, buffer, 1024) && int_quit == 0){
      
        if(!(valread > 1024)){
          buffer[1023] = '\0';
          printf("Usuario %s enviou o comando: %s\n", (state->username==0) ? "anonymous" : state->username, buffer);
          parse_command(buffer,cmd);
          state->connection = int_socket;
          
          /* Ignore non-ascii char. Ignores telnet command */ /*
          if(buffer[0]<=127 || buffer[0]>=0){
            response(cmd,state);
          }

	  if(int_quit == -1) {
	    printf("\nLiberando socket\n");
	  
            sleep(1);
	    close(server_fd);
            //sleep(1);
            //close(int_socket);
          }
          memset(buffer,0, 1024);
          memset(cmd,0,sizeof(cmd));
        }else{
          /* Read error */
          //perror("server:read");
        //}
      //} //Fecha colchete do segundo loop
      //sleep(2);
      //close(server_fd); 
    /* }else{
      printf("\nFechando conexao\n");
      close(int_socket);
    } */
  }
} //Fecha chave do main 

