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
    	perror("fopen"); //If fopen is occuring if there's some error it directly prints the error.
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

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /*
        1. if(argv[2]==NULL)
            {
                Yes-> printf(".bmp file not passed\n");
                return e_failure
            }
    */
    if(argv[2] == NULL)
    {
        printf("Error: .bmp file not passed\n"); 
        return e_failure;
    }  
    /*
        2. if(strstr(argv[2],".bmp") == NULL)
            {
                yes-> printf("Invalid file name\n");
                return e_failure
            }
    */
    if(strstr(argv[2],".bmp") == NULL)
    {
        printf("Invalid image file name\n");
        return e_failure;
    }
    /*
        3. encInfo->src_image_fname = argv[2];//From here onwards we will store everything in structure and access from their.
    */  
    encInfo->src_image_fname = argv[2];
    /*
        4. check if(argv[3] == NULL)
            {
                yes-> printf(".txt file not passed\n");
                return e_failure
            }
    */
    if(argv[3] == NULL)
    {
        printf("Error: .txt file not passed\n");
        return e_failure;
    }
    /*
        5. check if(strstr(argv[3], ".txt") == NULL)
            {
                yes-> printf("Invalid secret file name\n");
                return e_failure
            }
    */
    if(strstr(argv[3], ".txt") == NULL)
    {
        printf("Invalid secret file name\n");
        return e_failure;
    }
    /*
        6. encInfo->secret_fname = argv[3]
    */
    encInfo->secret_fname = argv[3];
    /*
        7. check argv[4] == NULL
            yes-> encInfo->stego_image_fname = "stego.bmp"
            no-> Validate and store to encInfo->stego_image_fname = argv[4]
    */
    if (argv[4] == NULL)
        encInfo->stego_image_fname = "stego.bmp";
    else
        encInfo->stego_image_fname = argv[4];
    /*
            8. copy secret file extn 
    */
    char *extn = strchr(encInfo->secret_fname, '.');
    strcpy(encInfo->extn_secret_file, extn);

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    Status ret;
    /*
        1. ret = open_files(encInfo);
    */
    ret = open_files(encInfo);
    /*
        2. check ret == failure
        yes->print open file failed
                return failure
    */
    if(ret == e_failure)
    {
        printf("Error: Open files failed\n");
        return e_failure;
    }    

    /*
        3. ret = check_capacity(encInfo);
    */
    ret = check_capacity(encInfo);
    /*
        4. check (ret == failure)
            return failure
    */
    if(ret == e_failure)
    {
        printf("Insufficient capacity\n");
        return e_failure;
    }
    /*
        5.copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    */
    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);

    /*
        6. encode_magic_string(const char *magic_string, EncodeInfo *encInfo);
    */
    encode_magic_string(MAGIC_STRING, encInfo);
    
    /*
        7. encode_secret_file_extn_size(int file_extn_size, EncodeInfo *encInfo);
    */
    encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo);

    /*
        8. encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);
    */
    encode_secret_file_extn(encInfo->extn_secret_file, encInfo);

    /*
        9. cal sec_file_size --> make sure to set offset back to first byte
        9.encode_secret_file_size(sec_file_size, encInfo);
    */
    fseek(encInfo->fptr_secret, 0, SEEK_END);
    long sec_file_size = ftell(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);
    encInfo->size_secret_file = sec_file_size;
    encode_secret_file_size(encInfo->size_secret_file, encInfo);

    /*
        10.encode_secret_file_data(encInfo);
        
    */
    encode_secret_file_data(encInfo);

    /*
        11.copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);
        12.close 3 files
        13. return success
    */
    copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    /*
        1.calculate no. of bytes needed for encoding from src file
            count = (magic_str_len +
            extn_size(int) + 
            extn_len + 
            file_size(int) + 
            file_data_len ) * 8 + 54

        2. check count <= src_file_size
        yes-> return sucess
        no-> return failure
    */
    uint image_capacity;
    //uint required_capacity;
    long secret_file_size;
    int magic_str_len;
    int extn_len;

    image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    fseek(encInfo->fptr_secret,0,SEEK_END);
    secret_file_size = ftell(encInfo->fptr_secret);
    fseek(encInfo->fptr_secret,0,SEEK_SET);

    encInfo->size_secret_file = secret_file_size;
    magic_str_len = strlen(MAGIC_STRING);
    extn_len = strlen(encInfo->extn_secret_file);

    int count = (magic_str_len + sizeof(int) + extn_len + sizeof(int) + secret_file_size)*8+54;
    printf(".txtcount = %d\n",count);
    if(count<=image_capacity)
    {
        printf("Image has sufficient capacity for encode\n");
        return e_success;
    }
    else{
        return e_failure;
    }
    
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    /*
        char temp[55];
        fread(fptr_src_image, 54, 1, temp);

        fwrite(fptr_dest_image, 54, 1, temp);

        return success
    */

    char temp[54];
    rewind(fptr_src_image);
    fread(temp, 54, 1, fptr_src_image);
    fwrite(temp, 54, 1, fptr_dest_image);
    printf(".bmp header copied successfully\n");
    printf("%ld\n",ftell(fptr_src_image));
    return e_success;
}

void encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0; i<8; i++)
    {
        char mask = 1 << (7-i);
        char bit = data & mask;
        image_buffer[i] = image_buffer[i] & 0xFE;
        bit = bit >> (7-i);
        image_buffer[i] = image_buffer[i]|bit;
       
    }
    
}

void encode_size_to_lsb(int data, char *image_buffer)
{
    for(int i = 0; i < 32; i++)
    {
        char mask = 1 << (31 - i);
        char bit = data & mask;
        image_buffer[i] = image_buffer[i] & 0xFe;
        bit = bit >> (31-i);
        image_buffer[i] = image_buffer[i]|bit;
    }
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{

    /*
        1. for(int i=1;i<=2;i++)
        {
            char temp[8];
            1. read 8 bytes buffer from src file and store to temp array
            2. byte_to_lsb(magic_string[i], temp);
            3. write temp array 8 bytes to stego file
        }
    */
    char temp[8];
    for(int i = 0; i < strlen(magic_string); i++)
    {
        fread(temp, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], temp);
        fwrite(temp, 8, 1, encInfo->fptr_stego_image);
    }
    printf("Magic string: %ld\n",ftell(encInfo->fptr_src_image));
    return e_success;
}

Status encode_secret_file_extn_size(int file_extn_size, EncodeInfo *encInfo)
{
    /*
        char temp_buffer[32];
        1. read 32 bytes buffer from src file
        2. call size_to_lsb(file_extn_size, temp_buffer)
        3. write temp_buffer to stego file
    */
    char temp_buffer[32];

    fread(temp_buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_extn_size, temp_buffer);
    fwrite(temp_buffer, 32, 1, encInfo->fptr_stego_image);
    printf("extn_size: %ld\n",ftell(encInfo->fptr_src_image));
    return e_success;
}

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    /*
        1.run a loop 0 to file_extn_len-1 times
        {
            1.char temp[8];
            2.read 8 bytes of buffer from src file
            3.call byte_to_lsb(file_extn[i], temp);
            4. write 8 bytes temp to stego file
        }  
    */
    char temp[8];
    for(int i = 0; i < strlen(file_extn); i++)
    {
        fread(temp, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], temp);
        fwrite(temp, 8, 1, encInfo->fptr_stego_image);
    }
    printf("file_extn: %ld\n",ftell(encInfo->fptr_src_image));
    return e_success;
}

Status encode_secret_file_size(int sec_file_size, EncodeInfo *encInfo)
{
    /*
        1. char temp[32];
        2. read 32 bytes of buff from src file
        3. cal size_to_lsb(file_size, temp);
        4. write temp to stego file
    */
    char temp[32];
    fread(temp, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(sec_file_size, temp);
    fwrite(temp, 32, 1, encInfo->fptr_stego_image);
    printf("file_size: %ld\n", ftell(encInfo->fptr_src_image));
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    /*
        1. run a loop until src_file reaching EOF
        {
            1. read a ch from sec file
            2. read 8 bytes buff from src file
            3.call byte_to_lsb(ch, temp)
            4.write temp to stego file
        }
    */
    char ch;
    char buffer[8];
    while((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb((char)ch, buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    printf("data: %ld\n", ftell(encInfo->fptr_src_image));
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    /*
        1. run a loop until src file is reaching EOF
        {
            1. read a 1 byte from src file
            2.write 1 byte to dest file
        }
    */
    int ch;
    while ((ch = fgetc(fptr_src)) != EOF)
    {
        fputc(ch, fptr_dest);
        //fwrite(&ch, 1, 1, fptr_dest);
    }
    printf("Copying: %ld\n",ftell(fptr_src));
    return e_success;
}