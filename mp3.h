#ifndef MP3_H
#define MP3_H

#include <stdio.h>

/* Defined colors for terminal output */
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[0;37m"
#define RESET   "\033[0m"

/* Function Declaration to view MP3 tag details */
void view_details(char *file_name);

/* Function Declaration to read frame ID */
int frame_id(FILE *fptr, char *id);

/* Function Declaration to read frame encoding */
int frame_encode(FILE *fptr, char *encoding);

/* Function Declaration to read frame size */
unsigned int frame_size(FILE *fptr);

/* Function Declaration to read frame data */
int read_frame_data(FILE *fptr, unsigned int *frame_size, char *frame_data);

/* Function Declaration to display frame details */
void display_view(char *frame_id, char *frame_data);

/* Function Declaration to edit MP3 tag details */
void edit_details(char *file_name, char *option, char *new_data);

#endif