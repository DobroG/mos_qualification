#include "img.h"

#define SMALL unsigned short

void pbm_image_free(PbmImage* img) {
	free(img->data);
	free(img);
}

int validateMagicNumber(char* magicNumber) {
	if (magicNumber[0] != 'P') {
		return RET_INVALID_FORMAT;
	}

	if (magicNumber[1] != '1' && magicNumber[1] != '2' && magicNumber[1] != '3'
			&& magicNumber[1] != '4' && magicNumber[1] != '5'
			&& magicNumber[1] != '6') {
		return RET_UNSUPPORTED_FILE_FORMAT;
	}

	return RET_PBM_OK;
}

PbmImage* pbm_image_load_from_stream(FILE* stream, int* error) {
	PbmImage* result = (PbmImage*) malloc(sizeof(PbmImage));
	if (result == NULL) {
		printf("Error allocating memory.");
		exit(RET_OUT_OF_MEMORY);
	}

	bool isEndOfLine = false;
	bool isCommentLine = false;

	int state = STATE_READING_MAGIC_NUMBER;
	unsigned char currentChar;

	int tmpMagicNumberCounter = 0;

	char tmpSize[5];
	memset(tmpSize, 0, 5);
	int tmpSizeCounter = 0;

	while (!feof(stream)) {
		fread(&currentChar, 1, 1, stream);

		if (currentChar == '\n') {
			isEndOfLine = true;
		} else if (currentChar == '#') {
			isCommentLine = true;
		}

		switch (state) {
		case STATE_READING_MAGIC_NUMBER:
			if (isEndOfLine == false) {
				result->type[tmpMagicNumberCounter] = currentChar;
				tmpMagicNumberCounter++;
			} else {
				result->type[tmpMagicNumberCounter] = '\n';
				*error = validateMagicNumber(result->type);
				state = STATE_READING_COMMENT_LINE;
			}

			break;
		case STATE_READING_COMMENT_LINE:
			if (isCommentLine && isEndOfLine) {
				isCommentLine = false;
			} else if (isCommentLine == false) {
				state = STATE_READING_SIZE_W;
				// hit the first char of width
				tmpSize[tmpSizeCounter] = currentChar;
				tmpSizeCounter++;
			}

			break;
		case STATE_READING_SIZE_W:
			if (currentChar != ' ') {
				tmpSize[tmpSizeCounter] = currentChar;
				tmpSizeCounter++;
			} else {
				char croppedSize[tmpSizeCounter + 1];
				memcpy(croppedSize, tmpSize, tmpSizeCounter + 1);
				sscanf(croppedSize, "%d", &result->width);

				state = STATE_READING_SIZE_H;
				memset(tmpSize, 0, 5);
				tmpSizeCounter = 0;
			}

			break;
		case STATE_READING_SIZE_H:
			if (currentChar != '\n') {
				tmpSize[tmpSizeCounter] = currentChar;
				tmpSizeCounter++;
			} else {
				char croppedSize[tmpSizeCounter + 1];
				memcpy(croppedSize, tmpSize, tmpSizeCounter + 1);
				sscanf(croppedSize, "%d", &result->height);

				// we know the size of the data now, allocate memory
				result->data = calloc(result->width * result->height, 1);

				state = STATE_READING_INTENSITY;
				memset(tmpSize, 0, 5);
				tmpSizeCounter = 0;
			}

			break;
		case STATE_READING_INTENSITY:
			// we skip the intensity as no other types than P5 are supported
			if (currentChar == '\n') {
				state = STATE_READING_DATA;
			}

			break;
		case STATE_READING_DATA:
			result->data[tmpSizeCounter] = currentChar;
			tmpSizeCounter++;

			break;
		default:
			exit(RET_PBM_ERROR);

			break;
		}

		// end
		isEndOfLine = false;
	}

	return result;
}

int count_number_of_digits(int n) {
	int count = 0;
	while (n != 0) {
		n /= 10;
		count++;
	}

	return count;
}

int pbm_image_write_to_stream(PbmImage* img, FILE* targetStream) {
	unsigned int offset = 0;

	int widthDigits = count_number_of_digits(img->width);
	// GET WIDTH
	char dimensions_w[widthDigits * sizeof(int)];
	snprintf(dimensions_w, sizeof(dimensions_w), "%d", img->width);

	int heightDigits = count_number_of_digits(img->height);
	// GET HEIGHT
	char dimensions_h[heightDigits * sizeof(int)];
	snprintf(dimensions_h, sizeof(dimensions_h), "%d", img->height);

	int imgDataSize = img->width * img->height;
	// magicNumber + widthDigits + space + heightDigits + line break + opacity  + data
	int sizeOfData = 3 + 18 + widthDigits + 1 + heightDigits + 1 + 4
			+ imgDataSize;

	char* dataToWrite = (char*) calloc(sizeOfData, 1);

	// put magic number into data
	memcpy(dataToWrite, img->type, 3);
	offset += 3;

	// power by
	char powerdBy[] = "#powered by IMS14\n";
	memcpy(&dataToWrite[offset], powerdBy, 18);
	offset += 18;

	// put dimensions into data
	memcpy(&dataToWrite[offset], dimensions_w, widthDigits);
	offset += widthDigits;

	dataToWrite[offset] = ' ';
	offset += 1;

	memcpy(&dataToWrite[offset], dimensions_h, heightDigits);
	offset += heightDigits;

	dataToWrite[offset] = '\n';
	offset += 1;

	// put opacity
	char opacity[4] = { '2', '5', '5', '\n' };
	memcpy(&dataToWrite[offset], opacity, 4);
	offset += 4;

	// put data into data
	memcpy(&dataToWrite[offset], img->data, imgDataSize);

	fwrite(dataToWrite, 1, sizeOfData, targetStream);

	free(dataToWrite);

	return RET_PBM_OK;
}
