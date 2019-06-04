#include <stdio.h>
#include <sys/types.h>

void porExemplo(){

  int pid;

  pid = fork();

  if(pid == 0){
    printf("\nProcesso filho %d\n", pid);
  }else{
    sleep(10);
    printf("\nProcesso Pai %d\n", pid);
  }



}

int main(){
  

  
  porExemplo();
  
  


  return 0;
}


