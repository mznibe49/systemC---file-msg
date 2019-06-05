#include "msg_file.h"

// Lecture de msg par lots
// pour testé l atomicité lancé un deuxieme terminal avec test1 (une ecriture ecriture)


int main(int argc, char const *argv[]) {

  const char *name = "/Goku";

  // nom, option, nb_msg, len_max
  MESSAGE *f =  msg_connect(name, O_CREAT | O_WRONLY  ,10,200,-1);
  printf("nom file dans le main %s\n\n",f->ent->nom_file);
  int pid = 0;

  for(int i = 0; i < 3; i++){
    pid = fork();
    if(pid == 0){
      sleep(1);

      MESSAGE *ff = msg_connect(name,O_RDWR ,-1);

      printf("nb proc %ld  name  %s\n",ff->ent->nproces,ff->ent->nom_file);

      if(i == 0){
        // ecrire un block de msg


        // on enleve le sleep 2 pour write pour mieux testé
        char *str  = "msg11111";
        char *str1 = "msg2";
        char *str2 = "msg3";
        char *str3 = "msg4";
        char *str4 = "msg5";
        char *str5 = "msg6";
        char *str6 = "msg7";
        char *str7 = "msg8";
        char *str8 = "msg9";


        //int nbmsg = 5;

        void *bloc[9] = {str,str1,str2,str3,str4,str5,str6,str7,str8};
        writev(ff,bloc,9);

      }
      if(i== 1){
        readv(ff,4);
      }

      if(i == 2){

        readv(ff,6);
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
