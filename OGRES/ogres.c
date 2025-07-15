#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "ogres.h"

BOOL 
OGRES_Load(ogres_t *ogres,
		   const char *path)
{
	FILE *file = fopen(path, "rb");

	if (!file) {
		fprintf(stderr, "Error: Could not open file:\n%s\n", path);
		return FALSE;
	}
	struct ogres_info_header header;
	printf("File %s opened.\n", path);
	fread(header.name, 5, 1, file);

	if (header.name[0] != 'O'
	 || header.name[1] != 'G'
	 || header.name[2] != 'R'
	 || header.name[3] != 'E'
	 || header.name[4] != 'S')
	{
		fprintf(stderr, "Error: Invalid or corrupted file format.\n");
		fclose(file);
		return FALSE;
	}
	fread(&header.layer_count, sizeof(WORD), 1, file);
	fread(&header.sz_image, sizeof(DWORD), 1, file);

	ogres->layer_count = header.layer_count;
	ogres->image_layers = (BYTE*)malloc(header.sz_image);
	if (!ogres->image_layers) {
		fprintf(stderr, "Error: Couldn't allocate memory.\n");
		fclose(file);
		return FALSE;
	}
	if (fread(ogres->image_layers, 1, header.sz_image, file) != header.sz_image) {
		fprintf(stderr, "Error: Loading images failed.\n");
		free(ogres->image_layers);
		fclose(file);
		return FALSE;
	}

	fclose(file);
	return TRUE;
}

BOOL 
OGRES_GetImage(ogres_t *ogres, 
			   image_t *image, 
			   WORD id)
{
	if (id >= ogres->layer_count) {
		fprintf(stderr, "Error: Couldn't find resource at the specified id.\n");
		return FALSE;
	}

	BYTE *cursor = ogres->image_layers;
	DWORD image_size;
	image_size = *((WORD*)(cursor + sizeof(WORD) * 2)); // size of the pixel data and header

	for (int i = 0; i < id; i++) {
		cursor += image_size;
		image_size = *((WORD*)(cursor + sizeof(WORD) * 2));
	}
	image->width = *((WORD*)(cursor));
	image->height = *((WORD*)(cursor + sizeof(WORD)));
	if (!(image->pixels = (pixel_t *)malloc(image->width * image->height * sizeof(pixel_t)))) {
		fprintf(stderr, "Error: Image data exceeds buffer size.\n");
		return FALSE;
	}
	memcpy(image->pixels, cursor + sizeof(WORD) * 3, image->width * image->height * sizeof(pixel_t));
	

	return TRUE;
}

void 
OGRES_Free(ogres_t *ogres)
{
	if (ogres && ogres->image_layers) {
		free(ogres->image_layers);
		ogres->image_layers = NULL;
	}
}

void 
OGRES_FreeImage(image_t *image)
{
	if (image && image->pixels) {
		free(image->pixels);
		image->pixels = NULL;
	}
}