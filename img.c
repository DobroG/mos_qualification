#include "img.h"
#define SMALL unsigned short

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

	char* allData = (char*) calloc(length + 1, 1);
	fread(allData, length, 1, stream);

	SMALL lineCounter = 1;

	SMALL magicNumberCharCounter = 0;
	SMALL isStartOfLine = 0;
	SMALL isCommentLine = 0;
	SMALL isSizeLine = 1;
	SMALL isIntensityLine = 1;
	SMALL isDataLine = 1;

	for (int i = 0; i < length; i++) {

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

				char* widthHeightLine = (char*) calloc(10, 1); // size limited to 99999x9999 or similar
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

				char* width = (char*) calloc(sizeOfWidth, sizeof(int));
				char* height = (char*) calloc(sizeOfHeight, sizeof(int));

				for (int i = 0; i < sizeOfWidth; i++) {
					width[i] = widthHeightLine[i];
				}

				for (int i = 0; i < sizeOfHeight; i++) {
					height[i] = widthHeightLine[sizeOfWidth + i + 1];
				}

				int w = atoi(width);
				int h = atoi(height);

				result->width = w;
				result->height = h;

				free(width);
				free(height);
				free(widthHeightLine);
				isSizeLine = 0;
				i = j;
			} else if (isIntensityLine) {
				// read intensity
				SMALL j = i;

				char* intensityLine = (char*) malloc(10);
				SMALL intensityLineCounter = 0;

				while (allData[j] != '\x0A') {
					intensityLine[intensityLineCounter] = allData[j];
					intensityLineCounter++;
					j++;
				}
				intensityLine[intensityLineCounter] = 'n';

				char *res = strstr(intensityLine, "n");
				int pos = res - intensityLine;
				char* intensity = (char*) calloc(pos, sizeof(int));

				for (int i = 0; i < pos; i++) {
					intensity[i] = intensityLine[i];
				}

				int intensityValue = atoi(intensity);

				if (intensityValue != 255) {
					*error = RET_INVALID_FORMAT;
				}

				free(intensity);
				free(intensityLine);
				isIntensityLine = 0;
				i = j;
			} else if (isDataLine) {
				// read data
				SMALL j = i;

				int dataSize = (result->height * result->width);
				char* dataLine = (char*) malloc(dataSize);

				for (int i = 0; i < dataSize; i++) {
					dataLine[i] = allData[j];
					j++;
				}

				result->data = dataLine;

				isDataLine = 0;
				i = j;
			}
		}

		if (allData[i] == '\x0A') {
			lineCounter++;
			isStartOfLine = 1;
			isCommentLine = 0;
		}

	}

	free(allData);
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

	// magicNumber + widthDigits + space + heightDigits + line break + opacity ( + data)
	int imgDataSize = img->width * img->height;
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
	return 0;
}
