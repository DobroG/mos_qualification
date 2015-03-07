#include "flip.h"

int pbm_image_flip(PbmImage* image){
	return 0;
}

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("First argument must be filename.");
		return 0;
	}

	FILE* fp = fopen(argv[1], "rb");
	
	PbmImage* image = pbm_image_load_from_stream(fp, 0); // TODO: check error
	printf("Image should be loaded.");

	
	fclose(fp);

	return 0;

}
