#include "msg_file.h"

//une file bloqué par des ecrivain (on a un teste de msg_trysend avec errno) et debloqué par un lecteur
// faire app a test1

int main(int argc, char const *argv[]) {
  const char *name = "/Goku";
  // nom, option, nb_msg, len_max
  MESSAGE *f =  msg_connect(name, O_CREAT | O_WRONLY,2,200,-1);
  printf("nom file dans le main %s\n",f->ent->nom_file);
  int pid = 0;
  for(int i = 0; i < 5; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);
      //printf("t1\n");
      MESSAGE *ff = msg_connect(name,O_RDWR,-1);
      printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);

      if(i == 0){
        char * str = "samu";
        msg_send(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
      }
      if(i == 1){
        char * str = "kris";
        msg_send(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
      }
      if(i== 2){
        char * str = "bob";
        msg_trysend(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
      }
      if(i == 3){
        char * str = "Jack";
        msg_send(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
      }
      if(i == 4){
        char * str = "Daniel";
        msg_send(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
      }
      printf("nb msg dans la file : %ld\n\n\n",ff->ent->msg_existant);
      break;
    } else if(pid < 0){
      perror("err fork");
      exit(1);
    } else { // pere
      wait(NULL);
      continue;
    }
  }
  return 0;
}
