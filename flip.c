#include "flip.h"

int pbm_image_flip(PbmImage* image) {
	int width = image->width;
	int dataSize = width * image->height;
	int front;
	char tmp;
	for (int i = 0; i < dataSize; i = i + width) {
		front = i;
		int j = width - 1 + i;
		while (j - i >= width / 2) {
			tmp = image->data[j];
			image->data[j] = image->data[front];
			image->data[front] = tmp;
			front++;
			j--;
		}
	}

	return 0;
}

