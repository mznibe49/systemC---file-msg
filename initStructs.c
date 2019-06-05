#include "msg_file.h"


MESSAGE* open_File(const char* nom, int option){



  MESSAGE *file = malloc(sizeof(MESSAGE));
  // ici le shm doit tjrs etre en lecture + ecritue parcequ'on a besoin de recuperer la taille du shm
  int shm_file = shm_open(nom, O_RDWR , S_IRUSR | S_IWUSR );
  if(shm_file == -1) {
    perror("Erreur de shm_open dans open_File 1");
    exit(1);
  }
  struct stat bufStat;
  if( fstat( shm_file, &bufStat ) == -1){
    perror("fstat");
    exit(1);
  }
  size_t size = bufStat.st_size;
  if(ftruncate(shm_file, size) == -1) {
    perror("Erreur de ftruncate dans open_File 2");
    exit(1);
  }


  void *M = mmap(NULL, size, /*flag */PROT_READ | PROT_WRITE,  MAP_SHARED, shm_file, 0);
  if(M == MAP_FAILED){
    perror("ERR map file dans open_File 3");
    exit(1);
  }

  file->ent = M;
  file->tab = M + sizeof(Entete);
  file->option = option;
  file->ent->nproces++;
  file->lots = 1;

  return file;
}

int shm_flag(int Option){
return 0;
}

MESSAGE* init_File(const char* nom, int option, int nb_msg, int len_max, int value_option){

  MESSAGE *file = malloc(sizeof(MESSAGE));

  int shm_file;// = shm_open(nom, option, S_IRUSR | S_IWUSR );

  if(value_option == 3)
    shm_file = shm_open(nom, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR );
  else  shm_file = shm_open(nom, O_CREAT | O_RDWR , S_IRUSR | S_IWUSR );


  if(shm_file == -1) {
    perror("Erreur de shm_open dans init_File 1");
    exit(1);
  }
  //size_t est unsigned
  // pr chaque msg on met sa taille avant  // au depart on part du principe qu'on a que des char
  size_t size = sizeof(Entete) + (len_max + sizeof(size_t))*nb_msg;
  if(ftruncate(shm_file, size) == -1) {
    perror("Erreur de ftruncate dans init_File 2");
    exit(1);
  }

  file->option = option;
  void* M =  mmap(NULL, size, /*flag*/ PROT_READ | PROT_WRITE , MAP_SHARED,shm_file,0);
  if(M == MAP_FAILED){
    perror("ERR map file dans init_File 3");
    exit(1);
  }

  file->ent = M;
  file->ent->nproces = 1;
  file->ent->len_max = len_max;
  file->ent->nb_msg = nb_msg; // nb_msg est la capacité de la file
  file->ent->first = -1; // a la creation il n'ya aucun msg a lire
  file->ent->last = 0; // a la creation le premiere case libre est la premiere
  file->ent->msg_existant = 0;
  file->ent->nom_file = nom;
  file->ent->flag = 0;
  file->lots = 1;
  file->ent->enregistrement = -1;


  sem_init(&(file->ent->sem_send), 1, 1);
  sem_init(&(file->ent->sem_receive), 1, 1);
  sem_init(&(file->ent->sem_plein),1,nb_msg); // bloque processus qui envoie quand la file est full
  sem_init(&(file->ent->sem_vide),1,0); // bloqué processus qui reçois quand la file est vide

  sem_init(&(file->ent->sem_lots),1,0); // bloqué ecrivain qd ya une ecriture par lot

  // pointe vers sa position dans la projection
  file->tab = M + sizeof(Entete);
  return file;

}

MESSAGE* init_File_Anon(const char* nom, int option, int nb_msg, int len_max, int value_option){

  MESSAGE *file = malloc(sizeof(MESSAGE));

  //size_t est unsigned
  // pr chaque msg on met sa taille avant  // au depart on part du principe qu'on a que des char
  size_t size = sizeof(Entete) + (len_max + sizeof(size_t))*nb_msg;

  file->option = O_RDWR;
  void* M =  mmap(NULL, size, PROT_READ | PROT_WRITE , MAP_SHARED | MAP_ANONYMOUS,-1,0);
  if(M == MAP_FAILED){
    perror("ERR map file dans init_File_Anon 1");
    exit(1);
  }
  file->ent = M;
  file->ent->nproces = 1;
  file->ent->len_max = len_max;
  file->ent->nb_msg = nb_msg; // nb_msg est la capacité de la file
  file->ent->first = -1; // a la creation il n'ya aucun msg a lire
  file->ent->last = 0; // a la creation le premiere case libre est la premiere
  file->ent->msg_existant = 0;
  file->ent->nom_file = nom;
  file->ent->flag = 0;
  file->lots = 1;
  file->ent->enregistrement = -1;

  sem_init(&(file->ent->sem_send), 1, 1);
  sem_init(&(file->ent->sem_receive), 1, 1);
  sem_init(&(file->ent->sem_plein),1,nb_msg); // bloque processus qui envoie quand la file est full
  sem_init(&(file->ent->sem_vide),1,0); // bloqué processus qui reçois quand la file est vide

  sem_init(&(file->ent->sem_lots),1,0); // bloqué ecrivain qd ya une ecriture par lot


  file->tab = M + sizeof(Entete);
  return file;
}
