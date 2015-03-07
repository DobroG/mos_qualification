#include "img.h"

void pbm_image_free(PbmImage* img) {
	free(img->data);
	free(img);
}

PbmImage* pbm_image_load_from_stream(FILE* stream, int* error) {
	PbmImage* result = (PbmImage*) malloc(sizeof(PbmImage));

	unsigned long length;
	fseek(stream, 0, SEEK_END);
	length = ftell(stream);
	fseek(stream, 0, SEEK_SET);

	char* allData = (char*) malloc(length + 1);
	fread(allData, length, 1, stream);

 	return result;
}

int pbm_image_write_to_stream(PbmImage* img, FILE* targetStream) {
	return 0;
}
