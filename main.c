#include "flip.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("First argument must be filename.");
		return 0;
	}

	FILE* fp = fopen(argv[1], "rb");

	int* err = (int*) malloc(1);
	*err = 0;
	PbmImage* image = pbm_image_load_from_stream(fp, err);
	if (*err != 0) {
		return *err;
	}
	fclose(fp);

	pbm_image_flip(image);

	FILE* outFile = fopen("flipped.pgm", "wb");
	pbm_image_write_to_stream(image, outFile);
	fclose(outFile);

	pbm_image_free(image);

	return 0;
}
