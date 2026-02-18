#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

enum{
	MAYUS = 'a'-'A',
	MAX_KEY = 25,
	MIN_KEY = 0,
    NUM_LETTERS = 26,
};

int is_minus(char letter){
	return ((int)letter <= 'z') && ((int)letter >= 'a');
}

int is_mayus(char letter){
	return ((int)letter <= 'Z') && ((int)letter >= 'A');
}

char upper_char(char letter){
	letter -= MAYUS;
	return letter;
}

void upper_str(char *str){
	for (; *str != '\0'; str++) {
		if (is_minus(*str)){
			*str = upper_char(*str);
		}
	}
}

int key_ok(int key){
	return (key >= MIN_KEY) && (key <= MAX_KEY);
}

void encrypt(int key, char *text){
	for(; *text != '\0'; text++){
		if (is_mayus(*text)){
			*text += key;
			if (*text > 'Z'){
				*text -= NUM_LETTERS;
			}
		}
	}
}

int main(int argc, char *argv[]){
	int key;
	char *text;

	if (argc != 3){
		errx(EXIT_FAILURE, "Malos argumentos");
	}
	
	key = (int)strtol(argv[1], NULL, 10);
	if (!key_ok(key)){
		errx(EXIT_FAILURE, "Key inválida");
	}
	text = strdup(argv[2]);
	upper_str(text);
	encrypt(key, text);
    printf("%s \n", text);
	free(text);
	exit(EXIT_SUCCESS);
}




