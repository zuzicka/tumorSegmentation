/** Vision & Graphics Library, 2011 Wanda Benesova

	Image reconstruction.
*/

#ifndef _IMAGERECONSTRUCTION_H_
#define _IMAGERECONSTRUCTION_H_

#include <opencv2/opencv.hpp>
#include "Queue.h"


using namespace cv;

//void imageReconstruct(Mat &image, int strength = 20);
int ImReconstruct( IplImage* oMarkerImage, IplImage* oMaskImage);
void PropagationStep(unsigned char*pPixelMarker, unsigned char*pPixelMask ,int nX, int nY, int nOffsetX, int nOffsetY, int nStep , XYQueueC & xyQueue);
void PropagationStep16(unsigned short*pPixelMarker, unsigned short*pPixelMask ,int nX, int nY, int nOffsetX, int nOffsetY, int nStep , XYQueueC & xyQueue);

#endif/*_IMAGERECONSTRUCTION_H_*/
