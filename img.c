#include "img.h"

void pbm_image_free(PbmImage* img){
	printf("Implement me");
}

PbmImage* pbm_image_load_from_stream(FILE* stream, int* error){
	fseek(stream, 0, SEEK_END);
	rewind(stream);
	return 0;
}

int pbm_image_write_to_stream(PbmImage* img, FILE* targetStream){
	return 0;
}
