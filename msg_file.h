#ifndef _FUNCS_H_
#define _FUNCS_H_

#include "structs.h"
#include "initStructs.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>


//MESSAGE *msg_connect( const char *nom, int option, size_t nb_msg, size_t len_max );
MESSAGE* msg_connect(const char* nom, int option, ...);
int msg_disconnect(MESSAGE *file);
// retourne 0 si OK et −1 en cas d’erreur.
/*
déconnecte le processus de la file de messages (la file n’est pas détruite, mais MESSAGE * devient
inutilisable)
*/
int msg_unlink(const char *nom); // suppression de la file apres que le dernier processus se deconnecte
/*
 Par contre une fois msg_unlink exécutée par un processus, toutes les tentatives de
msg_connect doivent échouer !!
msg_unlink est juste une petite fonction qui fait appel
à shm_unlink pour supprimer l’objet mémoire implémentant la file de messages
*/
int msg_send(MESSAGE *file, void *msg, size_t len);
int msg_trysend(MESSAGE *file, void *msg, size_t len);

ssize_t msg_receive(MESSAGE *file, void *msg, size_t len);
ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len);

// getteur de la file
size_t msg_message_size(MESSAGE *);
size_t msg_capacite(MESSAGE *);
size_t msg_nb(MESSAGE *);


int writev(MESSAGE *file, void **tab, int nb_msg/*, size_t len*/);
void readv(MESSAGE *file , int nb_msg);

int se_enregistrer(MESSAGE *file);
int use_signal(MESSAGE *file);

#endif
