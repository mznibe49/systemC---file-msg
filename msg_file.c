#include "msg_file.h"


int traiter_flags(int nb_arg, int option){
  if(nb_arg == 0){
    if(option == O_RDWR /*|| option == O_WRONLY || option == O_RDONLY*/){
      printf("O_RDWR\n");
      return 1;
    } else if(option == O_WRONLY){
      printf("O_WRONLY\n");
      return 1;
    } else if(option == O_RDONLY){
      printf("O_RDONLY\n");
      return 1;
    }
  } else if (nb_arg == 2){

    if( option == (O_CREAT | O_RDWR) ){
      printf("O_CREAT | O_RDWR\n");
      return 2;
    } else if ( option == ( O_CREAT | O_WRONLY) ){
      printf("O_CREAT | O_WRONLY\n");
      return 2;
    } else if ( option ==  ( O_CREAT | O_RDONLY) ) {
      printf("O_CREAT | O_RDONLY\n");
      return 2;
    }

    if(option == (O_CREAT | O_RDWR | O_EXCL)){
      printf("O_CREAT | O_RDWR | O_EXCL\n");
      return 3;
    } else if (option == (O_CREAT | O_WRONLY | O_EXCL)){
      printf("O_CREAT | O_WRONLY | O_EXCL\n");
      return 3;
    } else if (option == (O_CREAT | O_RDONLY | O_EXCL)) {
      printf("O_CREAT | O_RDONLY | O_EXCL\n");
      return 3;
    }

  }
  return -1; // le cas ou ça n'a pas fonctionner
}

size_t msg_message_size(MESSAGE *file){
  return file->ent->msg_existant;
}

size_t msg_capacite(MESSAGE *file){
  return file->ent->len_max;
}

size_t msg_nb(MESSAGE *file){
  return file->ent->nb_msg;
}

int msg_disconnect(MESSAGE* file){
  int nproc = file->ent->nproces--;
  int res;
  if(nproc == 0){
    res = munmap(file->ent,sizeof(Entete));
    if(file->ent->nom_file != NULL) msg_unlink(file->ent->nom_file); // quand ce n est pas une file anonyme
  } else {
    res = munmap(file->ent,sizeof(Entete));
  }
  free(file);
  return res;
}

int msg_unlink(const char *nom){
  return shm_unlink(nom);
}


int writev(MESSAGE *file, void **tab, int nb_msg/*, size_t len*/){


  printf("Nombre de msg qu'on peut ajouter dans la file %ld : \n", file->ent->nb_msg - file->ent->msg_existant);
  printf("Nombre de msg qu'on souhaite ajouter : %d\n",nb_msg);

  if (nb_msg > (file->ent->nb_msg - file->ent->msg_existant) ){
    printf("operation d ajout par lot impossible\n");
    return -1;
  }

  // on rend cette operation atomique
  // ecriture de bloque

  if(file->ent->flag == 0){

    file->lots = nb_msg;
    file->ent->flag = 1; // qq ecrit par lots
    int old_nproces = file->ent->nproces;
    printf("le block de msg ecrit est : \n");

    for(int i = 0; i<nb_msg; i++){
      sleep(2);
      msg_send(file,tab[i],sizeof(tab[i]));
      printf("%s\n",(char *)tab[i]);
    }

    int new_nproces = file->ent->nproces;
    int blocked_process = new_nproces - old_nproces;

    // nombre de processus bloqué si jamais y en a (gratir l'atomicité)
    for(int i = 0; i<blocked_process; i++){
      if(sem_post(&(file->ent->sem_lots)) < 0){
        perror("err sem wait (msg_send)");
        exit(1);
      }
    }
    file->ent->flag = 0;
    file->lots = 1;


  } else {
    printf("operation d ajout par lot impossible (un autre proc est entrain d'ecrire des msg par lot)\n");
    return -1;
  }


  return 0;


}


