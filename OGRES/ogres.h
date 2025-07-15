#ifndef OGRES_H
#define OGRES_H
#include <Windows.h>

struct ogres_info_header
{
	BYTE name[5];		// should be OGRES
	WORD layer_count;	// # of layers
	DWORD sz_image;		// # of bytes occupied by image data
};

//struct ogres_image_layer_header
//{
//	WORD width;
//	WORD height;
//	WORD sz_total;	// total # of bytes, including this header (sizeof QWORD) and the pixel data
//};

typedef struct pixel_t
{
	BYTE b;
	BYTE g;
	BYTE r;
} pixel_t;

typedef struct image_t
{
	WORD width;
	WORD height;
	pixel_t *pixels;
} image_t;

typedef struct ogres_t
{
	BYTE *image_layers; // first 8 bytes is metadata about the image, rest is image data->repeat for each layer
	WORD layer_count;
} ogres_t;

BOOL OGRES_Load(ogres_t *ogres, const char *path);
BOOL OGRES_GetImage(ogres_t *ogres, image_t *image, WORD id);

void OGRES_Free(ogres_t *ogres);
void OGRES_FreeImage(image_t *image);

#endif