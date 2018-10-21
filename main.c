#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *
readUntilEOL(FILE* line); //returns buffer, if line was read correctly; else - NULL

int
isNumber(char *string); // check if string == number, then 1; esle - 0;


int main(int argc, char *argv[]) {
	FILE *passwd = fopen( "./passwd", "r" );
	FILE *cur_line = fopen( "./passwd", "r" );

	if ( passwd == NULL ) {
		perror( "File \"passwd\" does not exist" );
		exit( 1 );
	}

	//If first two parametrs ( number of group  and filename ) is NULL
	if ( argc < 3 ) {
		fprintf( stderr, "One or more parameters are missed\n" );
		return 1;
	}

	if (!(isNumber( argv[1]))) {
		fprintf( stderr, "Second parametr must be a number. Please, try again.\n" );
		exit( 1 );
	}


	char *fileName;
	fileName = calloc(strlen(argv[2]) + 3, sizeof(char));
	if (fileName == NULL) {
		perror("Not enought memory to realloc: ");
		exit(0);
	}
	strcat(fileName, "./" );
	strcat(fileName, argv[2]);
	FILE *foutput = fopen(fileName , "w+" );
	free(fileName);

	char cur_element_passwd;
	int isNotEOF1 = 1;
	int isNotEOL1 = 1;
	int isCorrectGroup = 1;


	char *line_with_correct_group;
	char *number_of_correct_group = argv[1];

	unsigned int doubleDot_count = 0; // count of Double dot in single Line

	fseek(cur_line, ftell(passwd), SEEK_SET);

	while (isNotEOF1 == 1) {
		while (isNotEOL1 == 1) {
			if (fread(&cur_element_passwd, sizeof(cur_element_passwd), 1, passwd) == 0) {
				isNotEOF1 = 0;
			}  //if it is end of file then break;
			if (cur_element_passwd == '\n') { // if it is end of line
				isNotEOL1 = 0;
				fseek(cur_line, ftell( passwd), SEEK_SET);
			}
			if (cur_element_passwd == ':') {
				doubleDot_count++;
			}
			if (doubleDot_count == 3) { // if we meet third ':' then check GID and write to foutput
				int i = 0;
				do {
					fread(&cur_element_passwd, sizeof(cur_element_passwd), 1, passwd);
					if ( ( cur_element_passwd != number_of_correct_group[ i ]  ) && ( cur_element_passwd != ':' ) ) {
						isCorrectGroup = 0;
					}
				 	if ( ( cur_element_passwd == ':' ) && ( i == 0 ) ) {
				 		isCorrectGroup = 0;
				 	}
					i++;
				} while ( cur_element_passwd != ':' );
				if ( ( i - 1 ) != ( strlen( number_of_correct_group ) ) ) { //check if number of group in line == part of number of group from console
					isCorrectGroup = 0;
				}
				if ( isCorrectGroup == 1 ) {
						line_with_correct_group = readUntilEOL( cur_line );
						if ( line_with_correct_group != NULL ) {
							fputs(line_with_correct_group, foutput);
							free( line_with_correct_group );
						}
				}
				isCorrectGroup = 1;
				do {  // read all symbols after GID to end of line or end of file
					isNotEOF1 = fread( &cur_element_passwd, sizeof( cur_element_passwd ), 1, passwd );
					if ( cur_element_passwd == '\n' ) {
						isNotEOL1 = 0;
						fseek( cur_line, ftell( passwd ), SEEK_SET );
					}

				} while ( ( isNotEOF1 == 1 ) && ( isNotEOL1 == 1 ) );
				//doubleDot_count = 0;
				if ( isNotEOF1 == 0 ) {
					isNotEOL1 = 0;
				}
			}
		}
		doubleDot_count = 0;
		isNotEOL1 = 1;
	}

	fclose( passwd );
	fclose( foutput );
	fclose( cur_line );



	return ( 0 );
}


char *
readUntilEOL( FILE *line )
{
	int cycle_counter = 1;
	char *buffer = NULL;
	char *buffer_saver = NULL; // if after realloc we lose memory
	if ( line == NULL ) {
		perror( "Empty file!" );
		return NULL;
	} else {
		const unsigned int BUFFER_SIZE = 5;
		buffer = calloc( BUFFER_SIZE, sizeof( *buffer ) );
		if ( buffer == NULL ) {
			 perror( "Not enough memory for calloc" );
			 return NULL;
		}
		do {
			buffer_saver = fgets( buffer + ( cycle_counter - 1 ) * ( BUFFER_SIZE - 1 ), BUFFER_SIZE, line );
			if ( buffer_saver == NULL ) {  // third  variant
				return buffer;
			}
			if ( ( strlen( buffer ) == 1 ) && ( buffer[0] == '\n' ) ) {  // if it's blank string
				free( buffer );
				return NULL;
			}
			if ( buffer[ strlen( buffer ) - 1 ] == '\n' ) {
					return buffer;
			}
			cycle_counter++;
			buffer_saver = realloc( buffer, 1 + cycle_counter * ( BUFFER_SIZE - 1 ) * sizeof( *buffer ) );
			// we checked, that it's not EOL and EOF earlier, so we have at least one more symbol in line, so we have at least one byte of memory
			if ( buffer_saver == NULL ) {
				int memory_size = 1;
				buffer_saver = realloc( buffer, ( memory_size + 1 + ( BUFFER_SIZE - 1 ) * ( cycle_counter - 1 ) * sizeof( *buffer ) ) );
				while ( buffer_saver != NULL ) {
					buffer_saver = fgets( buffer + memory_size + ( cycle_counter - 1 ) * ( BUFFER_SIZE - 1 ), 1, line );
					if ( buffer_saver == NULL ) {  // third  variant
						return buffer;
					}
					if ( buffer[ strlen( buffer ) - 1 ] == '\n' ) {
						return buffer;
					}
					memory_size++;
					buffer_saver = realloc( buffer, ( memory_size + 1 + ( BUFFER_SIZE - 1 ) * ( cycle_counter - 1 ) * sizeof( *buffer ) ) );
				}
				perror( "Not enough memory for realloc." );
				free( buffer );
				return NULL;
			}
			buffer = buffer_saver;

		} while ( 1 );

	}
}

int
isNumber( char *string )
{
	if ( string == NULL ) {
		fprintf( stderr, "Error in function **isNumber**. First parametr is NULL" );
		return 3;
	}
	for( int i = 0; i < strlen( string ); i++ ) {
		if ( !( isdigit( string[i] ) ) ) {
			return 0;
		}
	}
	return 1;
}