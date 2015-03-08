#include "img.h"
#define SMALL unsigned short

void pbm_image_free(PbmImage* img) {
	free(img->data);
	free(img);
}

PbmImage* pbm_image_load_from_stream(FILE* stream, int* error) {
	PbmImage* result = (PbmImage*) malloc(sizeof(PbmImage));
	if (result == NULL) {
		printf("Error allocating memory.");
		exit(RET_OUT_OF_MEMORY);
	}

	unsigned long lengthOfStream;
	fseek(stream, 0, SEEK_END);
	lengthOfStream = ftell(stream);
	fseek(stream, 0, SEEK_SET);

	char* allData = (char*) calloc(lengthOfStream + 1, 1);
	if (allData == NULL) {
		printf("Error allocating memory.");
		exit(RET_OUT_OF_MEMORY);
	}

	fread(allData, lengthOfStream, 1, stream);

	SMALL lineCounter = 1;

	SMALL magicNumberCharCounter = 0;
	SMALL isStartOfLine = 0;
	SMALL isCommentLine = 0;
	SMALL isSizeLine = 1;
	SMALL isIntensityLine = 1;
	SMALL isDataLine = 1;

	for (int i = 0; i < lengthOfStream; i++) {

		if (lineCounter == 1) { // magic number line
			result->type[magicNumberCharCounter] = allData[i];
			magicNumberCharCounter++;
		}

		if (isStartOfLine && allData[i] == PBM_COMMENT_CHAR) { // check if comment line
			isCommentLine = 1;
		}

		if (!isCommentLine && lineCounter != 1) {
			if (isSizeLine) {
				// read size
				SMALL j = i;

				char widthHeightLine[10]; // size limited to 99999x9999 or similar
				memset(widthHeightLine, 0, 10);
				SMALL widthHeightLineCounter = 0;

				while (allData[j] != '\x0A') {
					widthHeightLine[widthHeightLineCounter] = allData[j];
					widthHeightLineCounter++;
					j++;
				}
				widthHeightLineCounter++;
				widthHeightLine[widthHeightLineCounter] = '\0';

				// find index of ' '
				char *res = strstr(widthHeightLine, " ");
				int sizeOfWidth = res - widthHeightLine; // position of space
				int sizeOfHeight = widthHeightLineCounter - sizeOfWidth - 1;

				char width[sizeOfWidth * sizeof(int)];
				memset(width, 0, sizeOfWidth * sizeof(int));
				char height[sizeOfHeight * sizeof(int)];
				memset(height, 0, sizeOfHeight * sizeof(int));

				memcpy(width, widthHeightLine, sizeOfWidth);
				memcpy(height, &widthHeightLine[sizeOfWidth + 1], sizeOfHeight);

				result->width = atoi(width);
				result->height = atoi(height);

				isSizeLine = 0;
				i = j;
			} else if (isIntensityLine) {
				// read intensity
				SMALL j = i;

				char intensityLine[10];
				SMALL intensityLineCounter = 0;

				while (allData[j] != '\x0A') {
					intensityLine[intensityLineCounter] = allData[j];
					intensityLineCounter++;
					j++;
				}
				intensityLine[intensityLineCounter] = 'n';

				char *res = strstr(intensityLine, "n");
				int pos = res - intensityLine;
				char intensity[pos * sizeof(int)];
				memset(intensity, 0, pos * sizeof(int));

				memcpy(intensity, intensityLine, pos);

				int intensityValue = atoi(intensity);

				if (intensityValue != 255) {
					*error = RET_INVALID_FORMAT;
				}

				isIntensityLine = 0;
				i = j;
			} else if (isDataLine) {
				// read data

				int dataSize = (result->height * result->width);
				char* dataLine = (char*) malloc(dataSize);

				memcpy(dataLine, &allData[i], dataSize);

				result->data = dataLine;

				isDataLine = 0;
			}
		}

		if (allData[i] == '\x0A') {
			lineCounter++;
			isStartOfLine = 1;
			isCommentLine = 0;
		}

	}

	free(allData);

	if (NULL == strstr(result->type, PBM_TYPE_P5)) {
		exit(RET_UNSUPPORTED_FILE_FORMAT);
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
