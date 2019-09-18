// VXP

#include <stdio.h>
#include <string.h>
#include "studio.h"

int main( int argc, char **argv )
{
	FILE *fp = fopen( argv[1], "rb" );
	fseek( fp, 0, SEEK_END );
	int len = ftell( fp );
	if ( len <= 0 )
	{
		fprintf( stderr, "Model %s is empty\r\n", argv[1] ); // Hello houndeye!
		fclose( fp );
		return -1;
	}

	rewind( fp );

	studiohdr_t *pStudioHdr = ( studiohdr_t * )malloc( len );
	if ( pStudioHdr == NULL ) // Cannot allocate a memory
	{
		fprintf( stderr, "Unexpected memory error\r\n" );
		fclose( fp );
		return -1;
	}

	size_t readlen = fread( pStudioHdr, 1, len, fp );
	if ( readlen != len || pStudioHdr == NULL )
	{
		fprintf( stderr, "Error reading a model\r\n" );
		fclose( fp );
		return -1;
	}

	Studio_ConvertStudioHdrToNewVersion( pStudioHdr );

	if( pStudioHdr->version != STUDIO_VERSION )
	{
	//	fprintf( stderr, "Wrong version (%d != %d)\r\n", ( int )pStudioHdr->version, ( int )STUDIO_VERSION );
		fclose( fp );
		return -1;
	}

	mstudiodummy1_t *dummy1 = pStudioHdr->pDummy1( 0 );
	if ( dummy1->dummy1 != 0 )
		printf( "%s has %d\r\n", pStudioHdr->name, dummy1->dummy1 );

	fclose( fp );
	return 0;
}
