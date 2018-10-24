#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *
readUntilEOL(FILE* line); //returns buffer, if line was read correctly; else - NULL

int
isNumber(char *string); // checks if string == number, then 1; esle - 0;

void
startErrorsCheck( FILE *file, int arg_count, char *group_number ); // checks if 1) passwd not found, 2) argc < 3, 3) first parameter is not number

FILE *
createFile( char *fileName ); // returns file with name argv[2]


int main(int argc, char *argv[]) {
	FILE *passwd = fopen( "./passwd", "r" );
	FILE *cur_line = fopen( "./passwd", "r" );

	startErrorsCheck( passwd, argc, argv[1] ); 

	FILE *foutput = createFile( argv[2] ); 

	char cur_element_passwd; // element, which was read from passwd
	int isNotEOF = 1; 
	int isNotEOL = 1;
	int isCorrectGroup = 1; 
	char *line_with_correct_group; // line for number of group in passwd
	char *number_of_correct_group = argv[1];
	unsigned long int line_counter = 1; // number of current line in passwd. For print number of invalid lines
	const unsigned int COLON_COUNT_TOGROUP = 3; // number of colons to GID

	unsigned int colon_count = 0; // count of colons in single line

	fseek(passwd, 0, SEEK_SET); 
	fseek(cur_line, ftell(passwd), SEEK_SET); // replace pointer to the start of file

	while (isNotEOF == 1) {
		while (isNotEOL == 1) {
                        //if it is end of file then break;
			if (fread(&cur_element_passwd, sizeof(cur_element_passwd), 1, passwd) == 0) { 
				isNotEOF = 0;
				break;
			}  
			if (cur_element_passwd == '\n') { // if it is end of line
				if ( colon_count < COLON_COUNT_TOGROUP ) {
					fprintf( stderr, "Invalid line - %ld\n", line_counter );
				}
				isNotEOL = 0;
				fseek(cur_line, ftell( passwd), SEEK_SET); 
			}
			if (cur_element_passwd == ':') {
				colon_count++;
			}
			if (colon_count == COLON_COUNT_TOGROUP) { // if we meet third ':' then check GID and write to foutput
				unsigned int digits_counter = 0; // number of digits in GID in passwd
				do {
					if ( fread(&cur_element_passwd, sizeof(cur_element_passwd), 1, passwd) == 0 ) {  
						isNotEOF = 0;
						isCorrectGroup = 1; // always is True here, because conditional after cycle check number of group again
						digits_counter++;
						break;
					} else {
						if ( !isdigit(cur_element_passwd) && cur_element_passwd != ':' && cur_element_passwd != '\n' ) { // if GID has not a digits and it is not '\n', '\0'
							fprintf( stderr, "Invalid line - %ld\n", line_counter );
							break;
						}
                                                // our number can end by EOL and ':'. It is not error
						if ((cur_element_passwd != number_of_correct_group[digits_counter]) && (cur_element_passwd != ':') && (cur_element_passwd != '\n')) { 
							isCorrectGroup = 0;
						}
						if ((cur_element_passwd == ':') && (digits_counter == 0)) { // GID has no digits
                                                    fprintf( stderr, "Invalid line - %ld\n", line_counter ); 
                                                    isCorrectGroup = 0;
                                                    break;
						}
						digits_counter++; 
					}
				} while ( cur_element_passwd != ':' && cur_element_passwd != '\n' );
				if ( digits_counter == 1 ) { // invalid line validator
					fprintf( stderr, "Invalid line - %ld\n", line_counter );
				}
				if ( cur_element_passwd == '\n' ) { // if it is EOL or EOF
					isNotEOL = 0;
				}

				if ( ( digits_counter - 1 ) != ( strlen( number_of_correct_group ) ) ) { //check if number of group in line == part of number of group from console
					isCorrectGroup = 0;
				}
				if ( isCorrectGroup == 1 ) {
						line_with_correct_group = readUntilEOL( cur_line );
						if ( line_with_correct_group != NULL ) {
							fputs(line_with_correct_group, foutput);
							free( line_with_correct_group );
						} else {
                            fprintf( stderr, "Line with correct GID was read incorrectly" );
                                                }
				}
				isCorrectGroup = 1; // refresh flag value
				do {  // read all symbols after GID to end of line or end of file
					isNotEOF = fread( &cur_element_passwd, sizeof( cur_element_passwd ), 1, passwd );
					if ( cur_element_passwd == '\n' ) {
						isNotEOL = 0;
						fseek( cur_line, ftell( passwd ), SEEK_SET );
					}

				} while ((isNotEOF == 1) && (isNotEOL == 1));
				if (isNotEOF == 0) { 
					isNotEOL = 0;
				}
			}
		}
		line_counter++;
		colon_count = 0;
		isNotEOL = 1;
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
	char *buffer_saver = NULL; // if after realloc we lost memory
	if ( line == NULL ) { 
		perror( "Empty file!" );
		return NULL;
	} else {
		const unsigned int BUFFER_SIZE = 1000000;
		buffer = calloc( BUFFER_SIZE, sizeof( *buffer ) );
		if ( buffer == NULL ) {
			 perror( "Not enough memory for calloc" );
			 return NULL;
		}
		do {
			buffer_saver = fgets( buffer + ( cycle_counter - 1 ) * ( BUFFER_SIZE - 1 ), BUFFER_SIZE, line );
			if ( buffer_saver == NULL ) {  //FIX
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
				buffer_saver = realloc( buffer,  memory_size + 1 + ( BUFFER_SIZE - 1 ) * ( cycle_counter - 1 ) * sizeof( *buffer )  );
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
	for( unsigned int i = 0; i < strlen( string ); i++ ) {
		if ( !( isdigit( string[i] ) ) ) {
			return 0;
		}
	}
	return 1;
}

void
startErrorsCheck( FILE *file, int arg_count, char *group_number )
{
	if ( file == NULL ) {
		perror( "File \"passwd\" does not exist" );
		exit(1);
	}
	char *checkIfEmptyFile_buf;
	if ( fread( &checkIfEmptyFile_buf, sizeof(*checkIfEmptyFile_buf), 1, file) == 0 ) {
		fprintf( stderr, "Empty File!\n" );
		exit(1);
	}
	//If first two parametrs ( number of group  and filename ) is NULL
	if ( arg_count < 3 ) {
		fprintf( stderr, "One or more parameters are missed\n" );
		exit(1);
	}

	if (!(isNumber( group_number))) {
		fprintf( stderr, "Second parametr must be a number. Please, try again.\n" );
		exit(1);
	}
}

FILE *
createFile( char *fileName )
{
	char *path_to_file;
	path_to_file = calloc(strlen( fileName ) + 3, sizeof(char));
	if ( path_to_file == NULL) {
		perror("Not enought memory to realloc: ");
		exit(0);
	}
	memcpy( path_to_file , "./", 2 );
	memcpy( path_to_file + 2 , fileName, strlen( fileName )  );
	path_to_file[strlen( fileName ) + 2] = '\0';
	FILE *foutput = fopen( path_to_file , "w+" );
	free( path_to_file );
	return foutput;
}