#ifndef __TYPES_
#define __TYPES_

#include "SGPStrings.h"


#if defined DUTCH + defined ENGLISH + defined FRENCH + defined GERMAN + defined ITALIAN + defined POLISH + defined RUSSIAN + defined RUSSIAN_GOLD != 1
#	error Exactly one of DUTCH, ENGLISH, FRENCH, GERMAN, ITALIAN, POLISH, RUSSIAN or RUSSIAN_GOLD must be defined.
#endif


#include <stdlib.h> // for abort()
#define UNIMPLEMENTED \
	fprintf(stderr, "===> %s:%d: %s() is not implemented\n", __FILE__, __LINE__, __func__); \
	abort();

#ifdef WITH_FIXMES
#	define FIXME \
	fprintf(stderr, "===> %s:%d: %s() FIXME\n", __FILE__, __LINE__, __func__);
#else
#	define FIXME (void)0;
#endif


#if defined(_WIN32) && !defined(_WIN64) // XXX HACK000A
#	define CASSERT(x)
#else
#	define CASSERT(x) extern int ASSERT_COMPILE[((x) != 0) * 2 - 1];
#endif


#define lengthof(a) (sizeof(a) / sizeof(a[0]))
#define endof(a) ((a) + lengthof(a))


#define __max(a, b) ((a) > (b) ? (a) : (b))
#define __min(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) __max(a, b)
#define MIN(a, b) __min(a, b)


typedef          int  INT;
typedef unsigned int UINT;
typedef unsigned int UINT32;
typedef   signed int  INT32;

// integers
typedef unsigned char   UINT8;
typedef signed char     INT8;
typedef unsigned short  UINT16;
typedef signed short    INT16;
// floats
typedef float           FLOAT;
typedef double          DOUBLE;
// strings
typedef char			      CHAR8;
typedef wchar_t					CHAR16;
// other
typedef unsigned char		BOOLEAN;
typedef void *					PTR;
typedef UINT8						BYTE;
typedef CHAR8						STRING512[512];

#define SGPFILENAME_LEN 100
typedef CHAR8 SGPFILENAME[SGPFILENAME_LEN];


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define BAD_INDEX -1

#define PI 3.1415926

#ifndef NULL
#define NULL 0
#endif

typedef struct SGPBox
{
	INT16 x;
	INT16 y;
	INT16 w;
	INT16 h;
} SGPBox;

typedef struct
{
  INT32 iLeft;
  INT32 iTop;
  INT32 iRight;
  INT32 iBottom;

} SGPRect;

typedef struct
{
	INT32 	iX;
	INT32   iY;

} SGPPoint;


typedef struct SGPPaletteEntry
{
	UINT8 peRed;
	UINT8 peGreen;
	UINT8 peBlue;
	UINT8 peFlags;
} SGPPaletteEntry;


typedef UINT32 COLORVAL;

typedef struct AuxObjectData   AuxObjectData;
typedef struct ETRLEObject     ETRLEObject;
typedef struct RelTileLoc      RelTileLoc;

typedef struct SGPImage    SGPImage;
typedef SGPImage*          HIMAGE;

#ifdef __cplusplus
class SGPVObject;
typedef SGPVObject* HVOBJECT;

typedef SGPVObject* Font;

class SGPVSurface;
#endif

typedef struct BUTTON_PICS BUTTON_PICS;

typedef struct SGPFile SGPFile;
typedef SGPFile*       HWFILE;


#define TRANSPARENT ((UINT16)0)


#ifdef __cplusplus
#	define ENUM_BITSET(type)                                                                   \
		static inline type operator ~  (type  a)         { return     (type)~(int)a;           } \
		static inline type operator &  (type  a, type b) { return     (type)((int)a & (int)b); } \
		static inline type operator &= (type& a, type b) { return a = (type)((int)a & (int)b); } \
		static inline type operator |  (type  a, type b) { return     (type)((int)a | (int)b); } \
		static inline type operator |= (type& a, type b) { return a = (type)((int)a | (int)b); }
#else
#	define ENUM_BITSET(type)
#endif

#endif
