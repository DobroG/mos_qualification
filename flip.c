#include "flip.h"

int pbm_image_flip(PbmImage* image) {
	int dataSize = image->width * image->height;

	int front = 0;
	char tmp;
	int back = dataSize - 1;
	while (back >= dataSize / 2) {
		tmp = image->data[back];
		image->data[back] = image->data[front];
		image->data[front] = tmp;
		front++;
		back--;
	}

	return RET_PBM_OK;
}

