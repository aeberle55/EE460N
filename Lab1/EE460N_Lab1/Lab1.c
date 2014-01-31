/*
//	Todo:
//	processOpcode;	Generate the binary for each opcode
//	parseOpcode;	Generate numeric representation of opcode from string
//	RegNum;			Translates character string into register number
//	genOffset;		Find an offset between two PC values
//	searchSymbol;	Finds Symbol tabel entry for character string
//	isOpcode;		Determines if a character string coorisponds to an opcode or not
*/

#include "Lab1.h"

int main(int argc, char** argv)
{
	char* line;		//Last line read
	char* label;	//Last label read
	char* oCode;	//Last opcode read
	char* a1;		//Last arguments read
	char* a2;
	char* a3;
	char* a4;
	int* steer;
	int stat;		//Status code of last line
	int num;
	char *prgName   = NULL;
    char *iFileName = NULL;
    char *oFileName = NULL;

    prgName   = argv[0];
    iFileName = argv[1];
    oFileName = argv[2];

	line=(char*)malloc((MAX_LINE_LENGTH+1)*sizeof(char));
    printf("program name = '%s'\n", prgName);
    printf("input file name = '%s'\n", iFileName);
    printf("output file name = '%s'\n", oFileName);

	/* open the source file */
     infile = fopen(argv[1], "r");
     outfile = fopen(argv[2], "w");
 
     if (infile==NULL) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
		 }
     if (outfile==NULL) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
     }
	 stat = EMPTY_LINE;
	 while(stat == EMPTY_LINE)	//Find first line
		stat=readAndParse( infile, line, &label, &oCode, &a1, &a2, &a3, &a4);
	 if(strcmp(oCode, ".orig")!=0)
	 {
		 printf("Error: File does not begin with .orig");
		 closeFiles();
		 exit(4);
	 }
	 origin = toNum(a1);	//Make sure origin is in correct range
	 if(origin < 0 || origin > 65535)
	 {
		 printf("Error: Invalid Origin, must be between 0x0000 and 0xFFFF");
		 closeFiles();
		 exit(4);
	 }
	 PC = origin;

	 while(stat != DONE)
	 {
		 stat = readLabel(infile, line, &label);
		 if(*label != '\0')
		 {
			 addLabel(&label);			 
		 }
		 //Increment PC
		 if(stat == OK)
			 PC+=2;
	 }
	 rewind(infile);
	 PC=origin;
	 stat = OK;
	 while(stat != DONE)
	 {
		 stat = readAndParse( infile, line, &label, &oCode, &a1, &a2, &a3, &a4);
		 if(stat == OK)
		 {
			 num = parseOpcode(oCode,steer);
			 if(num== -1)
			 {
				printf("Error: %s is not a valid opcode\n",oCode);
				closeFiles();
				exit(2);
			 }
			 if(num == 17)
				 stat=DONE;
			 else
			 {
				 //Execute Opcode
				processOpcode( num, steer, a1, a2, a3, a4);
				PC+=2;
			 }
		 }
	 }
	 //Insert opcode parsing here


	closeFiles();
    return 0;
}


//Closes input and output files
void closeFiles()
{
	int x;
	fclose(infile);
    fclose(outfile);
	//Prevent memory leaks
	for(x=0; x<numSymbols;x++)
		free(table[x].label);
}

//Converts null terminated char string to a number
//Works on decimal or hex
int toNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}


//Takes a file and parses into elements; returns int based on findings
// FILE * pInfile – file containing next line
// char * pLine – location to put unparsed line
// char ** pLabel – Pointer to label
// char ** pOpcode – Pointer to opcode
// char ** pArg… - Pointer to up to 4 arguments
int	readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
	{
	   char * lPtr;
	   int i;
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		return( DONE );
	   for( i = 0; i < (int)strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );
	   
           /* convert entire line to lowercase */
	   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	   /* ignore the comments */
	   lPtr = pLine;

	   while( *lPtr != ';' && *lPtr != '\0' &&
	   *lPtr != '\n' ) 
		lPtr++;

	   *lPtr = '\0';
	   if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
		return( EMPTY_LINE );

	   if( isOpcode( lPtr ) == 0 && lPtr[0] != '.' ) /* found a label */
	   {
		*pLabel = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   }
	   
           *pOpcode = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   
           *pArg1 = lPtr;
	   
           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg2 = lPtr;
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg3 = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg4 = lPtr;

	   return( OK );
	}

