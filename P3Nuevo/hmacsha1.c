#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>


//Prnit hexadecimal
void print_hex(const unsigned char *data, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    printf("%02x", data[i]);
  }
}

//File size
long get_file_size(FILE *file) {
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  rewind(file);
  return size;
}


//Read data from file
void read_data_from_file(const char *filename, unsigned char *buffer, long length) {
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
  fread(buffer, 1, length, file);
  fclose(file);
}

//Creat a HMAC key
EVP_PKEY *create_hmac_key(const char *key_file) {
  FILE *key_fp = fopen(key_file, "rb");
  if (key_fp == NULL) {
    perror("Error opening key file");
    exit(EXIT_FAILURE);
  }

  long key_size = get_file_size(key_fp);
  if (key_size > EVP_MAX_KEY_LENGTH) {
    fprintf(stderr, "Error: Key is too long\n");
    exit(EXIT_FAILURE);
  }
  
  // Read the key from key file
  unsigned char key[EVP_MAX_KEY_LENGTH];
  read_data_from_file(key_file, key, key_size);

  fclose(key_fp);

  return EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, key, key_size);
}

//Calculate HMAC
void calculate_hmac(const char *data_file, const char *key_file) {
  FILE *data_fp = fopen(data_file, "rb");
  if (data_fp == NULL) {
    perror("Error opening data file");
    exit(EXIT_FAILURE);
  }

  EVP_PKEY *pkey = create_hmac_key(key_file);
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();


  if (ctx == NULL) {
    perror("Error creating EVP_MD_CTX");
    exit(EXIT_FAILURE);
  }

  const EVP_MD *md = EVP_sha1();
  EVP_DigestSignInit(ctx, NULL, md, NULL, pkey);
  unsigned char buffer[1024];
  size_t read_bytes;


  while ((read_bytes = fread(buffer, 1, sizeof(buffer), data_fp)) > 0) {
    EVP_DigestSignUpdate(ctx, buffer, read_bytes);
  }

  fclose(data_fp);

  unsigned char result[EVP_MAX_MD_SIZE];
  size_t result_size = sizeof(result);

  if (EVP_DigestSignFinal(ctx, result, &result_size) != 1) {
    perror("Error calculating HMAC");
    exit(EXIT_FAILURE);
  }

  EVP_MD_CTX_free(ctx);
  EVP_PKEY_free(pkey);

  //Case: Key to short  
  if (get_file_size(fopen(key_file, "rb")) < EVP_MD_size(md)) {
    printf("warning: key is too short (should be longer than 20 bytes)\n", EVP_MD_size(md));
  }
  
  //Print result
  print_hex(result, result_size);
  printf("\n");
}


int main(int argc, char *argv[]) {
  //Check arguments  
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <data_file> <key_file>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  //Program
  calculate_hmac(argv[1], argv[2]);

}


