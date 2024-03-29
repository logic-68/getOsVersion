#ifndef FILE_H
#define FILE_H

#include <utils.h>
char *reading_param(char *init_file_path, char *param);
void copy_file(char *sourcefile, char *destfile);
void copy_dir(char *sourcedir, char *destdir);
void touch_file(char *destfile);
int file_compare(char *fname1, char *fname2);
int erase_folder(char *dest_path);
int file_exists(char *fname);
int dir_exists(char *dname);
int fgetc_pointer(int fp);
void make_log(char *error_file_path);
void write_log(char *error_file_path, char *error);
#endif
