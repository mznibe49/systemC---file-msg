#include "msg_file.h"

int main(int argc, char const *argv[]) {

  //Le meme processus ecrit 10 fois

  /*MESSAGE *ff = msg_connect("/Goku",O_WRONLY,-1); // le mm ppas ecrire plusieurs fois? oui
  for(int i = 0; i<10; i++){
    char * str = "Quentin <3 je t'aime";
    //printf("sizeof(str) dans le main est %ld\n",sizeof(str));
    msg_send(ff,str,sizeof(str));
    printf("proc a ecrit %s\n\n",str);
    sleep(1);
  }*/


  // un pere avec des fils qui ecrivent


  for(int i = 0; i<10; i++){

    int pid = fork();
    if(pid == 0){
      sleep(1);
      MESSAGE *ff = msg_connect("/Goku",O_RDWR,-1); // le mm ppas ecrire plusieurs fois?
      char * str = "Beta800";
      //printf("sizeof(str) dans le main est %ld\n",sizeof(str));
      msg_send(ff,str,sizeof(str));
      printf("proc a ecrit %s\n\n",str);
      break;
    } else if(pid < 0){
      perror("err fork");
      exit(1);
    } else { // pere
      wait(NULL);
      continue;
    }

  }













  //const char *name = "/Goku";

  // le meme qui ecrit 10 fois
  /*MESSAGE *ff = msg_connect("/Goku",O_RDWR ,-1);

  //printf("RR\n" );
  //for(int i = 0; i<10; i++){

    printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);
    char * str = "Quentin <3";
    printf("sizeof(str) dans le main est %ld\n",sizeof(str));
    msg_send(ff,str,sizeof(str));
    //printf("%d eme a ecrit %s\n",i+1,str);
    printf("indice prochaine case libre %d\n",ff->ent->last);
    printf("nb msg dans la file : %ld\n\n\n",ff->ent->msg_existant);*/
  //}


  // nom, option, nb_msg, len_max
  //MESSAGE *f =  msg_connect(name, O_WRONLY  ,10,200,-1);
  //printf("nom file dans le main %s\n\n",f->ent->nom_file);
  /*int pid = 0;

  for(int i = 0; i < 10; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);

      MESSAGE *ff = msg_connect(name,O_WRONLY ,-1);

      printf("RR\n" );
      printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);

      char * str = "Quentin <3";
      printf("sizeof(str) dans le main est %ld\n",sizeof(str));
      msg_send(ff,str,sizeof(str));
      printf("%d eme a ecrit %s\n",i+1,str);

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
  }*/
  // les free des processus dans le disconnect
  //printf("pere fini\n");

  return 0;

}
