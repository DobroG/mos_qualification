#include "flip.h"

int pbm_image_flip(PbmImage* image) {
	int width = image->width;
	int dataSize = width * image->height;

	int front = 0;
	char tmp;
	int j = dataSize - 1;
	while (j >= dataSize / 2) {
		tmp = image->data[j];
		image->data[j] = image->data[front];
		image->data[front] = tmp;
		front++;
		j--;
	}

	return 0;
}

