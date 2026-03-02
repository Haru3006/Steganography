#include"decode.h"
#include<stdio.h>
#include "types.h"
#include <string.h>

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego = fopen(decInfo->stego_fname, "r");
    if (decInfo->fptr_stego == NULL)
    {
    	perror("fopen"); //If fopen is occuring if there's some error it directly prints the error.
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_fname);

    	return e_failure;
    }
    return e_success;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    /*
        1. check argv[2] is having .bmp extension
        Yes-> store argv[2] to decInfo (stego_fname)

        2. if argv[3] is NULL
        yes-> store default name for output file to decInfo

        3. check argv[3] is not having '.'
        yes-> store argv[3] to decInfo.
        no-> print Invalid
    */
    if(argv[2] == NULL)
    {
        printf(".bmp file is not passed\n");
        return e_failure;
    }
    if((strstr(argv[2],".bmp"))==NULL)
    {
        printf("Invalid image file name\n");
        return e_failure;
    }
    decInfo->stego_fname = argv[2];

    if(argv[3] == NULL)
    {
       printf("Created default output file\n"); 
       strcpy(decInfo->dest_fname, "output"); 
    }
    else
    {
        if(strchr(argv[3], '.') != NULL)
        {
            printf("Invalid output file name\n");
            return e_failure;
        }
        else
        {
            strcpy(decInfo->dest_fname, argv[3]); 
        }
    } 
    return e_success; 
}

Status do_decoding(DecodeInfo *decInfo)
{
    Status ret;
    // 1. open stego file 
    ret = open_decode_files(decInfo);

    if(ret == e_failure)
    {
        printf("Error: Open files failed\n");
        return e_failure;
    }  

    // 2. call skip_bmp_header(decInfo->fptr_stego)
    skip_bmp_header(decInfo->fptr_stego);

    // 3. char magic_string[50];
    char magic_string[50];
    // 4. decode_magic_string(decInfo->fptr_stego, magic_string);
    decode_magic_string(decInfo->fptr_stego, magic_string);
    
    // 5. Read magic_string from user as well
    char user_magic_string[50];
    printf("Enter the magic string for decoding: ");
    scanf("%s", user_magic_string);

    /* 6. compare user entered and decode magic strings are same?
            no-> err:invalid magic string
                        return failure    */
    if(strcmp(user_magic_string, magic_string)!=0)
    {
        printf("Error: Invalid magic string\n");
        return e_failure;
    }

    //  7. Declare int ext_size;
    int ext_size;

    //  8. decode_extn_size(decInfo->fptr_stego, &extn_size);
    decode_extn_size(decInfo->fptr_stego, &ext_size);
    printf("%d\n",ext_size);
    
    //  9. char extn[10];
    char extn[10];
    //  10. deocde_extn(decInfo->fptr_stego, extn, extn_size);
    decode_extn(decInfo->fptr_stego, extn, ext_size);

    //  11. Concatenate/merge output file name with decoded extn
    strcat(decInfo->dest_fname, extn);
    printf("%s\n", decInfo->dest_fname);

    //  12. open output file
    decInfo->fptr_dest = fopen(decInfo->dest_fname, "w");
    if (decInfo->fptr_dest == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->dest_fname);
    	return e_failure;
    }
    /*  13. int file_size;
        13. decode_sec_file_size(decInfo->fptr_stego, &file_size);
    */
    int file_size;
    decode_file_size(decInfo->fptr_stego, &file_size);
    printf("%d\n",file_size);

    /*  14.decode_sec_data(decInfo->fptr_stego,decInfo->fptr_dest,file_size);
        15.close files
        16.return success */
    decode_sec_data(decInfo->fptr_stego, decInfo->fptr_dest, file_size);
    fclose(decInfo->fptr_stego);
    return e_success;
}

void skip_bmp_header(FILE *fptr_stego)
{
    fseek(fptr_stego, 54, SEEK_SET);
    printf("Skip: %ld",ftell(fptr_stego));
}

char lsb_to_byte(char *buffer)  //buffer-8 bytes
{
    /*
        run a loop for 8 times'
            get lsb of each index of buffer and store th the ith pos of data var

        return data
    */
    char ch = 0;
    for(int i=0; i<8; i++)
    {
        ch = ((ch << 1) | (buffer[i] & 1));
    }
    return ch;
}

int lsb_to_size(char *buffer)  //buffer-32 bytes
{
    int size = 0;
    for(int i=0; i<32; i++)
    {
        size = ((size << 1) | (buffer[i] & 1));
    }
    return size;
}

Status decode_magic_string(FILE *stego, char *magic_string)
{
    /*
        for(int i=0;i<2;i++)
        {
            1. read 8 bytes buffer from stego file
                char temp[8];
            2. magic_string[i] = lsb_to_byte(temp)
        }
    */
    char temp[8];
    for(int i=0;i<2;i++)
    {
        fread(temp,8,1,stego);
        magic_string[i] = lsb_to_byte(temp);
    }
    magic_string[2] = '\0';
    printf("Decoded magic string: %s\n", magic_string);
    return e_success;
}

Status decode_extn_size(FILE *stego, int *ext_size)
{
    /*
        1. read 32 bytes buffer
        2. *extn_size = lsb_to_size(temp);
        3. return e_success;

    */
    char temp[32];
    fread(temp,32,1,stego);
    *ext_size = lsb_to_size(temp);
    return e_success;
}

Status decode_extn(FILE * stego, char *ext, int ext_size)
{
    /*
        run a look for 0 to extn_size-1
        {
        1. read 8 byte temp buffer
        2. extn[i]=lsb_to_byte(temp);
        }
    */
    char temp[8];
    for(int i=0; i<ext_size; i++)
    {
        fread(temp,8,1,stego);
        ext[i] = lsb_to_byte(temp);
    }
    ext[ext_size] = '\0';
    printf("Decoded extension string: %s\n", ext);
    return e_success;
}

Status decode_file_size(FILE *stego, int *file_size)
{
    /*
        1. read 32 bytes buffer
        2. *file_size = lsb_to_size(temp);
    */
    char temp[32];
    fread(temp,32,1,stego);
    *file_size = lsb_to_size(temp);
    return e_success; 
}

Status decode_sec_data(FILE *stego, FILE *fptr_dest, int file_size)
{
    /*
        run a lop for file_size times
        {
            1. read 8 bytes buff
            2  . char ch = lsb_to_byte(temp)
            3  . write ch to dest file
        }
    */
    char temp[8];
    for (int i = 0; i < file_size; i++)
    {
        fread(temp, 8, 1, stego);
        char ch = lsb_to_byte(temp);
        fputc(ch, fptr_dest);
    }
    return e_success;
}
