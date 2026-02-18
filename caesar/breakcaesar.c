#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <math.h>

enum {
	MAX_KEY = 26,
	NUM_LETTERS = 26,
	NUM_BIGRAMS = 42,
	NUM_TRIGRAMS = 28,
	MAYUS_DIF = 'a' - 'A',
};

//.. This struct contains every metada we need to estimate 
//.. who is the correct message
typedef struct Dec_msg {	//.. Message decrypted. D(E(M)) = Mmsg
	int key;
	char *msg;
	int topbigrams;
	int toptrigrams;
	int aplicant;
	float dist;
} Dec_msg;

//.. This procedure frees the memory allocated by malloc
void
freeMemory(Dec_msg * msgs[])
{
	int i;

	for (i = 0; i < MAX_KEY; i++) {
		if (msgs[i]) {
			if (msgs[i]->msg) {
				free(msgs[i]->msg);
			}
			free(msgs[i]);
		}
	}
}

//.. This function returns True if the parsed char is a minus letter
int
isMinus(char letter)
{
	return ((int)letter <= 'z') && ((int)letter >= 'a');
}

//.. This function returns True if the parsed char is a mayus letter
int
isMayus(char letter)
{
	return ((int)letter <= 'Z') && ((int)letter >= 'A');
}

//.. This function convert the parsed char into mayus
char
upperChar(char letter)
{
	return (letter -= MAYUS_DIF);
}

//.. This procedure set the parsed string into one line changing minus letters
//.. to mayus letters and 'tabs' and '\n' to spaces
void
upper2oneLine(char *str)
{
	for (; *str != '\0'; str++) {
		if (isMinus(*str)) {
			*str = upperChar(*str);
		} else if ((*str == '\t') || (*str == '\n')) {
			*str = ' ';
		}
	}
}

//.. This procedure set/initialise the list of struct Dec_msg
//.. by adding the key, the decrypted message and some useful metadata
void
initDecryptedMsgs(char *text, Dec_msg * msgs[])
{
	int i;

	for (i = 0; i < MAX_KEY; i++) {
		msgs[i] = (Dec_msg *) malloc(sizeof(Dec_msg));
		if (msgs[i] == NULL) {
			free(text);
			freeMemory(msgs);
			err(EXIT_FAILURE, "Cannot Malloc");
		}
		msgs[i]->key = 0;
		msgs[i]->topbigrams = 0;
		msgs[i]->toptrigrams = 0;
		msgs[i]->aplicant = 0;
		msgs[i]->dist = 0.0;
	}
}

//.. This function decrypt an encrypted caesar chiper message 
//.. based on the key parsed
char
*
decrypt(char *text, int key)
{
	char *decrypted;
	int i;

	decrypted = strdup(text);
	if (decrypted == NULL) {
		return NULL;
	}
	for (i = 0; i < strlen(decrypted); i++) {
		if (isMayus(decrypted[i])) {
			decrypted[i] -= key;
			if (decrypted[i] < 'A') {
				decrypted[i] += NUM_LETTERS;
			}
		}
	}
	return decrypted;
}

//.. Procedure witch set the decrypted msg and the key to the struct Dec_msg
void
setDecryptedMsg(char *text, Dec_msg * msgs[], int key)
{
	msgs[key]->key = key;
	msgs[key]->msg = decrypt(text, key);
	if (msgs[key]->msg == NULL) {
		free(text);
		freeMemory(msgs);
		err(EXIT_FAILURE, "Cannot malloc");
	}
}

//.. This function calculate the mean of apartion of a letter parsed
//.. in the message parsed
float
meanLetter(char *msg, char letter)
{
	int i, total_letters, count;

	count = 0;
	total_letters = 0;
	for (i = 0; i < (int)strlen(msg); i++) {
		if (isMayus(msg[i])) {
			total_letters++;
			if (msg[i] == letter) {
				count++;
			}
		}
	}
	return ((float)count / (float)total_letters) * 100.0;
}

//.. This function calculate the manhattan distance bettwen the relative
//.. frecuency(fr) of the letters according to wikipedia and the relative 
//.. frecuency of the letters parsed in fi
float
manhattanDist(float fr[], float fi[])
{
	float dist;
	int i;

	dist = 0.0;
	for (i = 0; i < NUM_LETTERS; i++) {
		dist += (fabs(fr[i] - fi[i])) / (float)NUM_LETTERS;
	}
	return dist;
}

//.. procedure witch set the manhattan distance in Dec_msg of the msg considered
//.. between the global english letter frequencies and the letter freqs of
//.. the message
void
setDistance(Dec_msg * msg, float englishfr[], char *alphabet)
{
	float letterfr[NUM_LETTERS];
	int i;

	memset(letterfr, 0.0, sizeof(float) * NUM_LETTERS);
	for (i = 0; i < NUM_LETTERS; i++) {
		letterfr[i] = meanLetter(msg->msg, alphabet[i]);
	}
	msg->dist = manhattanDist(englishfr, letterfr);
}

//.. Procedure witch set the aplicant by the frecuency analisys
void
setLetterAplicant(Dec_msg * msgs[])
{
	int i, aplicant;
	float smallest;

	aplicant = 0;
	smallest = msgs[0]->dist;
	for (i = 0; i < MAX_KEY; i++) {
		if (msgs[i]->dist < smallest) {
			smallest = msgs[i]->dist;
			aplicant = i;
		}
	}
	msgs[aplicant]->aplicant = 1;
}


//.. This function returns true if the 2 letters conform a bigram 
//.. iqual to the one parsed as *bigram
int
isBigramValid(char letter1, char letter2, char *bigram)
{
	return (letter1 == bigram[0]) && (letter2 == bigram[1]);
}

