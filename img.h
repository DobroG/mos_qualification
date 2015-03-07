#ifndef IMG_H_
#define IMG_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 *  @brief In memory representation of an PBM image
 */
typedef struct {
	//type of the PBM file as 0 terminated string (P1-P6) you only need to support P5 (binary greymaps)
	char type[3];
	//width of the image
	int width;
	//height of the image
	int height;
	//image data, representation is dependent on the format of the image. 
	char* data;
} PbmImage;

//the only PBM type we currently support
#define PBM_TYPE_P5 "P5"
//start character that indicates a comment in PBM
#define PBM_COMMENT_CHAR '#'

//Indicates that the write operation was a success
#define RET_PBM_OK 0
//Indicates an unspecified write error
#define RET_PBM_ERROR 1
//Premature end of file
#define RET_EOF 2
//Indicates an unsupported file PBM file format
#define RET_UNSUPPORTED_FILE_FORMAT 3
//Indicates an invalid file format
#define RET_INVALID_FORMAT 4
//a (heap) memory allocation failed
#define RET_OUT_OF_MEMORY 5

/**
 * @brief Frees the memory allocated by the Pbm structure and its members.
 * @param img The image of which the memory should be freed. If img is NULL
 * no operation is performed.
 */
void pbm_image_free(PbmImage* img);

/**
 * @brief Loads a PBM file from the given stream.
 * @error Error code that is set by passing a reference can be (RET_EOF, RET_PBM_OK, RET_UNSUPPORTED_FILE_FORMAT, RET_INVALID_FORMAT)
 * @return A fully initilized PbmImage instance or NULL if an error occured. For further error details the error variable should be checked
 */
PbmImage* pbm_image_load_from_stream(FILE* stream, int* error);

/** 
 * @brief Writes a PbmImage structure to the given stream.
 * @param img the image structure that should be written.
 * @param targetStream to stream to which the image should be written.
 * @return  RET_PBM_OK if the write was successfull or RET_PBM_ERROR if an error occured.
 */
int pbm_image_write_to_stream(PbmImage* img, FILE* targetStream);

#endif
