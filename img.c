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

	unsigned short lineCounter = 1;

	unsigned short magicNumberCharCounter = 0;

	for (int i = 0; i < length; i++) {

		if (lineCounter == 1) { // magic number line
			result->type[magicNumberCharCounter] = allData[i];
			magicNumberCharCounter++;
		}

		printf("%c", allData[i]);

		if (allData[i] == '\x0A') {
			lineCounter++;
		}
	}

	return result;
}

int pbm_image_write_to_stream(PbmImage* img, FILE* targetStream) {
	return 0;
}
