#include "msg_file.h"

// envoie reception d'un tableau d'entier

int main(int argc, char const *argv[]) {

  const char *name = "/Goku";

  // nom, option, nb_msg, len_max
  MESSAGE *f =  msg_connect(name, O_CREAT | O_WRONLY  ,10,200,-1);
  printf("nom file dans le main %s\n\n",f->ent->nom_file);
  int pid = 0;

  for(int i = 0; i < 4; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);

      MESSAGE *ff = msg_connect(name,O_RDWR ,-1);

      printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);

      if(i == 3){
        int *var = malloc(sizeof(int));
        var[0] = 20;
        msg_send(ff,var,sizeof(var));
        printf("%d eme a ecrit %d\n",i+1,var[0]);
      }
      if(i == 0){

        int var[4]; // = malloc(sizeof(int []));// = 30;
        var[0] = 20;
        var[1] = 30;
        var[2] = 4;
        var[3] = 67;
        msg_send(ff,var,sizeof(var));
        printf("%d eme a ecrit un tab \n",i+1);

      }
      if(i== 1){


        int recu[100];
        msg_receive(ff,recu,100);
        printf("element du tab lu : \n");

        for(int i = 0; i<4; i++){
          printf("%d\n",recu[i]);
        }
      }
      printf("indice prochaine case libre %d\n",ff->ent->last);
      if(i == 2){
        char * str = "kris2";
        printf("sizeof(str) dans le main est %ld\n",sizeof(str));
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