void readv(MESSAGE *file , int nb_msg){


  if(file->ent->flag == 0){

    file->lots = nb_msg;
    file->ent->flag = 1; // qq lit ou ecrit par lots
    int old_nproces = file->ent->nproces;

    printf("le block de msg %d lu est : \n",nb_msg);
    for(int i = 0; i<nb_msg; i++){
      sleep(1);
      char recu[100];
      msg_tryreceive(file,recu,100);
      printf("%d eme a lu : %s\n",i+1,(char *)recu);
    }

    int new_nproces = file->ent->nproces;
    int blocked_process = new_nproces - old_nproces;

    // nombre de processus bloqué si jamais y en a (gratir l'atomicité)
    for(int i = 0; i<blocked_process; i++){
      if(sem_post(&(file->ent->sem_lots)) < 0){
        perror("err sem wait (msg_send)");
        exit(1);
      }
    }
    file->ent->flag = 0;
    file->lots = 1;


  } else {
    printf("operation de lecture par lot impossible (un autre proc est entrain d'ecrire/lire des msg par lot)\n");

  }
}

int check_sending_flag(int option){
  if ( option == O_RDONLY  ||  option == (O_CREAT | O_RDONLY | O_EXCL)  || option == (O_CREAT | O_RDONLY) ){
    return -1;
  }
  return 1;
}

int check_receiving_flag(int option){
  // le O_RDONLY est un cas exceptionnel vu que son & avec n'importe quel autre flag ca donnera tjrs 0
  if ( option == O_WRONLY  ||  option == ((O_CREAT | O_WRONLY | O_EXCL))   || option == ((O_CREAT | O_WRONLY)) ){
    return -1;
  }
  return 1;
}


int msg_send_with_sem(MESSAGE *file, void *msg, size_t len){


  if(sem_wait(&(file->ent->sem_send)) < 0){  // ici on garantis qu'il ya un seul ecrivain qui ecrit a la fois
    perror("err sem wait sem_send");
    exit(1);
  }


    int pos_len_msg = file->ent->last * (file->ent->len_max + sizeof(size_t)); // position len_msg dans i eme bloque
    int pos_msg =  pos_len_msg + sizeof(size_t); // position msg dans i eme bloque



    memcpy(file->tab + pos_len_msg, &len, sizeof(size_t)); // ecrire la taille du msg
    memcpy(file->tab + pos_msg,msg,len); // ecrire le msg ,  len ici est deja un sizeof(msg) donné dans l'argument !!

    if(file->ent->first == -1){   // premier cas on ecrit quand la file est vide (vierge)
      // on envoie dans le last et puis on incremente les deux valeurs
      file->ent->first++;
      file->ent->last++;
    } else { // la file n est pas vierge
      if(file->ent->last < file->ent->nb_msg){
        file->ent->last++;
      } else {
        file->ent->last = 0;
      }
    }

    int val_sem;
    sem_getvalue(&(file->ent->sem_vide),&val_sem);
    printf("get value sem vide : %d\n",val_sem );

    if(file->ent->enregistrement != -1 && file->ent->msg_existant == 0 && val_sem == 0 ){ // si la file est vide (0msg) et aucun proc est en attente et ya qq enregistrer
      printf("j'envoie le signal a %d\n",file->ent->enregistrement );
      kill(file->ent->enregistrement,SIGINT);
    }

    file->ent->msg_existant++;

  // synchronizer
  size_t size = sizeof(Entete) + (file->ent->len_max + sizeof(size_t))*file->ent->nb_msg;
  msync(file->ent,size, MS_SYNC);

  if(sem_post(&(file->ent->sem_send)) < 0){
       perror("err sem post sem_send");
       exit(1);
  }
  return 0;
}



int msg_send(MESSAGE *file, void *msg, size_t len){
  int res;
  if(file->ent->flag!= 0 && file->lots == 1){ // tu attend
    // tu met une sem que tu debloque a la fin du writev
    printf("un processu est entrain d'ecrire un lots de msg :s\n");
    if( sem_wait(&(file->ent->sem_lots)) < 0){
      perror("err sem wait (msg_send)");
      exit(1);
    }
  }
  if(check_sending_flag(file->option) == -1){
    printf("Vous etes en mode ecriture et vous esseyez de lire\n");
    return -1;
  }
  // on veut un seul proc qui ecrit a la fois ( on peut avoir un sem qui lit et ecrit a la fois)

  if(file->ent->len_max < len ){
    errno = EMSGSIZE;
    perror("taille de votre msg est plus grand que le taille max msg de la file\n");
    return -1;
  }

  // pour notifier au envoyeur qu'ils peuvent envoyer -> la file n'est pas pleine
  if(sem_wait(&(file->ent->sem_plein)) < 0){
    perror("err sem wait (msg_send)");
    exit(1);
  }
  // on ecrit dans la file avant de liberer les lecteur bloqué
  res = msg_send_with_sem(file,msg,len);

  // pour notifier aux recepteurs qu'il peuvent recevoir -> la file n'est pas vide
  if(sem_post(&(file->ent->sem_vide)) < 0){
    perror("err sem post (msg_send)");
    exit(1);
  }

  return res;
}



