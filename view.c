#include <stdio.h>
#include <string.h>
#include "mp3.h"

void view_details(char *file_name)
{
    int i;
    FILE *fptr;

    unsigned int size;

    char id[5];
    char encoding;
    char frame_data[100];

    /* Checking whether the file is an MP3 file */
    char *extension = strrchr(file_name, '.');

    if (extension == NULL || strcmp(extension, ".mp3") != 0)
    {
        printf(RED "ERROR! INVALID FILE FORMAT.\n" RESET);
        return;
    }

    /* Open MP3 file */
    fptr = fopen(file_name, "rb");

    if (fptr == NULL)
    {
        printf(RED "ERROR! FILE NOT OPENED.\n" RESET);
        return;
    }

    /* Skip 10-byte ID3 header */
    if (fseek(fptr, 10, SEEK_SET) != 0)
    {
        printf(RED "ERROR! FAILED TO SKIP ID3 HEADER.\n" RESET);
        fclose(fptr);
        return;
    }

    /* Read six supported frames */
    for (i = 0; i < 6; i++)
    {
        /* Read frame ID */
        if (frame_id(fptr, id) == 0)
        {
            printf(RED "ERROR! FAILED TO READ FRAME ID.\n" RESET);
            fclose(fptr);
            return;
        }

        /* Read frame size */
        size = frame_size(fptr);

        if (size == 0)
        {
            printf(RED "ERROR! FAILED TO READ FRAME SIZE.\n" RESET);
            fclose(fptr);
            return;
        }

        /* Skip two-byte flags */
        if (fseek(fptr, 2, SEEK_CUR) != 0)
        {
            printf(RED "ERROR! FAILED TO READ FRAME FLAGS.\n" RESET);
            fclose(fptr);
            return;
        }

        /* Read encoding byte */
        if (frame_encode(fptr, &encoding) == 0)
        {
            printf(RED "ERROR! FAILED TO READ ENCODING BYTE.\n" RESET);
            fclose(fptr);
            return;
        }

        /* Read frame text. Only maximum 99 bytes are stored because frame_data has 100 bytes. */
        if (read_frame_data(fptr, &size, frame_data) == 0)
        {
            printf(RED "ERROR! FAILED TO READ FRAME DATA.\n" RESET);
            fclose(fptr);
            return;
        }

        /* Display frame data */
        display_view(id, frame_data);
    }

    fclose(fptr);
}

/* Read frame ID */
int frame_id(FILE *fptr, char *id)
{
    if (fread(id, 1, 4, fptr) != 4)
    {
        printf(RED "ERROR! FAILED TO READ FRAME ID.\n" RESET);
        return 0;
    }

    id[4] = '\0';
    return 1;
}


/* Read frame size */
unsigned int frame_size(FILE *fptr)
{
    unsigned char temp_size[4];
    unsigned int size;

    if (fread(temp_size, 1, 4, fptr) != 4)
    {
        printf(RED "ERROR! FAILED TO READ FRAME SIZE.\n" RESET);
        return 0;
    }

    /* Convert big-endian byte order */
    size = ((unsigned int)temp_size[0] << 24) |
           ((unsigned int)temp_size[1] << 16) |
           ((unsigned int)temp_size[2] << 8)  |
           ((unsigned int)temp_size[3]);
    return size;
}

/* Read encoding byte */
int frame_encode(FILE *fptr, char *encoding)
{
    if (fread(encoding, 1, 1, fptr) != 1)
    {
        printf(RED "ERROR! FAILED TO READ ENCODING BYTE.\n" RESET);
        return 0;
    }
    return 1;
}

/* Read frame text data */
int read_frame_data(FILE *fptr, unsigned int *frame_size, char *frame_data)
{
    unsigned int data_size;
    unsigned int display_size;
    unsigned int remaining_size;

    if (*frame_size < 1)
        return 0;

    /* Frame size includes encoding byte. */
    data_size = *frame_size - 1;

    /* Store maximum 99 characters. */
    if (data_size > 99)
        display_size = 99;
    else
        display_size = data_size;

    /* Read data that will be displayed */
    if (fread(frame_data, 1, display_size, fptr) != display_size)
    {
        printf(RED "ERROR! FAILED TO READ FRAME DATA.\n" RESET);
        return 0;
    }

    frame_data[display_size] = '\0';

    /* Skip remaining bytes of this frame.
     * This keeps the file pointer correctly positioned for the next frame.*/

    remaining_size = data_size - display_size;

    if (remaining_size > 0)
    {
        if (fseek(fptr, remaining_size, SEEK_CUR) != 0)
        {
            printf(RED "ERROR! FAILED TO SKIP REMAINING FRAME DATA.\n" RESET);
            return 0;
        }
    }
    return 1;
}


/* Display readable frame information */
void display_view(char *frame_id, char *frame_data)
{
    if (strcmp(frame_id, "TIT2") == 0)
    {
        printf(" Title   : %s\n", frame_data);
    }
    else if (strcmp(frame_id, "TPE1") == 0)
    {
        printf(" Artist  : %s\n", frame_data);
    }
    else if (strcmp(frame_id, "TALB") == 0)
    {
        printf(" Album   : %s\n", frame_data);
    }
    else if (strcmp(frame_id, "TYER") == 0)
    {
        printf(" Year    : %s\n", frame_data);
    }
    else if (strcmp(frame_id, "TCON") == 0)
    {
        printf(" Genre   : %s\n", frame_data);
    }
    else if (strcmp(frame_id, "COMM") == 0)
    {
        printf(" Comment : %s\n", frame_data);
    }
}