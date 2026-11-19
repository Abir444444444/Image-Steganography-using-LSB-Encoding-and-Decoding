#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

int main(int argc,char **argv){
     // Ensure minimum required arguments are provided
    if(argc>=3){
        if(check_operation_type(argv)==e_encode){
            printf("Selected encoding \n");
            EncodeInfo encoding;
            // Validate encoding arguments
            if (read_and_validate_encode_args(argv,&encoding)==e_success){
                printf("Validated the encoding arguments successfully \n");
                printf("-------------started encoding--------------\n");
                //perform encoding
                if(do_encoding(&encoding)==e_success){
                    printf("Completed encoding successfully\n");
                }
                else{
                    printf("Failed to complete encoding\n");
                    return 1;
                }
                
            }
            else{
                printf("Failed to validate the encoding arguments \n");
                return 1;
            }
    } 
        // Validate decoding arguments
        else if(check_operation_type(argv)==e_decode){
            printf("Selected decoding \n");
            DecodeInfo decoding;
            if (read_and_validate_decode_args(argv,&decoding)==e_success){
                printf("Validated the decoding arguments successfully \n");
                printf("-------------started decoding--------------\n");
                //perform decoding
                if(do_decoding(&decoding)==e_success){
                    printf("Completed decoding successfully\n");
                }
                else{
                    printf("Failed to complete decoding\n");
                    return 1;
                }
                
            }
            else{
                printf("Failed to validate the decoding arguments \n");
                return 1;
            }
        }
        else{
            printf("Invalid argument \n");
            printf("For encoding: ./a.out -e beautiful.bmp secret.txt [stego.bmp] \n");
            printf("For decoding: ./a.out -d stego.bmp [decode.txt] \n");
        }
    }

    else{
        printf("Pass the sufficient arguments in CL\n");
        return 1;
    }
    return 0;
}

/*
 * Determine requested operation
 * -e : Encoding
 * -d : Decoding
 */
OperationType check_operation_type(char *argv[]){
    if(strcmp(argv[1],"-e")==0){
       return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0){
         return e_decode;
    }
    else{
        return e_unsupported;
    }
}
