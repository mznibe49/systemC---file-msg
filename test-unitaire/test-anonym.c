#include "msg_file.h"

int main(int argc, char const *argv[]) {


  // nom, option, nb_msg, len_max
  MESSAGE *ff =  msg_connect(NULL, 0 ,10,200,-1);
  //printf("nom file dans le main %s\n\n",f->ent->nom_file);
  int pid = 0;
  for(int i = 0; i < 4; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);
      //MESSAGE *ff = msg_connect(name,O_RDWR,-1);
      printf("\nnb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);
      printf("nb msg dans la file : %ld\n",ff->ent->msg_existant);
      if(i == 0){
        char * str = "samu";
        msg_send(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
      }
      if(i == 2){
        char * str = "kris";
        msg_send(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
      }
      if(i== 1){
        char recu[100];
        msg_receive(ff,recu,100);
        printf("%d eme a lu : %s\n",i+1,recu);
      }
      if(i == 3){
        char recu[100];
        msg_receive(ff,recu,100);
        printf("%d eme a lu : %s\n",i+1,recu);
        msg_unlink(ff->ent->nom_file);
      }
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
