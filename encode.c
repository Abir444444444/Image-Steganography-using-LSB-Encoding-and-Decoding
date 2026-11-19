#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/*
 * Validate command-line arguments
 * Format: ./a.out -e image.bmp secret.txt [stego.bmp]
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo){
    if(strcmp(strchr(argv[2],'.'),".bmp")==0){
        encInfo->src_image_fname=argv[2];
    }
    else{
        return e_failure;
    }

    if(strcmp(strchr(argv[3],'.'),".txt")==0){
        encInfo->secret_fname=argv[3];
    }
    else{
        return e_failure;
    }
    // Optional output file
    if(argv[4]!=NULL){
        encInfo->stego_image_fname=argv[4];
    }
    else{
        encInfo->stego_image_fname="stego.bmp";
    }
    return e_success;
}

/*
 * Get size of file (secret file)
 */
uint get_file_size(FILE *fptr){
    fseek(fptr,0,SEEK_END);
   unsigned int size = ftell(fptr);

    printf("File size = %u bytes\n", size);

    return size;
}

/*
 * Check if image has enough capacity
 * Required space:
 * metadata + secret file size * 8 bits
 */
Status check_capacity(EncodeInfo *encInfo){
    //get the size of beautiful.bmp
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    //get the size of secret.txt
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    //compare size of beautiful.bmp>required size
    if(encInfo->image_capacity >(2+4+4+4+(encInfo->size_secret_file*8)))
      return e_success;
    else
       return e_failure;

}

/*
 * Copy first 54 bytes (BMP header)
 * Must be unchanged in stego image
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){
    char str[54];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(str,54,1,fptr_src_image);
    fwrite(str,54,1,fptr_dest_image);
    return e_success;
}

/*
 * Encode data bytes into image using LSB technique.
 * Each character is encoded into 8 image bytes.
 */
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo){
    for(int i=0;i<size;i++){
        fread(encInfo->image_data,8,1,encInfo->fptr_src_image);
         encode_byte_to_lsb(data[i],encInfo->image_data);
        fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
    }
    return e_success;

}

/*
 * Encode 1 byte into 8 image bytes using LSB
 */
Status encode_byte_to_lsb(char data, char *image_buffer){
    for(int i=0;i<8;i++){
    image_buffer[i]=(image_buffer[i]&0xFE)|((data>>(7-i))&1);
    }
    return e_success;
}

/*
 * Encode magic string into image
*/
Status encode_magic_string( char *magic_string, EncodeInfo *encInfo){
    encode_data_to_image(magic_string,2,encInfo);
    return e_success;
}

/*
 * Encode secret file extension size
 * into 32 image bytes using LSB.
 */
Status encode_secret_file_extn_size( int size, EncodeInfo *encInfo){
    encode_size_to_lsb(size,encInfo);
    return e_success;
}

/*
 * Encode a 32-bit integer into 32 image bytes using LSB
*/
Status encode_size_to_lsb(int size, EncodeInfo *encInfo){
    char str[32];
    fread(str,32,1,encInfo->fptr_src_image);
    for(int i=0;i<32;i++){
        str[i]=(str[i]&0xFE)|((size>>(31-i))&1);
    }
    fwrite(str,32,1,encInfo->fptr_stego_image);
    return e_success;

}
/*
 * Encode secret file extension
 * (e.g. ".txt") into image using LSB.
 */
Status encode_secret_file_extn( char *file_extn, EncodeInfo *encInfo){
    encode_data_to_image(file_extn,4, encInfo);
    return e_success;
}

/*
 * Encode secret file size
 * into 32 image bytes using LSB.
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){
    encode_size_to_lsb(file_size,encInfo);
    return e_success;
}

/*
 * Encode complete secret file contents.
 * Read entire secret file into memory
 * and encode it byte-by-byte into image.
 */
Status encode_secret_file_data(EncodeInfo *encInfo){
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    char str[encInfo->size_secret_file];
    fread(str,encInfo->size_secret_file,1,encInfo->fptr_secret);
     encode_data_to_image(str,encInfo->size_secret_file,encInfo);
     return e_success;
}

/*
 * Copy remaining image bytes after all
 * encoding operations are completed.
 * This preserves the rest of the BMP image.
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest){
    char ch;
    while(fread(&ch,1,1,fptr_src)==1){
        fwrite(&ch,1,1,fptr_dest);
        if(feof(fptr_src))
           break;
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo){
    //open the required files in required mode
    if(open_files(encInfo)==e_success)
        printf("Opened the required files in required mode successfully \n");
    else{
        printf("Failed to open files in required mode \n");
        return e_failure;
    }
    //check if beautiful.bmp is suitable for encoding
    if(check_capacity(encInfo)==e_success)
        printf("beautiful.bmp is suitable for encoding\n");
    else{
        printf("beautiful.bmp is not suitable for encoding\n");
        return e_failure;
    }
    //copy 54 bytes of header from beautiful.bmp to secret.txt
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
        printf("Copied header from beautiful.bmp to secret.txt successfully \n");
    else{
        printf("Failed to copied header from beautiful.bmp to secret.txt\n");
        return e_failure;
    }
    //encode magic string
    if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
       printf("Encoded magic string successfully\n");
    else{
        printf("Failed to encode magic string\n");
        return e_failure;
    }
    //encode secret file extension size
    strcpy(encInfo->extn_secret_file,strchr(encInfo->secret_fname,'.'));
     printf("file extension is %s\n",encInfo->extn_secret_file);
    if(encode_secret_file_extn_size(4, encInfo)==e_success){
        printf("Encoded secret file extension size successfully\n");
    }
    else{
        printf("Failed to encoded secret file extension size\n");
        return e_failure;
    }
    //encode secret file extention
    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
        printf("Encoded secret file extension successfully\n");
    else{
        printf("Failed to encode secret file extension\n");
        return e_failure;
    }
    //encode secret file size
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
        printf("Encoded secret file size successfully\n");
    else{
        printf("Failed to encode secret file size\n");
        return e_failure;
    }
    //encode secret file data
    if(encode_secret_file_data(encInfo)==e_success)
        printf("Encoded secret file data successfully\n");
    else{
        printf("Failed to encode secret file data\n");
        return e_failure;
    }
    //copy remaining data from beautiful.bmp to stego.bmp
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
        printf("Copied remaining data successfully\n");
    else{
        printf("Failed to Copied remaining data\n");
        return e_failure;
    }

    return e_success;
}