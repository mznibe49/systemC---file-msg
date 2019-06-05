#ifndef _INITSTRUCTS_H_
#define _INITSTRUCTS_H_

//char** init_tabCirc(int nb_msg, int len_max);
MESSAGE* open_File(const char* nom, int option);
MESSAGE* init_File(const char* nom, int option, int nb_msg, int len_max, int value_option);
MESSAGE* init_File_Anon(const char* nom, int option, int nb_msg, int len_max, int value_option);


#endif
