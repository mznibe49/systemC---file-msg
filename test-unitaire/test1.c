#include "msg_file.h"


// ecriture - lecture externe
int main(int argc, char const *argv[]) {
  MESSAGE *ff = msg_connect("/Goku",O_RDWR,-1);

  //lecture seul pour test7
  /*char recu[100];
  msg_receive(ff,recu,100);
  printf("proc a lu : %s\n",recu);*/

  // ecriture seul pour tes-lec10 et test 12! test14(signaux)

  char * str = "kris2";
  printf("sizeof(str) dans le main est %ld\n",sizeof(str));
  msg_send(ff,str,sizeof(str));
  printf("proc a ecrit %s\n",str);


  // lecture multiple pour test7
  /*
  for (int i = 0; i < 1; i++) {
    sleep(1);
    char recu[100];
    msg_receive(ff,recu,100);
    printf("%d eme a lu : %s\n\n",i+1,recu);
  }*/


  /*int option = O_RDONLY;
  if ( option == O_WRONLY  ||  option == ((O_CREAT | O_WRONLY | O_EXCL))   || option == ((O_CREAT | O_WRONLY)) ){
    printf("not Good\n" );
    printf("%d\n",option );
    printf("%d\n", (option & O_WRONLY));
    printf("%d\n",O_WRONLY );
  } else {
    printf("Good\n");
  }*/
  //printf("%d\n",O_WRONLY );
  //printf("%s\n",res);

  return 0;
}
