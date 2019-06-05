#include "msg_file.h"
//lire au depart , bloquage

int main(int argc, char const *argv[]) {
  const char *name = "/toto";
  // nom, option, nb_msg, len_max
  MESSAGE *f =  msg_connect(name, O_CREAT | O_RDWR | O_EXCL,10,200,-1);
  printf("nom file dans le main %s\n",f->ent->nom_file);
  int pid = 0;
  for(int i = 0; i < 4; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);
      MESSAGE *ff = msg_connect(name,O_RDWR,-1);
      printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);

      if(i== 0){
        char recu[100];
        msg_receive(ff,recu,100);
        printf("%d eme a lu : %s\n",i+1,recu);
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
