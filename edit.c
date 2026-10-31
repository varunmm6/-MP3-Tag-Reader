#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mp3.h"

void edit_details(char *file_name, char *option, char *new_data)
{
    FILE *fptr;
    FILE *temp;

    /* Store frame ID and selected frame ID */
    char id[5];
    char selected_id[5];
    char field_name[20];

    /* Store frame sizes */
    unsigned int size;
    unsigned int new_size;
    unsigned int data_size;

    /* Store frame encoding and flags */
    char encoding;
    char flags[2];

    /* Store frame data */
    unsigned char *frame_data;

    /* Track whether the selected frame is edited */
    int edited = 0;

    /* Check the selected edit option */
    if (strcmp(option, "-t") == 0)
    {
        /* Select title frame */
        strcpy(selected_id, "TIT2");
        strcpy(field_name, "TITLE");
    }
    else if (strcmp(option, "-A") == 0)
    {
        /* Select artist frame */
        strcpy(selected_id, "TPE1");
        strcpy(field_name, "ARTIST");
    }
    else if (strcmp(option, "-a") == 0)
    {
        /* Select album frame */
        strcpy(selected_id, "TALB");
        strcpy(field_name, "ALBUM");
    }
    else if (strcmp(option, "-y") == 0)
    {
        /* Select year frame */
        strcpy(selected_id, "TYER");
        strcpy(field_name, "YEAR");
    }
    else if (strcmp(option, "-c") == 0)
    {
        /* Select genre frame */
        strcpy(selected_id, "TCON");
        strcpy(field_name, "MUSIC");
    }
    else if (strcmp(option, "-C") == 0)
    {
        /* Select comment frame */
        strcpy(selected_id, "COMM");
        strcpy(field_name, "COMMENT");
    }
    else
    {
        /* Display error for invalid edit option */
        printf(RED "ERROR! INVALID EDIT OPTION.\n" RESET);
        return;
    }

    /* Find the file extension */
    char *extension = strrchr(file_name, '.');

    /* Checking whether the file is an MP3 file */
    if (extension == NULL || strcmp(extension, ".mp3") != 0)
    {
        /* Display invalid file format error */
        printf(RED "ERROR! INVALID FILE FORMAT.\n" RESET);
        return;
    }

    /* Open the original MP3 file */
    fptr = fopen(file_name, "rb");

    /* Check whether the original file opened successfully */
    if (fptr == NULL)
    {
        /* Display file opening error */
        printf(RED "ERROR! FILE NOT OPENED.\n" RESET);
        return;
    }

    /* Create a temporary MP3 file */
    temp = fopen("temp.mp3", "wb");

    /* Check whether the temporary file was created */
    if (temp == NULL)
    {
        /* Display temporary file creation error */
        printf(RED "ERROR! TEMP FILE NOT CREATED.\n" RESET);

        /* Close the original file */
        fclose(fptr);
        return;
    }

    /* Store the 10-byte ID3 header */
    unsigned char header[10];

    /* Read the ID3 header */
    if (fread(header, 1, 10, fptr) != 10)
    {
        /* Display header reading error */
        printf(RED "ERROR! HEADER NOT READ.\n" RESET);

        /* Close both files */
        fclose(fptr);
        fclose(temp);

        /* Delete the incomplete temporary file */
        remove("temp.mp3");
        return;
    }

    /* Copy the ID3 header to the temporary file */
    fwrite(header, 1, 10, temp);

    /* Process the six supported MP3 frames */
    for (int i = 0; i < 6; i++)
    {
        /* Read the frame ID */
        if (fread(id, 1, 4, fptr) != 4)
        {
            /* Display frame ID reading error */
            printf(RED "ERROR! FRAME ID NOT READ.\n" RESET);

            /* Close both files */
            fclose(fptr);
            fclose(temp);

            /* Delete temporary file */
            remove("temp.mp3");
            return;
        }

        /* Add null character to make frame ID a string */
        id[4] = '\0';

        /* Read the frame size */
        size = frame_size(fptr);

        /* Check whether the frame size is valid */
        if (size < 1)
        {
            /* Display invalid frame size error */
            printf(RED "ERROR! INVALID FRAME SIZE.\n" RESET);

            /* Close both files */
            fclose(fptr);
            fclose(temp);

            /* Delete temporary file */
            remove("temp.mp3");
            return;
        }

        /* Read the two frame flag bytes */
        if (fread(flags, 1, 2, fptr) != 2)
        {
            /* Display frame flag reading error */
            printf(RED "ERROR! FRAME FLAGS NOT READ.\n" RESET);

            /* Close both files */
            fclose(fptr);
            fclose(temp);

            /* Delete temporary file */
            remove("temp.mp3");
            return;
        }

        /* Read the frame encoding byte */
        if (fread(&encoding, 1, 1, fptr) != 1)
        {
            /* Display encoding reading error */
            printf(RED "ERROR! ENCODING NOT READ.\n" RESET);

            /* Close both files */
            fclose(fptr);
            fclose(temp);

            /* Delete temporary file */
            remove("temp.mp3");
            return;
        }

        /* Remove the encoding byte from the frame size */
        data_size = size - 1;

        /* Allocate memory according to frame data size */
        frame_data = malloc(data_size);

        /* Check whether memory allocation was successful */
        if (frame_data == NULL && data_size > 0)
        {
            /* Display memory allocation error */
            printf(RED "ERROR! MEMORY ALLOCATION FAILED.\n" RESET);

            /* Close both files */
            fclose(fptr);
            fclose(temp);

            /* Delete temporary file */
            remove("temp.mp3");
            return;
        }

        /* Read the existing frame data */
        if (data_size > 0)
        {
            if (fread(frame_data, 1, data_size, fptr) != data_size)
            {
                /* Display frame data reading error */
                printf(RED "ERROR! FRAME DATA NOT READ.\n" RESET);

                /* Free dynamically allocated memory */
                free(frame_data);

                /* Close both files */
                fclose(fptr);
                fclose(temp);

                /* Delete temporary file */
                remove("temp.mp3");
                return;
            }
        }

        /* Check whether the current frame is the selected frame */
        if (strcmp(id, selected_id) == 0 && edited == 0)
        {
            /* Calculate new frame size */
            new_size = strlen(new_data) + 1;

            /* Write the frame ID */
            fwrite(id, 1, 4, temp);

            /* Store new frame size in big-endian format */
            unsigned char new_size_bytes[4];

            new_size_bytes[0] = (new_size >> 24) & 0xFF;
            new_size_bytes[1] = (new_size >> 16) & 0xFF;
            new_size_bytes[2] = (new_size >> 8) & 0xFF;
            new_size_bytes[3] = new_size & 0xFF;

            /* Write the new frame size */
            fwrite(new_size_bytes, 1, 4, temp);

            /* Write the original frame flags */
            fwrite(flags, 1, 2, temp);

            /* Set text encoding to ISO-8859-1 */
            unsigned char new_encoding = 0;

            /* Write the new encoding byte */
            fwrite(&new_encoding, 1, 1, temp);

            /* Write the new tag data */
            fwrite(new_data, 1, strlen(new_data), temp);

            /* Mark the frame as edited */
            edited = 1;

            //Progress bar for updating the frame
            int i, delay, j;

            for (i = 1; i <= 100; i++)
            {
                printf(GREEN "\rUpdating [" RESET);

                for (j = 1; j <= 23; j++)
                {
                    j <= i * 23 / 100 ? printf(GREEN "#" RESET) : printf(" ");
                }

                printf(GREEN "] %i%%" RESET, i);
                fflush(stdout);

                for (delay = 0xffffff; delay--;);
            }

            /* Display successful updated message */
            printf(GREEN "\n      %s UPDATED SUCCESSFULLY.\n" RESET, field_name);
        }
        else
        {
            /* Write the unchanged frame ID */
            fwrite(id, 1, 4, temp);

            /* Store original frame size in big-endian format */
            unsigned char size_bytes[4];

            size_bytes[0] = (size >> 24) & 0xFF;
            size_bytes[1] = (size >> 16) & 0xFF;
            size_bytes[2] = (size >> 8) & 0xFF;
            size_bytes[3] = size & 0xFF;

            /* Write the original frame size */
            fwrite(size_bytes, 1, 4, temp);

            /* Write the original frame flags */
            fwrite(flags, 1, 2, temp);

            /* Write the original encoding */
            fwrite(&encoding, 1, 1, temp);

            /* Write the original frame data */
            if (data_size > 0)
            {
                fwrite(frame_data, 1, data_size, temp);
            }
        }

        /* Free dynamically allocated frame data */
        free(frame_data);
        frame_data = NULL;
    }

    /* Store remaining MP3 audio data */
    unsigned char buffer[1024];

    /* Store number of bytes read */
    size_t bytes;

    /* Copy remaining audio data to temporary file */
    while ((bytes = fread(buffer, 1, sizeof(buffer), fptr)) > 0)
    {
        /* Write the audio data to temporary file */
        fwrite(buffer, 1, bytes, temp);
    }

    /* Check whether the selected frame was found */
    if (edited == 0)
    {
        /* Display frame not found error */
        printf(RED "ERROR! SELECTED FRAME NOT FOUND.\n" RESET);

        /* Close both files */
        fclose(fptr);
        fclose(temp);

        /* Delete temporary file */
        remove("temp.mp3");
        return;
    }

    /* Close the original MP3 file */
    fclose(fptr);

    /* Close the temporary MP3 file */
    fclose(temp);

    /* Remove the original MP3 file */
    if (remove(file_name) != 0)
    {
        /* Display error if original file cannot be removed */
        printf(RED "ERROR! OLD FILE COULD NOT BE REMOVED.\n" RESET);

        /* Delete temporary file */
        remove("temp.mp3");
        return;
    }

    /* Rename temporary file to original file name */
    if (rename("temp.mp3", file_name) != 0)
    {
        /* Display file replacement error */
        printf(RED "ERROR! FILE REPLACEMENT FAILED.\n" RESET);
        return;
    }

    /* Display final successful edit message */
    printf(GREEN "\n      SUCCESSFULLY EDITED %s.\n" RESET, field_name);
}