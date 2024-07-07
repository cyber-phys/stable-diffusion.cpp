#include "kitty_image.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

// Constants
#define CHUNK_SIZE 4096

// Function to read a file into a buffer
unsigned char* read_file(const char* filename, size_t* size_out) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    unsigned char* buffer = (unsigned char*)malloc(size);
    if (!buffer) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    fclose(file);

    *size_out = size;
    return buffer;
}

// Function to encode a buffer to base64
char* base64_encode(const unsigned char* data, size_t input_length, size_t* output_length) {
    static const char encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char* encoded_data = (char*)malloc((input_length + 2) / 3 * 4 + 1);  // +1 for null terminator
    if (!encoded_data) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    char* ptr = encoded_data;
    for (size_t i = 0; i < input_length;) {
        unsigned int octet_a = i < input_length ? data[i++] : 0;
        unsigned int octet_b = i < input_length ? data[i++] : 0;
        unsigned int octet_c = i < input_length ? data[i++] : 0;

        unsigned int triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        *ptr++ = encoding_table[(triple >> 3 * 6) & 0x3F];
        *ptr++ = encoding_table[(triple >> 2 * 6) & 0x3F];
        *ptr++ = encoding_table[(triple >> 1 * 6) & 0x3F];
        *ptr++ = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    if ((input_length % 3) > 0) {
        *(ptr - 1) = '=';
        if ((input_length % 3) == 1)
            *(ptr - 2) = '=';
    }

    *ptr = '\0';  // null terminator
    *output_length = (size_t)(ptr - encoded_data);
    return encoded_data;
}

// Function to transmit the PNG image data to the kitty terminal in chunks
void transmit_image(const char* base64_data, size_t length) {
    size_t pos = 0;
    while (pos < length) {
        size_t chunk_size = (length - pos > CHUNK_SIZE) ? CHUNK_SIZE : (length - pos);
        printf("\033_Gm=%i;", (pos + chunk_size < length) ? 1 : 0);
        fwrite(base64_data + pos, 1, chunk_size, stdout);
        fputs("\033\\", stdout);
        pos += chunk_size;
    }
}

// Function to display the image in the terminal using kitty protocol
void display_image_with_kitty_protocol(const std::string& file_path, unsigned int width, unsigned int height) {
    size_t file_size;
    unsigned char* file_data = read_file(file_path.c_str(), &file_size);
    if (!file_data) {
        return;
    }

    size_t encoded_size;
    char* base64_data = base64_encode(file_data, file_size, &encoded_size);
    free(file_data);

    if (!base64_data) {
        return;
    }

    printf("\033_Ga=T,f=100,s=%u,v=%u,m=1;\033\\", width, height);
    transmit_image(base64_data, encoded_size);
    putchar('\n');

    free(base64_data);
}
