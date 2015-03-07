#ifndef FLIP_H
#define FLIP_H
#include "img.h"
/**
* @brief Flips the image, i.e. the buttom right pixel will be the top left pixel..
* @param image Image this is fliped.
* @return  Error code indiciates success (RET_PBM_OK) or failure (RET_PBM_ERROR or RET_UNSUPPORTED_FILE_FORMAT)  
*/
int pbm_image_flip(PbmImage* image);
#endif
