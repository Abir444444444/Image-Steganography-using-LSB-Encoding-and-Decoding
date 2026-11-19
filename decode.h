#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/*
 * Structure used for decoding stego image
 * Stores file pointers, file names, and extracted data
 */
 
#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _decodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output secret file */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

} DecodeInfo;


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Open required files */
Status open_defiles(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string( char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extention */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo);

/* Decode secret file extention size*/
int decode_secret_file_extn_size( DecodeInfo *decInfo);

/* Decode secret file size */
int decode_secret_file_size( DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(int size,DecodeInfo *decInfo);

/* Convert image LSBs to characters */
Status decode_image_to_data( int size,DecodeInfo *decInfo);

/* Convert 8 bytes LSBs to 1 character */
char decode_lsb_to_byte(char *image_buffer);

/* Convert 32 bytes LSBs to 1 integer  */
int decode_lsb_to_size(DecodeInfo *decInfo);


#endif
