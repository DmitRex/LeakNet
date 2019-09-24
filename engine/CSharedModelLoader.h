// VXP

#ifndef CSHAREDMODELLOADER_H
#define CSHAREDMODELLOADER_H
#ifdef _WIN32
#pragma once
#endif

#include "engine/ISharedModelLoader.h"
//#include "utlrbtree.h"

class CSharedModelLoader : public ISharedModelLoader
{
public:
	CSharedModelLoader();

	void					InitFilesystem( IBaseFileSystem *filesystem );
	studioanimgrouphdr_t	*LoadSharedModel( const char *path );

private:
	IBaseFileSystem			*m_pFilesystem;
	studioanimgrouphdr_t	*header;
};


#endif // CSHAREDMODELLOADER_H
