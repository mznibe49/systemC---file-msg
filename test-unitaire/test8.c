#include "msg_file.h"
//  teste de msg_trysend avec une file de taille 1 et 2 processus

int main(int argc, char const *argv[]) {

  const char *name = "/toto";

  // nom, option, nb_msg, len_max
  MESSAGE *f =  msg_connect(name, O_CREAT | O_EXCL,1,200,-1);
  printf("nom file dans le main %s\n",f->ent->nom_file);
  int pid = 0;

  for(int i = 0; i < 3; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);

      MESSAGE *ff = msg_connect(name,O_RDWR,-1);
      printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);
      printf("nb msg dans la file : %ld\n",ff->ent->msg_existant);

      if(i == 1){
        char * str = "kris2";
        printf("sizeof(str) dans le main est %ld\n",sizeof(str));
        msg_trysend(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
      }
      if(i== 0){
        char * str = "kris";
        printf("sizeof(str) dans le main est %ld\n",sizeof(str));
        msg_send(ff,str,sizeof(str));
        printf("%d eme a ecrit %s\n",i+1,str);
      }
      printf("indice prochaine case libre %d\n\n",ff->ent->last);
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



/*
//test de msg_tryreceive sur une file vide

int main(int argc, char const *argv[]) {
  const char *name = "/toto";
  // nom, option, nb_msg, len_max
  MESSAGE *f =  msg_connect(name, O_CREAT | O_EXCL,12,200,-1);
  printf("nom file dans le main %s\n",f->ent->nom_file);
  int pid = 0;

  for(int i = 0; i < 3; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);
      MESSAGE *ff = msg_connect(name,O_RDWR,-1);
      printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);
      printf("nb msg dans la file : %ld\n",ff->ent->msg_existant);
      if(i== 0){
        char recu[100];
        size_t len_read = msg_tryreceive(ff,recu,100);
        printf("%d eme a lu : %s, nbr d'octect lu sont : %ld\n",i+1,recu,len_read);
      }
      printf("indice prochaine case libre %d\n\n",ff->ent->last);
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


*/