//Find any label
int readLabel(FILE * pInfile, char * pLine, char ** pLabel)
{
	char * lPtr;
	   int i;
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		return( DONE );
	   for( i = 0; i < (int) strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );
	   
           /* convert entire line to lowercase */
	   *pLabel = pLine + strlen(pLine);

	   /* ignore the comments */
	   lPtr = pLine;

	   while( *lPtr != ';' && *lPtr != '\0' &&
	   *lPtr != '\n' ) 
		lPtr++;

	   *lPtr = '\0';
	   if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
		return( EMPTY_LINE );

	   if( isOpcode( lPtr ) == 0 && lPtr[0] != '.' ) /* found a label */
	   {
		*pLabel = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   }
	   return(OK);
}

//Adds a label to the symbol table
//Throws error if invalid label
void addLabel(char** label)
{
	int size;
	int k;
	char* labelLoc;
	if(numSymbols>=MAX_SYMBOLS)
	{
		printf("Error: Too many symbols\n",label);
		closeFiles();
		exit(4);
	}
	size = strlen(*label);
	if(size > 20)	//Oversized Label
	{
		printf("Error: The Label %s is too long; Must be less than 20 characters\n",label);
		closeFiles();
		exit(4);
	}
	if(searchSymbol(*label) != NULL)	//Label is opcode
	{
		printf("Error: The Label %s is used multiple times\n",label);
		closeFiles();
		exit(4);
	}
	for(k = 0; k<numIllegal;k++)	//Label is illegal
	{
		if(strcmp(*label, illegalLabel[k])==0)
		{
		printf("Error: The Label %s is an illegal label\n",label);
		closeFiles();
		exit(4);
		}
	}
	if(**label == 'x' || !isalpha(**label))	//First letter of label not a letter, or is x
	{
		if(strcmp(*label, illegalLabel[k])==0)
			{
			printf("Error: The Label %s is an illegal label\n",label);
			closeFiles();
			exit(4);
			}
	}
	for(k=1; k<size;k++)	//Checks for non-alphanumeric label
	{
		if(!isalnum(*(*label + k)))
		{
		printf("Error: The Label %s is an illegal label\n",label);
		closeFiles();
		exit(4);
		}
	}
	if(RegNum(*label)!= -1)
	{
		printf("Error: The Label %s is a register name\n",label);
		closeFiles();
		exit(4);
	}
	labelLoc=(char*)malloc((size+1)*sizeof(char));	//Allocate memory for label
	strcpy(labelLoc,*label);	
	table[numSymbols].label=labelLoc;	//Add label to symbol tabel
	table[numSymbols].location=PC;
	numSymbols++;
}

//Returns 0 if not an opcode; 1 otherwise
int isOpcode(char * code)
{
	return 0;
}

//Returns line number of a given symbol sym
//Return Null if not found or invalid label
Symbol* searchSymbol(char * sym)
{
	return NULL;
}

//Generates an offset based on the current PC and an offset
//Throws error is number of necessary digits for offset exceeds 
//maxDigits; 1 otherwise
int genOffset(int offset, int maxDigits)
{
	return 0;
}

//Returns integer corresponding to register of character string reg
//Returns -1 if not a valid register
int RegNum(char * reg)
{
	return -1;
}

//Returns integer equal to the binary representation of the opcode
//Return 16 for .fill, 17 for .end, and -1 for an invalid opcode
//sets steering if necessary for special cases due to overloaded opcodes
//Special Cases:
	//JSR - steering=1
	//JSRR – steering=0
	//LSHF – steering=0
	//RSHFL – steering=1
	//RSHFA – steering=3
	//NOP – steering=1
	//HALT – steering=1
	//NOT – steering=1
	//JMP – steering=0
	//RET – steering=1
	//Otherwise – steering=0
int parseOpcode(char * opcode, int * steering)
{
	*steering = 0;
	return -1;
}

