# Image Steganography in C

## Overview

This project implements **Image Steganography using the Least Significant Bit (LSB) technique** in C.
It allows users to hide a secret file inside a BMP image (encoding) and retrieve it later (decoding) using command-line arguments.


## Features

* Encode a secret file into a BMP image
* Decode the hidden file from a stego image
* Stores file extension and file size inside the image
* Preserves the original BMP header
* Simple command-line interface


## Technologies Used

* C Programming
* File Handling
* Bitwise Operations
* LSB Steganography Technique


## Project Files

* `main.c`
* `encode.c`
* `decode.c`
* `encode.h`
* `decode.h`
* `types.h`


## Compilation

Use GCC to compile the project:

```bash
gcc main.c encode.c decode.c -o stego
```


## Usage

### Encoding

```bash
./stego -e <source_image.bmp> <secret_file> <output_image.bmp>
```

Example:

```bash
./stego -e input.bmp secret.txt output.bmp
```


### Decoding

```bash
./stego -d <stego_image.bmp> <output_file>
```

Example:

```bash
./stego -d output.bmp extracted.txt
```


## How It Works

### Encoding

1. Validates input arguments
2. Checks image capacity
3. Encodes magic string, file extension, file size, and secret data
4. Copies remaining image data

### Decoding

1. Validates input arguments
2. Skips BMP header
3. Extracts file extension, file size, and secret data
4. Recreates the original file

