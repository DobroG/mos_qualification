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

	char* allData = (char*) malloc(length + 1);
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

				char* widthHeightLine = (char*) malloc(10); // size limited to 99999x9999 or similar
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

				char* width = (char*) malloc(sizeOfWidth);
				char* height = (char*) malloc(sizeOfHeight);

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
				char* intensity = (char*) malloc(pos);

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

	return result;
}

int pbm_image_write_to_stream(PbmImage* img, FILE* targetStream) {


	return 0;
}