int msg_receive_with_sem(MESSAGE *file, void *msg, size_t len){

  int res = 0;
  // ici on garantis qu'il ya un seul lecteur qui lit a la fois
  if(sem_wait(&(file->ent->sem_receive)) < 0){
       perror("err sem wait sem_send");
       exit(1);
  }
  int pos_len_msg = file->ent->first * (file->ent->len_max + sizeof(size_t)); // position len_msg dans i eme bloque
  int pos_msg =  pos_len_msg + sizeof(size_t); // position msg dans i eme bloque

  // renvoyer -1 si la taille est inferieur a "len" donné en arguemnt
  size_t *var = malloc(sizeof(size_t));// = 30;
  memcpy(var, file->tab + pos_len_msg, sizeof(size_t));
  //printf("la taille du msg lu stocké dans la memoire %ld\n",var[0] );
  if(len < var[0]){
    printf("la taille du msg qui fait la lecture est petite\n");
    // mettent EMSGSIZE dans errno.
    res =  -1;
  } else {
    res = var[0]; // nombre d'octect lu
  }
  //free(var); on free dans le main la variable qui contient l'appelle de la fonction

  memcpy(msg, file->tab + pos_msg, len); // ecrire la taille du msg

  if(file->ent->first == -1){   // premier cas on ecrit quand la file est vide (vierge)
      // on va jamais tomber dans ce cas parceque le processus sera en attente
      // on ne va pas tomber dans le cas ou last == first parceque les processus vont etre bloqué
  } else { // la file n est pas vierge
    if(file->ent->first < file->ent->nb_msg){
      file->ent->first++;
    } else {
      file->ent->first = 0;
    }
  }

  file->ent->msg_existant--;


  // synchronizer
  size_t size = sizeof(Entete) + (file->ent->len_max + sizeof(size_t))*file->ent->nb_msg;
  msync(file->ent,size, MS_SYNC);

  if(sem_post(&(file->ent->sem_receive)) < 0){
       perror("err sem post sem_send");
       exit(1);
  }
  return res;
}


ssize_t msg_receive(MESSAGE *file, void *msg, size_t len){
  if(file->ent->flag!= 0 && file->lots == 1){ // tu attend
    // tu met une sem que tu debloque a la fin du writev
    printf("un processu est entrain de lire un lots de msg :s\n");
    if( sem_wait(&(file->ent->sem_lots)) < 0){
      perror("err sem wait (msg_send)");
      exit(1);
    }
  }

  if(check_receiving_flag(file->option) == -1){
    printf("Vous etes en mode de lecture ou vous esseyez d'ecrire\n");
    return -1;
  }
  if(file->ent->len_max < len ){
    errno = EMSGSIZE;
    perror("taille de votre msg est plus grand que le taille max msg de la file\n");
    return -1;
  }
  // pour notifier aux recepteurs qu'il ne peuvent plus recevoir quand la file est vide
  if(sem_wait(&(file->ent->sem_vide)) < 0){
    perror("err sem wait (msg_send)");
    exit(1);
  }
  int res = msg_receive_with_sem(file,msg,len);
  // pour que les envoyeur peuvent envoyer de nouveau (si la file est pleine)
  if(sem_post(&(file->ent->sem_plein)) < 0){
    perror("err sem post (msg_send)");
    exit(1);
  }

  return res;
}



