/*Documentation

Name        : Varun M M
Student id  : 26018_042
Batch id    : 26018D
Start Date  : 24/09/2026
End date    : 04/10/2026

Description:
The following main functions are implemented in the MP3 Tag Reader and Editor project to view and modify MP3 metadata.

main()                  :-> Handles command line arguments and selects the required operation such as help, view or edit.
                         -> Validates the arguments and calls the appropriate function for viewing or editing MP3 tag details.

view_details()          :-> Reads and displays the metadata details from an MP3 file.
                         -> Displays the song title, artist, album, year, genre and comment stored in the six supported ID3 frames.

edit_details()          :-> Modifies the selected metadata field of an MP3 file based on the edit option provided.
                         -> Updates the selected ID3 tag while preserving the remaining metadata and audio data.

frame_id()              :-> Reads the 4-byte frame ID from the MP3 file.
                         -> Stores the frame ID as a string for identifying the required metadata frame.

frame_size()            :-> Reads the 4-byte frame size from the MP3 file.
                         -> Converts the frame size bytes into an unsigned integer value.

frame_encode()          :-> Reads the encoding byte from the MP3 frame.
                         -> Checks whether the encoding byte is successfully read.

read_frame_data()       :-> Reads the frame text data from the MP3 file.
                         -> Stores a maximum of 99 characters in the 100-byte frame buffer and skips the remaining frame data.

display_view()          :-> Identifies the frame ID and displays the corresponding metadata field.
                         -> Displays the title, artist, album, year, genre and comment in a readable format.


Supported Edit Options:
-t                      :-> Edit Song Title (TIT2)
-A                      :-> Edit Artist Name (TPE1)
-a                      :-> Edit Album Name (TALB)
-y                      :-> Edit Year (TYER)
-c                      :-> Edit Genre / Content (TCON)
-C                      :-> Edit Comment (COMM)


Sample input:
$ ./a.out -v sample.mp3

Sample output:
------------SELECTED VIEW DETAILS------------

---------------------------------------------
    MP3 TAG READER AND EDITOR FOR ID3v2
---------------------------------------------
 Title   : Sunnysunny
 Artist  : Yo Yo Honey Singh - [SongsPk.CC]
 Album   : Yaariyan
 Year    : 2013
 Genre   : Bollywood Music - [SongsPk.CC]
 Comment : eng
---------------------------------------------
       DETAILS DISPLAYED SUCCESSFULLY        
---------------------------------------------

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mp3.h"

int main(int argc, char *argv[])
{
    char *new_data;
    size_t total_length = 0;

    /* Check whether any command-line argument is given */
    if (argc < 2)
    {
        printf(CYAN "\n-------------------------------------------------------------------------------\n\n" RESET);
        printf(RED "ERROR: INVALID ARGUMENTS\n" RESET);
        printf(YELLOW "To view please pass like: ./a.out -v sample.mp3\n" RESET);
        printf(YELLOW "To edit please pass like: ./a.out -e sample.mp3 -t New Title\n" RESET);
        printf(YELLOW "To get help please pass like: ./a.out -h or ./a.out -help\n" RESET);
        printf(CYAN "-------------------------------------------------------------------------------\n" RESET);
        return 1;
    }

    /* Check whether help option is selected */
    if (strcmp(argv[1], "-h") == 0 ||
        strcmp(argv[1], "-help") == 0)
    {
        printf("\n");

        printf(GREEN "┌──────────────────────────────────────────────────────────┐\n");
        printf("│            MP3 TAG READER & EDITOR HELP MENU             │\n");
        printf("├──────────────────────────────────────────────────────────┤\n");
        printf("│                                                          │\n");
        printf("│  COMMANDS                                                │\n");
        printf("│                                                          │\n");
        printf("│  1. -v       View MP3 file contents                      │\n");
        printf("│  2. -e       Edit MP3 file contents                      │\n");
        printf("│  3. -h       Display help menu                           │\n");
        printf("│  4. -help    Display help menu                           │\n");
        printf("│                                                          │\n");
        printf("│  EDIT OPTIONS                                            │\n");
        printf("│                                                          │\n");
        printf("│  -t         Edit Song Title                              │\n");
        printf("│  -A         Edit Artist Name                             │\n");
        printf("│  -a         Edit Album Name                              │\n");
        printf("│  -y         Edit Year                                    │\n");
        printf("│  -c         Edit Genre / Content                         │\n");
        printf("│  -C         Edit Comment                                 │\n");
        printf("│                                                          │\n");
        printf("│  EXAMPLES                                                │\n");
        printf("│                                                          │\n");
        printf("│  ./a.out -v song.mp3                                     │\n");
        printf("│  ./a.out -e song.mp3 -t New Title                        │\n");
        printf("│  ./a.out -e song.mp3 -A New Artist                       │\n");
        printf("│  ./a.out -e song.mp3 -a New Album                        │\n");
        printf("│  ./a.out -e song.mp3 -y 2026                             │\n");
        printf("│  ./a.out -e song.mp3 -c Rock                             │\n");
        printf("│  ./a.out -e song.mp3 -C New Comment                      │\n");
        printf("│                                                          │\n");
        printf("└──────────────────────────────────────────────────────────┘\n" RESET);
        /* Stop the program after displaying help */
        return 0;
    }

    /* VIEW */
    else if (strcmp(argv[1], "-v") == 0)
    {
        /* Check whether correct number of arguments is given */
        if (argc != 3)
        {
            printf(CYAN "---------------------------------------------\n" RESET);
            printf(RED "ERROR: Invalid arguments for view\n" RESET);
            printf(YELLOW "Usage: ./a.out -v sample.mp3\n" RESET);
            printf(CYAN "---------------------------------------------\n" RESET);

            return 1;
        }

        /* Display selected view operation */
        printf("\n");
        printf(CYAN "------------SELECTED VIEW DETAILS------------\n" RESET);
        printf("\n");

        /* Display program heading */
        printf(CYAN "---------------------------------------------\n" RESET);
        printf(GREEN "    MP3 TAG READER AND EDITOR FOR ID3v2\n" RESET);
        printf(CYAN "---------------------------------------------\n" RESET);

        /* Call function to display MP3 tag details */
        view_details(argv[2]);

        /* Display successful view message */
        printf(CYAN "---------------------------------------------\n" RESET);
        printf(GREEN "       DETAILS DISPLAYED SUCCESSFULLY        \n" RESET);
        printf(CYAN "---------------------------------------------\n" RESET);
    }

    /* EDIT */
    else if (strcmp(argv[1], "-e") == 0)
    {
        /*
         * Required format:
         * ./a.out -e song.mp3 -t New Title
         *
         * argv[1] = -e
         * argv[2] = song.mp3
         * argv[3] = -t
         * argv[4...] = New Title
         */

        /* Check whether all required arguments are given */
        if (argc < 5)
        {
            printf(CYAN "------------------------------------------------------------\n" RESET);
            printf(RED "ERROR: Invalid Arguments For Edit\n" RESET);
            printf(YELLOW "Usage: ./a.out -e sample.mp3 -t New Title\n" RESET);
            printf(YELLOW "Options: -t -A -a -y -c -C\n" RESET);
            printf(CYAN "------------------------------------------------------------\n" RESET);

            return 1;
        }

        /* Validate the edit option */
        if (strcmp(argv[3], "-t") != 0 &&
            strcmp(argv[3], "-A") != 0 &&
            strcmp(argv[3], "-a") != 0 &&
            strcmp(argv[3], "-y") != 0 &&
            strcmp(argv[3], "-c") != 0 &&
            strcmp(argv[3], "-C") != 0)
        {
            printf(RED "ERROR: Invalid edit option\n" RESET);
            return 1;
        }

        /* Calculate the total length of the new data */
        for (int i = 4; i < argc; i++)
        {
            /* Add current argument length */
            total_length += strlen(argv[i]);

            /* Add space between words */
            if (i < argc - 1)
                total_length++;
        }

        /* Allocate memory for the complete new data */
        new_data = malloc(total_length + 1);

        /* Check whether memory allocation was successful */
        if (new_data == NULL)
        {
            printf(RED "ERROR: MEMORY ALLOCATION FAILED.\n" RESET);
            return 1;
        }

        /* Initialize the new data string */
        new_data[0] = '\0';

        /* Combine all command-line words into one string */
        for (int i = 4; i < argc; i++)
        {
            /* Add current word to new_data */
            strcat(new_data, argv[i]);

            /* Add space between words */
            if (i < argc - 1)
                strcat(new_data, " ");
        }

        /* Display selected edit operation */
        printf(CYAN "\n--------SELECTED EDIT DETAILS----------\n" RESET);

        /* Call function to edit selected MP3 tag */
        edit_details(argv[2], argv[3], new_data);

        printf(CYAN "\n------------EDITED DETAILS-------------\n" RESET);

        /* Release dynamically allocated memory */
        free(new_data);
    }

    /* Handle invalid command-line option */
    else
    {
        printf(RED "ERROR: Invalid option\n" RESET);
        printf(YELLOW "Use ./a.out -h or ./a.out -help for help.\n" RESET);
        return 1;
    }

    /* End the program successfully */
    return 0;
}