// VXP

#include "CSharedModelLoader.h"
#include "modelgen.h"

EXPOSE_SINGLE_INTERFACE( CSharedModelLoader, ISharedModelLoader, ISHAREDMODELLOADER_INTERFACE_VERSION );

CSharedModelLoader::CSharedModelLoader()
{
	header = (studioanimgrouphdr_t *)malloc( 512 );
	memset( header, 0, 512 );
}

void CSharedModelLoader::InitFilesystem( IBaseFileSystem *filesystem )
{
	m_pFilesystem = filesystem;
}

studioanimgrouphdr_t *CSharedModelLoader::LoadSharedModel( const char *path )
{
	if ( header->id != NULL )
		return header;

	FileHandle_t pFileHandle = m_pFilesystem->Open( path, "rb" );
	if ( pFileHandle == NULL )
	{
		Warning( "CSharedModelLoader: Cannot open %s\n", path );
		return NULL;
	}

	int nSize = m_pFilesystem->Size( pFileHandle );
	if ( nSize <= 0 )
	{
		Warning( "CSharedModelLoader: Cannot read %s\n", path );
		m_pFilesystem->Close( pFileHandle );
		return NULL;
	}

	studioanimgrouphdr_t *sharedModelMemory = (studioanimgrouphdr_t *)malloc( nSize );
	int readSize = m_pFilesystem->Read( sharedModelMemory, nSize, pFileHandle );
	if ( readSize != nSize )
	{
		Warning( "CSharedModelLoader: %s is corrupted\n", path );
		m_pFilesystem->Close( pFileHandle );
		return NULL;
	}

	m_pFilesystem->Close( pFileHandle );

	if ( sharedModelMemory->id != IDSTUDIOANIMGROUPHEADER )
	{
		Warning( "CSharedModelLoader: %s is not a valid animgroup file\n", path );
		return NULL;
	}

	sharedModelMemory->pAnimdesc(0); // For debugger

	header = sharedModelMemory;
	return header;
}