ssize_t msg_receive_with_trysem(MESSAGE *file, void *msg, size_t len){
  int res = 0;
  // ici on garantis qu'il ya un seul lecteur qui lit a la fois
  if(sem_trywait(&(file->ent->sem_receive)) < 0){
       perror("err sem wait sem_send");
       exit(1);
  }

  int pos_len_msg = file->ent->first * (file->ent->len_max + sizeof(size_t)); // position len_msg dans i eme bloque
  int pos_msg =  pos_len_msg + sizeof(size_t); // position msg dans i eme bloque

  // renvoyer -1 si la taille est inferieur a "len" donné en arguemnt
  size_t *var = malloc(sizeof(size_t));// = 30;  // penser a free cette valeur
  memcpy(var, file->tab + pos_len_msg, sizeof(size_t));
  //printf("la taille du msg lu stocké dans la memoire %ld\n",var[0] );
  if(len < var[0]){
    printf("la taille du msg qui fait la lecture est petite\n");
    // mettent EMSGSIZE dans errno.
    res =  -1;
  } else {
    res = var[0]; // nombre d'octect lu
  }
  memcpy(msg, file->tab + pos_msg, len); // ecrire la taille du msg
  if(file->ent->first == -1){   // premier cas on ecrit quand la file est vide (vierge)
  } else { // la file n est pas vierge
    if(file->ent->first < file->ent->nb_msg){
      file->ent->first++;
    } else {
      file->ent->first = 0;
    }
  }
  file->ent->msg_existant--;
  size_t size = sizeof(Entete) + (file->ent->len_max + sizeof(size_t))*file->ent->nb_msg;
  msync(file->ent,size, MS_SYNC);

  if(sem_post(&(file->ent->sem_receive)) < 0){
       perror("err sem post sem_send");
       exit(1);
  }
  return res;
}

ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len){

  if(file->ent->flag!= 0 && file->lots == 1){ // tu attend
    // tu met une sem que tu debloque a la fin du writev
    printf("un processu est entrain de lire un lots de msg :s\n");
    if( sem_wait(&(file->ent->sem_lots)) < 0){
      perror("err sem wait (msg_send)");
      exit(1);
    }
  }

  if(check_receiving_flag(file->option) == -1){
    printf("Vous etes en mode de lecture ou vous esseyez d'ecrire\n");
    return -1;
  }
  // verification de taille
  if(file->ent->len_max < len ){
    errno = EMSGSIZE;
    perror("taille de votre msg est plus grand que le taille max msg de la file\n");
    return -1;
  }
// pour notifier aux recepteurs qu'il ne peuvent plus recevoir quand la file est vide
  if(sem_trywait(&(file->ent->sem_vide)) == -1){
    errno = EAGAIN;
    perror("procc non bloqué a la lecture : ");
    return -1;
  }
  ssize_t res = msg_receive_with_trysem(file,msg,len);
  // pour que les envoyeur peuvent envoyer de nouveau (si la file est pleine)
  if(sem_post(&(file->ent->sem_plein)) < 0){
    perror("err sem post (msg_send)");
    exit(1);
  }
  return res;
}

int msg_send_with_trysem(MESSAGE *file, void *msg, size_t len){

  if(sem_trywait(&(file->ent->sem_send)) == -1){  // ici on garantis qu'il ya un seul lecteur qui lit a la fois
    printf("procc non bloqué a l'ecriture 2\n");
    return -1;
  }

  int pos_len_msg = file->ent->last * (file->ent->len_max + sizeof(size_t)); // position len_msg dans i eme bloque
  int pos_msg =  pos_len_msg + sizeof(size_t); // position msg dans i eme bloque

  memcpy(file->tab + pos_len_msg, &len, sizeof(size_t)); // ecrire la taille du msg
  memcpy(file->tab + pos_msg,msg,len); // ecrire le msg ,  len ici est deja un sizeof(msg) donné dans l'argument !!

  if(file->ent->first == -1){   // premier cas on ecrit quand la file est vide (vierge)
    // on envoie dans le last et puis on incremente les deux valeurs
    file->ent->first++;
    file->ent->last++;
  } else { // la file n est pas vierge
    if(file->ent->last < file->ent->nb_msg){
      file->ent->last++;
    } else {
      file->ent->last = 0;
    }
  }




  file->ent->msg_existant++;

  // synchronizer
  size_t size = sizeof(Entete) + (file->ent->len_max + sizeof(size_t))*file->ent->nb_msg;
  msync(file->ent,size, MS_SYNC);

  if(sem_post(&(file->ent->sem_send)) < 0){
       perror("err sem post sem_send");
       exit(1);
  }
  return 0; // a modifé
}