//.. Procedure witch set the number of top bigrams contained in the message msg
void
setTopBigrams(Dec_msg * msg, char *bigrams[])
{
	int i, j;

	for (i = 0; i < NUM_BIGRAMS; i++) {
		for (j = 1; j < strlen(msg->msg); j++) {
			if (isBigramValid
			    (msg->msg[j - 1], msg->msg[j], bigrams[i])) {
				msg->topbigrams++;
			}
		}
	}
}

//.. Procedure witch set the aplicant by searching the higher topBigrams number
//.. in the list of struct Dec_msg
void
setBigramAplicant(Dec_msg * msgs[])
{
	int i, higher, aplicant;

	aplicant = 0;
	higher = msgs[0]->topbigrams;
	for (i = 0; i < MAX_KEY; i++) {
		if (msgs[i]->topbigrams > higher) {
			higher = msgs[i]->topbigrams;
			aplicant = i;
		}
	}
	msgs[aplicant]->aplicant = 1;
}

//.This function returns true if the 3 letters parsed conform the trigram parsed
int
trigramValid(char char1, char char2, char char3, char *trigram)
{
	return (char1 == trigram[0]) && (char2 == trigram[1]) &&
	    (char3 == trigram[2]);
}

//.. Procedure witch set the number of top trigrams contained in the message msg
//.. in the list of struct Dec_msg
void
setTopTrigrams(Dec_msg * msg, char *trigrams[])
{
	int i, j;

	for (i = 0; i < NUM_TRIGRAMS; i++) {
		for (j = 2; j < strlen(msg->msg); j++) {
			if (trigramValid
			    (msg->msg[j - 2], msg->msg[j - 1], msg->msg[j],
			     trigrams[i])) {
				msg->toptrigrams++;
			}
		}
	}
}

//.. Procedure witch set the aplicant by searching the higher toptrigrams number
void
setTrigramAplicant(Dec_msg * msgs[])
{
	int i, higher, aplicant;

	aplicant = 0;
	higher = msgs[0]->toptrigrams;
	for (i = 0; i < MAX_KEY; i++) {
		if (msgs[i]->toptrigrams > higher) {
			higher = msgs[i]->toptrigrams;
			aplicant = i;
		}
	}
	msgs[aplicant]->aplicant = 1;
}

//.. Procedure witch print the key and the decrypted msg estimated to be correct
void
showAplicants(Dec_msg * msgs[])
{
	int i;

    printf("\nCANDIDATO/S:\n");
	for (i = 0; i < MAX_KEY; i++) {
		if (msgs[i]->aplicant) {
			printf("key %d: %s\n", msgs[i]->key, msgs[i]->msg);
		}
	}
}

int
main(int argc, char *argv[])
{

/*  
englishfr is an array of floats whose numbers belong to the relative frequency
of letters in english text in alphabetical order LIKE THE VAR ALPHABET
englishfr[0]= fr of 'a', letterfr[1] = fr of 'b', etc
https://www3.nd.edu/~busiforc/handouts/cryptography/letterfrequencies.html 
*/
	float englishfr[] =
	    { 8.4966, 2.0720, 4.5388, 3.3844, 11.1607, 1.8121, 2.4705,
		3.0034, 7.5448, 0.1965, 1.1016, 5.4893, 3.0129, 6.6544, 7.1635,
		    3.1671,
		0.1962, 7.5909, 5.7351, 6.950, 3.6308, 1.0074, 1.2899, 0.2902,
		    1.7779,
		0.2722
	};
	char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *bigrams[] =
	    { "TH", "HE", "IN", "ER", "AN", "RE", "ON", "AT", "EN",
		"ND", "TI", "ES", "OR", "TE", "OF", "ED", "IS", "IT", "AL",
		    "AR", "ST",
		"TO", "NT", "NG", "SE", "HA", "AS", "OU", "IO", "LE", "VE",
		    "CO", "ME",
		"DE", "HI", "RI", "RO", "IC", "NE", "EA", "RA", "CE"
	};
	char *trigrams[] =
	    { "THE", "AND", "THA", "ENT", "ING", "ION", "TIO", "FOR",
		"HER", "NTH", "INT", "ERE", "TER", "EST", "ERS", "ATI", "HAT",
		    "ATE",
		"ALL", "ETH", "HES", "VER", "HIS", "OFT", "ITH", "FTH", "STH",
		    "ONT"
	};
	char *text;
	Dec_msg *decrypted_msgs[MAX_KEY];
	int i;

	if (argc != 2) {
		errx(0, "Invalid arguments");
	}
	memset(decrypted_msgs, 0, sizeof(Dec_msg *) * MAX_KEY);
	text = strdup(argv[1]);
	if (text == NULL) {
		err(EXIT_FAILURE, "Cannot malloc");
	}
	upper2oneLine(text);
	initDecryptedMsgs(text, decrypted_msgs);

	for (i = 0; i < MAX_KEY; i++) {
		setDecryptedMsg(text, decrypted_msgs, i);
	}
	free(text);
	for (i = 0; i < MAX_KEY; i++) {
		setDistance(decrypted_msgs[i], englishfr, alphabet);
	}
	for (i = 0; i < MAX_KEY; i++) {
		setTopBigrams(decrypted_msgs[i], bigrams);
	}
	for (i = 0; i < MAX_KEY; i++) {
		setTopTrigrams(decrypted_msgs[i], trigrams);
	}
	setLetterAplicant(decrypted_msgs);
	setBigramAplicant(decrypted_msgs);
	setTrigramAplicant(decrypted_msgs);

	showAplicants(decrypted_msgs);
	freeMemory(decrypted_msgs);
	exit(EXIT_SUCCESS);
}
