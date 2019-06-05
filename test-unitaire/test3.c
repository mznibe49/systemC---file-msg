//ecrire (entier), lire(entier), ecrit( char *)
#include "msg_file.h"


int main(int argc, char const *argv[]) {
  const char *name = "/toto";
  // nom, option, nb_msg, len_max
  MESSAGE *f =  msg_connect(name, O_CREAT | O_WRONLY,2,200,-1);
  printf("nom file dans le main %s\n",f->ent->nom_file);
  int pid = 0;

  for(int i = 0; i < 6; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);
      MESSAGE *ff = msg_connect(name,O_RDWR,-1);
      printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);

      if(i == 0){
        int *var = malloc(sizeof(int));// = 30;
        var[0] = 20;
        msg_send(ff,var,sizeof(var));
        printf("%d eme a ecrit %d\n",i+1,var[0]);
      }
      if(i == 2){
        char * str = "kris";
        msg_send(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
      }
      if(i== 1){
        int recu[100];
        msg_receive(ff,recu,100);
        printf("%d eme a lu : %d\n",i+1,recu[0]);
      }
      if(i== 3){
        char recu[100];
        msg_receive(ff,recu,100);
        printf("%d eme a lu : %s\n",i+1,recu);
      }
      if(i == 4){
        char * str = "jack";
        msg_send(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
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