int msg_trysend(MESSAGE *file, void *msg, size_t len){
  if(file->ent->flag!= 0 && file->lots == 1){ // tu attend
    // tu met une sem que tu debloque a la fin du writev
    printf("un processu est entrain d'ecrire un lots de msg :s\n");
    if( sem_wait(&(file->ent->sem_lots)) < 0){
      perror("err sem wait (msg_send)");
      exit(1);
    }
  }

  if(check_sending_flag(file->option) == -1){
    printf("Vous etes en mode ecriture et vous esseyez de lire\n");
    return -1;
  }

  if(file->ent->len_max < len ){
    errno = EMSGSIZE;
    perror("taille de votre msg est plus grand que le taille max msg de la file\n");
    return -1;
  }
  //int res = 0;
  // pour notifier au envoyeur qu'ils peuvent envoyer -> la file n'est pas pleine
  if(sem_trywait(&(file->ent->sem_plein)) == -1){
    errno = EAGAIN;
    perror("procc non bloqué a l'ecriture : ");
    return -1;
  }
  // on ecrit dans la file avant de liberer les lecteur bloqué
  int res = msg_send_with_trysem(file,msg,len);

  // pour notifier aux recepteurs qu'il peuvent recevoir -> la file n'est pas vide
  if(sem_post(&(file->ent->sem_vide)) < 0){
    perror("err sem post (msg_send)");
    exit(1);
  }

  return res;
}

int se_enregistrer(MESSAGE *file){
  if(file->ent->enregistrement == -1){
    file->ent->enregistrement = getpid();
    printf("Enregistrement du proc %d reussi\n",getpid() );
    return 1;
  } else {
    printf("Enregistrement echoué ! :s , un autre proc est deja enregistrer\n");
  }
  return -1;
}

volatile int boucle = 0;

static void handler_msg_rec(int sig){
  if(sig == SIGINT){
    printf("var boucle changed\n");
    boucle = 1;
  } else {
    printf("var boucle not changed\n");
  }
  return;
}

int use_signal(MESSAGE *file){

  int res = se_enregistrer(file);
  if(res == 1){

    struct sigaction act;
    act.sa_handler = handler_msg_rec;

    // bloquer les signaux pendant l execution de SIGUSER1
    sigfillset(&act.sa_mask);

    act.sa_flags = 0;

    // installer le handler pr SIGUSER1
    if(sigaction(SIGINT, &act, NULL) == -1){
      printf("Err signaction\n");
      return -1;
    }

    while(1){
      printf("Hello ^^""\n");
      sleep(3);
      if(boucle == 1){
        char recu[100];
        msg_receive(file,recu,100);
        printf("processus a lu : %s\n",recu);
        file->ent->enregistrement = -1;
        boucle = 0;
        return 1;
      }
    }
  }
  return -1;
}


//MESSAGE *msg_connect( const char *nom, int option, size_t nb_msg, size_t len_max );
MESSAGE* msg_connect(const char* nom, int option, ...){

  va_list ap;
  size_t val;
  va_start(ap, option);

  // func() : gerer les signaux avec le OR
  int nb_arg = 0;
  size_t nb_msg = 0, len_max = 0;

  while (1){
    val = va_arg(ap, int);
    if(val == -1) break;
    nb_arg++;
    switch (nb_arg) {
      // on recupere les valeurs a initialisé dans la struct
      case 1: nb_msg = val; printf("nb_msg : %ld\n",nb_msg); break;
      case 2: len_max = val; printf("len_max msg : %ld\n",len_max); break;
    }
  }
  //on teste le nbr d'arguments et on gerer l'appelle des funcs en fonction des flags
  MESSAGE *file = NULL;
  int verif = traiter_flags(nb_arg,option);
  if(nom == NULL){ // file anonyme
    file = init_File_Anon(nom,option,nb_msg, len_max,verif);
  } else {
    // on part du principe que la verif a etait faite
    if(verif == 2 || verif == 3){  // le cas du creat - on fait le init
      file = init_File(nom,option,nb_msg, len_max,verif);
    } else if (verif == 1){ // les autre cas
      file = open_File(nom,option);
    }
  }

  va_end(ap);
  return file; // pour l'instant , on envoie le MESSAGE a ce moment

}
