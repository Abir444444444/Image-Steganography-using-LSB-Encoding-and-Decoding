#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/*
 * Open required files:
 * - stego image (input)
 * - secret output file (decoded data)
 */
Status open_defiles(DecodeInfo *decInfo){
    // Secret file
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    // Do Error handling
    if (decInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }
    // No failure return e_success
    return e_success;
}

/*
 * Validate command line arguments for decoding:
 * ./a.out -d stego.bmp [decode.txt]
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo){
    if(strcmp(strchr(argv[2],'.'),".bmp")==0){
        decInfo->stego_image_fname=argv[2];
    }
    else{
        return e_failure;
    }

    // default output file
    if(argv[3]!=NULL){
        decInfo->secret_fname=argv[3];
    }
    else{
        decInfo->secret_fname="decode.txt";
    }
    return e_success;

}

/* 
* Decode and verify MAGIC STRING from image 
 */
Status decode_magic_string( char *magic_string, DecodeInfo *decInfo){
    if(decode_image_to_data(2,decInfo)==e_success){
    // Compare with expected magic string
       if(strcmp(magic_string,decInfo->secret_data)==0){
           return e_success;
        }
    else{
        return e_failure;
    }
   }
}

/* 
* Decode characters from image LSBs
*/
Status decode_image_to_data(int size, DecodeInfo *decInfo){
    char str[8];
    
    for(int i=0;i<size;i++){
        // Read 8 bytes from image
        fread(str,8,1,decInfo->fptr_stego_image);
         // Convert LSBs to character
        decInfo->secret_data[i]=decode_lsb_to_byte(str);
    }
    // Null terminate string
    decInfo->secret_data[size] = '\0';
    return e_success;

}


//  * Convert 8 image bytes to 1 character using LSB 
char decode_lsb_to_byte(char *image_buffer){
    char ch =0;
    for(int i=0;i<8;i++){
        ch=ch|((image_buffer[i]&1)<<(7-i));
    
    }
    return ch;
}

/*
* Decode secret file extension (.txt)
*/ 
Status decode_secret_file_extn(int size,DecodeInfo *decInfo){
    decode_image_to_data(size, decInfo);
    return e_success;
}

/* 
* Decode secret file extension size (32 bits from LSB)
*/
int decode_secret_file_extn_size( DecodeInfo *decInfo){
    return decode_lsb_to_size(decInfo);
}

/* 
* Convert 32 LSB bits to an integer
*/
int decode_lsb_to_size(DecodeInfo *decInfo){
    char str[32];
    int a=0;
    // Read 32 bytes from image
    fread(str,32,1,decInfo->fptr_stego_image);
    // Convert bits to integer
    for(int i = 0; i < 32; i++){
    a = a | ((str[i] & 1) << (31-i));
    }
    return a;
}

/* 
* Decode secret file size
*/
int decode_secret_file_size( DecodeInfo *decInfo){
    return decode_lsb_to_size(decInfo);
}

/* 
* Decode actual secret message and write to file decode.txt
*/
Status decode_secret_file_data(int size,DecodeInfo *decInfo){
    char str[8];
    char ch;
    for(int i=0;i<size;i++){
        fread(str,8,1,decInfo->fptr_stego_image);
        ch=decode_lsb_to_byte(str);
        // Write character to output file
        fwrite(&ch,1,1,decInfo->fptr_secret);
    }
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo){
    //open the required files in required mode
    int size;
    if(open_defiles(decInfo)==e_success)
        printf("Opened the required files in required mode successfully \n");
    else{
        printf("Failed to open files in required mode \n");
        return e_failure;
    }
    //Skip BMP header (54 bytes)
    if(fseek(decInfo->fptr_stego_image,54,SEEK_SET)){
        printf("Failed to move file pointer\n");
        return e_failure;
    }
    else{
    printf("File pointer moved successfully\n");
    }

    //decode magic string
    if(decode_magic_string(MAGIC_STRING, decInfo)==e_success)
       printf("Decoded magic string successfully\n");
    else{
        printf("Failed to decode magic string\n");
        return e_failure;
    }
    //decoding secret file extension size
    size = decode_secret_file_extn_size(decInfo);
    printf("Decoded file extension size = %d\n", size);
    if(size>0){
        printf("Decoded secret file extension size successfully\n");
    }
    else{
        printf("Failed to decode secret file extension size\n");
        return e_failure;
    }
    //decode secret file extension
    if(decode_secret_file_extn(size,decInfo)==e_success)
        printf("Decoded secret file extension successfully\n");
    else{
        printf("Failed to decode secret file extension\n");
        return e_failure;
    }
    //decode secret file size
    size=decode_secret_file_size(decInfo);
    printf("Decoded secret file size = %d\n", size);
    if(size>0)
        printf("decoded secret file size successfully\n");
    else{
        printf("Failed to decode secret file size\n");
        return e_failure;
    }
    //decode secret file data
    if(decode_secret_file_data(size,decInfo)==e_success)
        printf("Decoded secret file data successfully\n");
    else{
        printf("Failed to decode secret file data\n");
        return e_failure;
    }
    return e_success;
}
