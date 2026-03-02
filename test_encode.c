#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    int ret = check_operation_type(argv);

    if(ret == e_unsupported)
    {
        printf("Error: INVALID ARGUMENTS\n");
        return 0;
    }

    if(ret == e_encode)
    {
        //encoding
        int ret = read_and_validate_encode_args(argv, &encInfo); // returns success or failure
        if(ret == e_failure)
        {
            printf("INVALID ARGUMENTS\n");
            return 0;
        }
        //start encoding
        ret = do_encoding(&encInfo);
        if(ret == e_failure)
        {  
            printf("Encoding Failed\n");
            return 0;
        }
        else
        {
            printf("Encoding is successfull.....");
            printf("\n");
            return 0;
        }
        
        
    }

    if(ret == e_decode)
    {
        /*
            1. call read_and_validate_encode_args(argv, &decInfo)
            2. do_decoding(&decInfo);
        */
        int ret = read_and_validate_decode_args(argv, &decInfo);
        if(ret == e_failure)
        {
            printf("INVALID ARGUMENTS\n");
            return 0;
        }
        //start encoding
        ret = do_decoding(&decInfo);
        if(ret == e_failure)
        {  
            printf("Decoding Failed\n");
            return 0;
        }
        else
        {
            printf("Decoding is successfull.....");
            printf("\n");
            return 0;
        } 
         return e_success;
    }
}

OperationType check_operation_type(char *argv[])
{
    /*
        1. check argv[1] == NULL
        yes-> return e_unsupported
    */
     if (argv[1] == NULL)
    {
        return e_unsupported;
    }

    /*
        2. if(strcmp(argv[1],"-e")==0)
        yes-> return e_encode
    */
    if (strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    /*
        3. if(strcmp(argv[1],"-d")==0)
        yes-> return e_decode
    */
    if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    /*
        4. return e_unsupported 
    */
    else
    {
        return e_unsupported;
    }
}
    
