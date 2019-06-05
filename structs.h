#ifndef _STRUCTS_H_
#define _STRUCTS_H_

#include <semaphore.h>


typedef struct Entete Entete;
struct Entete {
  const char* nom_file;
  //size_t project_size;
  size_t len_max; // longueur maximal d'un msg
  size_t nb_msg; // nombre de message que la file peut contenir (capacite)
  size_t msg_existant;
  int first; // l'indice du premier element de la file : first == last : tab plein
  int last; // l’indice de premier élément libre de tableau : first == -1 : tab vide
  int block_flag; // si c'est le dernier processus, cette var est mise a 0 sin elle est a -1
  size_t nproces;// nbr de processus connecté

  sem_t sem_send; // on garentis qu'il y a un ecrivain a la fois
  sem_t sem_receive; // -- -- --- ---         lecteur a la fois

  sem_t sem_plein; // bloqué processus qui envoie quand la file est full
  sem_t sem_vide; //  bloqué a la reception quand la file est vide

  int flag; // quand c est a 0, personne ne fait une ecriture ou lecture par lot
  //int flag_reader;

  sem_t sem_lots; // ecriture / lecture par lot

  int enregistrement;

};


typedef struct MESSAGE MESSAGE;
struct MESSAGE {
  int option; // O_RDWR, O_WRONLY..etc
  Entete *ent; // la structure sur laquelle on va faire un mmap
  void *tab;
  int lots; // lot de msg a lire ou a ecrire
};

#endif
