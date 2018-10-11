#include <stdio.h>
#include <stdlib.h>


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
	char cur_element_passwd, cur_element_line;
	int isNotEOF1 = 1;
	int isNotEOL1 = 1;
	int isCorrectGroup = 1;

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
					do {
						isNotEOF1 = fread( &cur_element_line, sizeof( cur_element_line), 1, cur_line );
						fwrite( &cur_element_line, sizeof( cur_element_line), 1, foutput );
					} while ( ( cur_element_line != '\n') && ( isNotEOF1 == 1 ) );
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



	return ( 0 );
}
