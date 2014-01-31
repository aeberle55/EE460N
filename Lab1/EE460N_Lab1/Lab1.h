#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

typedef struct{
	int location;
	char* label;
}Symbol;

#define MAX_LINE_LENGTH 255
#define MAX_SYMBOLS 255
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
int numSymbols = 0;	//Number of labels in symbol table
Symbol table[255];		//Array of symbol objects for symbol table

#define numCodes 24
char* codes[numCodes] = {"add", "and", "br", "halt", "jmp", "jsr", "jsrr", "ldb", "ldw", "lea", "nop",
	"not", "ret", "lshf", "rshfl", "rshfa", "rti", "stb", "stw", "trap", "xor",".orig",".fill",".end"};

#define numIllegal 4
char* illegalLabel[numIllegal] = {"in","out","getc","putc"};

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
	);

int toNum( char * pStr ); 

Symbol* searchSymbol(char * sym);

int RegNum(char * reg);

int genOffset(int offset, int maxDigits);

int parseOpcode(char * opcode, int * steering);

int isOpcode(char * code);

void closeFiles();

int readLabel(FILE * pInfile, char * pLine, char ** pLabel);

void addLabel(char** label);


void processOpcode( int code, int * steer, char * pArg1, char * pArg2, char * pArg3, char * pArg4);
