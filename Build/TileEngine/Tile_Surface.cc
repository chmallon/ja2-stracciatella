#include "HImage.h"
#include "Structure.h"
#include "TileDef.h"
#include "Tile_Surface.h"
#include "VObject.h"
#include "WorldDef.h"
#include "WorldDat.h"
#include "Debug.h"
#include "Smooth.h"
#include "MouseSystem.h"
#include "Sys_Globals.h"
#include "TileDat.h"
#include "FileMan.h"
#include "MemMan.h"
#include "Tile_Cache.h"


TILE_IMAGERY				*gTileSurfaceArray[ NUMBEROFTILETYPES ];


TILE_IMAGERY* LoadTileSurface(const char* cFilename)
{
	// Add tile surface
	AutoSGPImage hImage(CreateImage(cFilename, IMAGE_ALLDATA));
	if (hImage == NULL)
	{
		SET_ERROR( "Could not load tile file: %s", cFilename );
		return NULL;
	}

	AutoSGPVObject hVObject(AddVideoObjectFromHImage(hImage));
	if (hVObject == NULL)
	{
		SET_ERROR( "Could not load tile file: %s", cFilename );
		// Video Object will set error conition.]
		return NULL;
	}

	// Load structure data, if any.
	// Start by hacking the image filename into that for the structure data
	SGPFILENAME cStructureFilename;
	strcpy( cStructureFilename, cFilename );
	char* cEndOfName = strchr( cStructureFilename, '.' );
	if (cEndOfName != NULL)
	{
		cEndOfName++;
		*cEndOfName = '\0';
	}
	else
	{
		strcat( cStructureFilename, "." );
	}
	strcat( cStructureFilename, STRUCTURE_FILE_EXTENSION );
	AutoStructureFileRef pStructureFileRef;
	if (FileExists( cStructureFilename ))
	{
		pStructureFileRef = LoadStructureFile( cStructureFilename );
		if (pStructureFileRef == NULL)
		{
			SET_ERROR("Structure file error: %s", cStructureFilename);
			return NULL;
		}

		if (hVObject->usNumberOfObjects != pStructureFileRef->usNumberOfStructures)
		{
			SET_ERROR("Structure file error: %s", cStructureFilename);
			return NULL;
		}

		DebugMsg( TOPIC_JA2, DBG_LEVEL_3, cStructureFilename );

		if (!AddZStripInfoToVObject(hVObject, pStructureFileRef, FALSE, 0))
		{
			SET_ERROR(  "ZStrip creation error: %s", cStructureFilename );
			return NULL;
		}
	}

	TILE_IMAGERY* const pTileSurf = MALLOCZ(TILE_IMAGERY);

	if (pStructureFileRef && pStructureFileRef->pAuxData != NULL)
	{
		pTileSurf->pAuxData = pStructureFileRef->pAuxData;
		pTileSurf->pTileLocData = pStructureFileRef->pTileLocData;
	}
	else if (hImage->uiAppDataSize == hVObject->usNumberOfObjects * sizeof( AuxObjectData ))
	{
		// Valid auxiliary data, so make a copy of it for TileSurf
		pTileSurf->pAuxData = MALLOCN(AuxObjectData, hVObject->usNumberOfObjects);
		if (pTileSurf->pAuxData == NULL)
		{
			MemFree(pTileSurf);
			return NULL;
		}
		memcpy( pTileSurf->pAuxData, hImage->pAppData, hImage->uiAppDataSize );
	}
	else
	{
		pTileSurf->pAuxData = NULL;
	}

	pTileSurf->vo                = hVObject.Release();
	pTileSurf->pStructureFileRef = pStructureFileRef.Release();
	return pTileSurf;
}


void DeleteTileSurface(TILE_IMAGERY* const pTileSurf)
{
	if ( pTileSurf->pStructureFileRef != NULL )
	{
		FreeStructureFile( pTileSurf->pStructureFileRef );
	}
	else
	{
		// If a structure file exists, it will free the auxdata.
		// Since there is no structure file in this instance, we
		// free it ourselves.
		if (pTileSurf->pAuxData != NULL)
		{
			MemFree( pTileSurf->pAuxData );
		}
	}

	DeleteVideoObject(pTileSurf->vo);
	MemFree( pTileSurf );
}


void SetRaisedObjectFlag(const char* cFilename, TILE_IMAGERY* pTileSurf)
{
	static const char RaisedObjectFiles[][9] =
	{
		"bones",
		"bones2",
		"grass2",
		"grass3",
		"l_weed3",
		"litter",
		"miniweed",
		"sblast",
		"sweeds",
		"twigs",
		"wing"
	};

	// Loop through array of RAISED objecttype imagery and set global value...
	if ((pTileSurf->fType >= DEBRISWOOD && pTileSurf->fType <= DEBRISWEEDS) || pTileSurf->fType == DEBRIS2MISC || pTileSurf->fType == ANOTHERDEBRIS)
	{
		char cRootFile[128];
		GetRootName(cRootFile, cFilename);
		for (UINT32 i = 0; i != lengthof(RaisedObjectFiles); i++)
		{
			if (strcasecmp(RaisedObjectFiles[i], cRootFile) == 0)
			{
				pTileSurf->bRaisedObjectType = TRUE;
				return;
			}
		}
	}
}