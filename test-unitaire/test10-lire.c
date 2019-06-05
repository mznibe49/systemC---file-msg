#include "msg_file.h"

// app a test1 ou test-ecriture10

int main(int argc, char const *argv[]) {

  const char *name = "/Goku";

  // nom, option, nb_msg, len_max
  MESSAGE *f =  msg_connect(name, O_CREAT | O_RDWR  ,10,200,-1);
  printf("nom file dans le main %s\n\n",f->ent->nom_file);
  int pid = 0;
  // un pere avec des fils qui lisent
  for(int i = 0; i < 10; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);

      MESSAGE *ff = msg_connect(name,O_RDWR ,-1);

      printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);

      char recu[100];
      msg_receive(ff,recu,100);
      printf("%d eme a lu : %s\n",i+1,recu);
      printf("indice prochaine case libre %d\n",ff->ent->last);
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
  // les free des processus dans le disconnect
  //printf("pere fini\n");

  return 0;

}
