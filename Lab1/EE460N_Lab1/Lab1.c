#include "Lab1.h"

int main(int argc, char** argv)
{
	char* line;		//Last line read
	char** label;	//Last label read
	char** oCode;	//Last opcode read
	char** a1;		//Last arguments read
	char** a2;
	char** a3;
	char** a4;
	char* labelLoc;	//Temporary Variable for symbol table generation
	int stat;		//Status code of last line
	int size;		//Tempory Variable for symbol table generation
	char *prgName   = NULL;
    char *iFileName = NULL;
    char *oFileName = NULL;
	int k;			//Temproary integer

    prgName   = argv[0];
    iFileName = argv[1];
    oFileName = argv[2];

    printf("program name = '%s'\n", prgName);
    printf("input file name = '%s'\n", iFileName);
    printf("output file name = '%s'\n", oFileName);

	/* open the source file */
     infile = fopen(argv[1], "r");
     outfile = fopen(argv[2], "w");
		 
     if (!infile) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
		 }
     if (!outfile) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
     }
	 stat = EMPTY_LINE;
	 while(stat == EMPTY_LINE)	//Find first line
		stat=readAndParse( infile, line, label, oCode, a1, a2, a3, a4);
	 if(strcmp(*oCode, ".orig")!=0)
	 {
		 printf("Error: File does not begin with .orig");
		 closeFiles();
		 exit(4);
	 }
	 origin = toNum(*a1);	//Make sure origin is in correct range
	 if(origin < 0 || origin > 65535)
	 {
		 printf("Error: Invalid Origin, must be between 0x0000 and 0xFFFF");
		 closeFiles();
		 exit(4);
	 }
	 PC = origin;

	 while(stat != DONE)
	 {
		 stat = readLabel(infile, line, label);
		 if(*label != '\0')
		 {
			 size = strlen(*label);
			 if(size > 20)	//Oversized Label
			 {
				printf("Error: The Label %s is too long; Must be less than 20 characters\n",label);
				closeFiles();
				exit(4);
			 }
			 if(searchSymbol(*label) != -1)	//Label is opcode
			 {
				printf("Error: The Label %s is used multiple times\n",label);
				closeFiles();
				exit(4);
			 }
			 for(k = 0; k<numIllegal;k++)	//Label is illegal
			 {
				 if(strcmp(*label, &illegalLabel[k])==0)
				 {
					printf("Error: The Label %s is an illegal label\n",label);
					closeFiles();
					exit(4);
				 }
			 }
			if(**label == 'x' || !isalpha(**label))	//First letter of label not a letter, or is x
			{
				if(strcmp(*label, &illegalLabel[k])==0)
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

			 labelLoc=(char*)malloc((size+1)*sizeof(char));	//Allocate memory for label
			 strcpy(labelLoc,*label);	
			 table[numSymbols].label=labelLoc;	//Add label to symbol tabel
			 table[numSymbols].location=PC;
			 numSymbols++;
			 
		 }
		 //Increment PC
		 if(stat == OK)
			 PC+=2;
	 }
	 rewind(infile);
	 PC=origin;

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
	   char * lRet, * lPtr;
	   int i;
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		return( DONE );
	   for( i = 0; i < strlen( pLine ); i++ )
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

	   if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
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
	char * lRet, * lPtr;
	   int i;
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		return( DONE );
	   for( i = 0; i < strlen( pLine ); i++ )
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

	   if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
	   {
		*pLabel = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   }
	   return(OK);
}