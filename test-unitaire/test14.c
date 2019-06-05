#include "msg_file.h"

// teste de signal SIGUSR1
// utiliser ecriture simple avec test1.c

int main(int argc, char const *argv[]) {

  const char *name = "/Goku";

  // nom, option, nb_msg, len_max
  MESSAGE *f =  msg_connect(name, O_CREAT | O_WRONLY  ,10,200,-1);
  printf("nom file dans le main %s\n\n",f->ent->nom_file);
  int pid = 0;

  for(int i = 0; i < 1; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);

      MESSAGE *ff = msg_connect(name,O_RDWR ,-1);

      printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);

      if(i == 0){
        // ecrire un block de msg

        use_signal(ff);

      }
      if(i== 1){
        readv(ff,4);
      }

      if(i == 2){

        readv(ff,5);
      }

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

  return 0;

}