void processOpcode( int code, int * steer, char * pArg1, char * pArg2, char * pArg3, char * pArg4)
{
	int output = 0;
	int DR = -1;
	int SR1 = -1;
	int SR2 = -1;
	int num = -1;
	Symbol* temp = NULL;
	if(code <= 15 && code >= 0)
		output = code<<12;
	switch (code)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	case 9:			//XOR and NOT
		DR = RegNum(pArg1);
		SR1 = RegNum(pArg2);
		//Too many arguments, or invalid
		if(*pArg3 != '\0' || *pArg4 != '\0'||SR1 == -1 || DR == -1)	
		{
			printf("Error: invalid argument\n");
			closeFiles();
			exit(4);
		}
		code += DR<<9+SR1<<6;	//Common to all implementations
		if(*steer == 0)
		{
			SR2=RegNum(pArg3);
			if(SR2 == -1)		//Immediate Value
			{
				num = toNum(pArg3);
				if(num>15 || num < -16)	//5 Bits
				{
					printf("Error: Immediate Value Out of Bounds\n");
					closeFiles();
					exit(3);
				}
				code+= 1<<5 + num;
			}
			else	//Register value
			{
				code+=SR2;
			}
		}
		else	//NOT function
		{
			output += 63;
		}
		break;
	//Opcodes 10 and 11 are invalid
	case 12:	//JMP
		SR1 = RegNum(pArg1);
		if(*pArg2 != '\0' ||*pArg3 != '\0' || *pArg4 != '\0'||SR1==-1)
		{
			printf("Error: invalid argument\n");
			closeFiles();
			exit(4);
		}
		output+=SR1<<6;
		break;
	case 13:	//Shift
		DR = RegNum(pArg1);
		SR1 = RegNum(pArg2);
		num = toNum(pArg3);
		if(*pArg4 != '\0' || DR==-1||SR1==-1)
		{
			printf("Error: invalid argument\n");
			closeFiles();
			exit(4);
		}
		if(num>15||num<0)
		{
			printf("Error: Immediate Value Out of Bounds\n");
			closeFiles();
			exit(3);
		}
		//Steering integer also acts as steering bits
		output += DR<<9+SR1<<6+(*steer)<<4+num;
		break;
	case 14:	//LEA
		DR = RegNum(pArg1);
		temp = searchSymbol(pArg2);
		if(temp == NULL)	//Label not found in symbol tabel
		{
			printf("Error: Unidentified Label\n");
			closeFiles();
			exit(1);
		}
		if(DR==-1 || *pArg3 != '\0' || *pArg4 != '\0')
		{
			printf("Error: invalid argument\n");
			closeFiles();
			exit(4);
		}
		num = genOffset(temp->location-PC,9);	//genOffset will throw out of bounds error if necisarry
		output=DR<<9+num;
		break;
	case 15:	//TRAP
		//Must be hex
		if(*pArg2 != '\0' || *pArg3 != '\0' || *pArg4 != '\0')
		{
			printf("Error: invalid argument\n");
			closeFiles();
			exit(4);
		}
		if(*steer == 1)	//HALT
		{
			if(*pArg1 != '\0')
			{
				printf("Error: invalid argument\n");
				closeFiles();
				exit(4);
			}
			num= 37;
		}
		else
		{
			if(*pArg1 != 'x')	//Must be a hex value
			{
				printf("Error: TRAP must be a hex value\n");
				closeFiles();
				exit(3);
			}
			num = toNum(pArg1);
			if(num>255||num<0)		//>0 and no more than 8 digits
			{
				printf("Error: TRAP Value Out of Bounds\n");
				closeFiles();
				exit(3);
			}
		}
		output+=num;
		break;
	case 16:	//.FILL
		if(*pArg2 != '\0' || *pArg3 != '\0' || *pArg4 != '\0')
		{
			printf("Error: invalid argument\n");
			closeFiles();
			exit(4);
		}
		temp = searchSymbol(pArg1);
		if(temp != NULL)			//Allows addressing via labels
			num = temp->location;
		else
			num = toNum(pArg1);
		if(num>65535|| num< -32768)	//Can be signed or unsigned
		{
			printf("Error: TRAP Value Out of Bounds\n");
			closeFiles();
			exit(3);
		}
		output = num;
		break;
	default:
		printf("Error: invalid opcode\n");
		closeFiles();
		exit(2);
	}
	fprintf( outfile, "0x%0.4X\n", output );		//Prints out formated Hex version of code
}