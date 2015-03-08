#include "flip.h"

int main(int argc, char* argv[]) {
	int retval = 0;

	if (argc < 3) {
		printf(
				"First argument must be source filename.\nSecond must be destination filename.\n");
		return retval;
	}

	FILE* fp = fopen(argv[1], "rb");

	int* err = (int*) malloc(sizeof(int));
	*err = 0;
	PbmImage* image = pbm_image_load_from_stream(fp, err);
	if (*err != 0) {
		retval = *err;
	}
	free(err);
	fclose(fp);

	if (retval != 0) {
		return 1;
	}

	pbm_image_flip(image);

	FILE* outFile = fopen(argv[2], "wb");
	pbm_image_write_to_stream(image, outFile);
	fclose(outFile);

	pbm_image_free(image);

	return retval;
}
