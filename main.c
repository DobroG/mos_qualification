#include "flip.h"

int main(int argc, char* argv[]) {
	int retval = RET_PBM_OK;

	if (argc < 3) {
		printf(
				"First argument must be source filename.\nSecond must be destination filename.\n");
		return retval;
	}

	FILE* inFile = fopen(argv[1], "rb");
	if (inFile == NULL) {
		fputs("File error\n", stderr);
		exit(RET_PBM_ERROR);
	}

	int* err = (int*) malloc(sizeof(int));
	*err = 0;
	PbmImage* image = pbm_image_load_from_stream(inFile, err);
	if (*err != RET_PBM_OK) {
		fclose(inFile);
		retval = *err;
		free(err);
		return retval;
	} else {
		free(err);
		fclose(inFile);
	}

	pbm_image_flip(image);

	FILE* outFile = fopen(argv[2], "wb");
	if (outFile == NULL) {
		fputs("File error\n", stderr);
		exit(RET_PBM_ERROR);
	}

	pbm_image_write_to_stream(image, outFile);
	fclose(outFile);

	pbm_image_free(image);

	return retval;
}
