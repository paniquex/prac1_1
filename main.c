#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *readUntilEOL( FILE* line ); //returns buffer, if line was read correctly; else - NULL


int main( void ) {
	FILE *passwd = fopen( "./passwd", "r" );
	FILE *cur_line = fopen( "./passwd", "r" );

	if ( passwd == NULL ) {
		perror( "File \"passwd\" does not exist" );
	}

//FUNCTION OF INPUT GROUP ID
//	int *groupID = calloc( 3, sizeof( groupID ) );
//	int groupIDCount = 0;
//	do {
//		groupID[groupIDCount] = getchar();
//		groupIDCount++;
//		if ( ( groupIDCount + 1 ) % 3 == 0 ) {
//			groupID = realloc( groupID, ( groupIDCount + 4 ) * sizeof( groupID ) );
//		}
//	} while( groupID[ groupIDCount - 1 ] != EOF );
//
//
//UNCTION 	OF INPUT NAME OF FILE;
//	int *foutputName = calloc( 3, sizeof( foutputName ) );
//	foutputName[0] = getchar(); // to read space
//	int foutputCount = 0;
//	do {
//		foutputName[foutputCount] = getchar();
//		foutputCount++;
//		if ( ( foutputCount + 1 ) % 3 == 0 ) {
//			foutputName = realloc( foutputName, ( foutputCount + 4 ) * sizeof( foutputName ) );
//		}
//	} while ( foutputName[ foutputCount - 1 ] != EOF );

	FILE *foutput = fopen( "./foutput" , "w" );
	char cur_element_passwd;
	int isNotEOF1 = 1;
	int isNotEOL1 = 1;
	int isCorrectGroup = 1;


	char *line_with_correct_group;

	unsigned int doubleDot_count = 0; // count of Double dot in single Line

	fseek( cur_line, ftell( passwd ), SEEK_SET );

	while ( isNotEOF1 == 1 ) {
		while ( isNotEOL1 == 1 ) {
			if ( fread( &cur_element_passwd, sizeof( cur_element_passwd ), 1, passwd ) == 0 ) {
				isNotEOF1 = 0;
				break;
			}  //if it is end of file then break;
			if ( cur_element_passwd == ':' ) {
				doubleDot_count++;
			}
			if ( doubleDot_count == 3 ) { // if we meet third ':' then check GID and write to foutput
				do {
					fread( &cur_element_passwd, sizeof( cur_element_passwd ), 1, passwd );
					if ( ( cur_element_passwd != '2' ) && ( cur_element_passwd != ':' ) ) {
						isCorrectGroup = 0;
					}
				} while ( cur_element_passwd != ':' );
				if ( isCorrectGroup == 1 ) {
						line_with_correct_group = readUntilEOL( cur_line );
						if ( line_with_correct_group != NULL ) {
							fputs(line_with_correct_group, foutput);
							free(line_with_correct_group);
						} else {
							printf( "\ncheck" );
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
				doubleDot_count = 0;
				if ( isNotEOF1 == 0 ) {
					isNotEOL1 = 0;
				}
			}
		}
		isNotEOL1 = 1;
	}

	fclose( passwd );
	fclose( foutput );
	fclose( cur_line );



	return ( 0 );
}


char *readUntilEOL( FILE *line ) {
	int cycle_counter = 1;
	char *buffer;
	char *buffer_saver; // if after realloc we lose memory
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
			buffer_saver = buffer;
			buffer_saver = realloc( buffer, cycle_counter * ( BUFFER_SIZE - 1 ) * sizeof( *buffer ) );
			if ( buffer_saver == NULL ) {
				perror( "Not enough memory for realloc: " );
				free( buffer );
				return NULL;
			}

		} while ( 1 );

	}
}