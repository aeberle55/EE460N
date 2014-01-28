#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

struct Symbol{
	int location;
	char* label;
};

#define MAX_LINE_LENGTH 255
enum
{
   DONE, OK, EMPTY_LINE
};


char *prgName   = NULL;	//Name of Program
char *iFileName = NULL;	//Input file name
char *oFileName = NULL;	//Output file name
FILE* infile = NULL;		//Input file stream
FILE* outfile = NULL;		//Output file stream
int error=0;			//0 if no error; otherwise error code 1-4
int origin;			//Starting point; set by .orig
int PC;				//Program counter
Symbol table[100];		//Array of symbol objects for symbol table

const char* codes = ("add", "and", "br", "halt", "jmp", "jsr", "jsrr", "ldb", "ldw", "lea", "nop",
	"not", "ret", "lshf", "rshfl", "rshfa", "rti", "stb", "stw", "trap", "xor",".orig",".fill",".end");


int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
	);

int toNum( char * pStr ); 

int SearchSymbol(char * sym);

int RegNum(char * reg);

int genOffsett(int offset, int maxDigits, int* isValid);

int parseOpcode(char * opcode, int * steering);

int isOpcode(char * code);

void closeFiles();