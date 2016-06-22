
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "ImageReconstruct.h"



int ImReconstruct( IplImage* oMarkerImage, IplImage* oMaskImage)
//-------------------------------------------------------
{
	if ( oMarkerImage->depth == IPL_DEPTH_8U && oMaskImage->depth == IPL_DEPTH_8U )
	{
		int         x,y;

		//int         cnc=4;
		XY          xy;
		int         nWidth = oMaskImage->width;
		int         nHeight = oMaskImage->height;

		int         idx[4][2]={{-1,0},{0,-1},{-1,-1},{1,-1}};
		CvPoint     cvPointOffset;
		CvScalar    cvScalar;
		int nOffset = 4;
		cvPointOffset.x = nOffset;
		cvPointOffset.y = nOffset;
		cvScalar.val[0] = 0;
		cvScalar.val[1] = 0;
		cvScalar.val[2] = 0;
		cvScalar.val[3] = 0;

		//   CvMat *oTempMarkerImage      =  cvCreateMat(nHeight +2*nOffset, nWidth+2*nOffset, CV_8UC1  );
		IplImage* oTempMarkerImage      =  cvCreateImage(cvSize(nWidth+2*nOffset, nHeight +2*nOffset), IPL_DEPTH_8U , 1);      
		cvCopyMakeBorder( oMarkerImage, oTempMarkerImage, cvPointOffset, IPL_BORDER_CONSTANT, cvScalar);

		//   CvMat *oTempMaskImage      =  cvCreateMat(nHeight +2*nOffset, nWidth+2*nOffset, CV_8UC1  );
		IplImage* oTempMaskImage      =  cvCreateImage(cvSize(nWidth+2*nOffset, nHeight +2*nOffset), IPL_DEPTH_8U , 1);      
		cvCopyMakeBorder( oMaskImage, oTempMaskImage, cvPointOffset, IPL_BORDER_CONSTANT, cvScalar);



		CvSize size;
		int    step = 0;
		unsigned char *oTempMarker = NULL;
		unsigned char *oTempMask = NULL;

		cvGetRawData( oTempMarkerImage, &oTempMarker, &step, &size );
		int    nWidthTemp = size.width - nOffset;
		unsigned char* pcMarkerImgStart = oTempMarker + nOffset*step;
		unsigned char* pcMarkerImgEnd = oTempMarker + (size.height-nOffset)*step ;



		cvGetRawData( oTempMaskImage, &oTempMask, &step, &size );


		unsigned char* pcMaskImgStart = oTempMask + nOffset*step;
		unsigned char* pcMaskImgEnd = oTempMask + (size.height-nOffset)*step ;
		unsigned char* pPixelMarker;
		unsigned char* pPixelMask;
		unsigned char      cPixelTemp;
		XYQueueC    xyQueue(size.height * size.width);

		for( pPixelMarker = pcMarkerImgStart, pPixelMask = pcMaskImgStart;
			pPixelMarker < pcMarkerImgEnd;
			pPixelMarker += step , pPixelMask += step)
		{
			int x;     
			for( x = nOffset; x < nWidthTemp; x++ )
			{

				cPixelTemp = pPixelMarker[x];
				if(cPixelTemp < pPixelMarker[x - 1])
					cPixelTemp = pPixelMarker[x - 1];
				if(cPixelTemp < pPixelMarker[x-step-1])
					cPixelTemp = pPixelMarker[x-step-1];
				if(cPixelTemp < pPixelMarker[x-step])
					cPixelTemp = pPixelMarker[x-step];
				if(cPixelTemp < pPixelMarker[x-step+1])
					cPixelTemp = pPixelMarker[x-step+1];

				if( cPixelTemp > pPixelMask[x]) 
				{
					pPixelMarker[x] = pPixelMask[x] ;
				} else 
				{
					pPixelMarker[x] = cPixelTemp;
				}
			}
		}


		int nY = size.height-nOffset;
		for( pPixelMarker = pcMarkerImgEnd , pPixelMask = pcMaskImgEnd  ;
			pPixelMarker > pcMarkerImgStart ;
			pPixelMarker -= step , pPixelMask -= step)
		{
			int x; 

			int nX = nWidthTemp;
			for( x = nWidthTemp; x > 1; x-- )
			{

				cPixelTemp = pPixelMarker[x];
				if(cPixelTemp < pPixelMarker[x + 1])
					cPixelTemp = pPixelMarker[x + 1];
				if(cPixelTemp < pPixelMarker[x+step])
					cPixelTemp = pPixelMarker[x+step];
				if(cPixelTemp < pPixelMarker[x+step+1])
					cPixelTemp = pPixelMarker[x+step+1];
				if(cPixelTemp < pPixelMarker[x+step-1])
					cPixelTemp = pPixelMarker[x+step-1];

				if( cPixelTemp > pPixelMask[x]) 
				{
					pPixelMarker[x] = pPixelMask[x] ;
				} else 
				{
					pPixelMarker[x] = cPixelTemp;
				}
				// fill queue with points which could be changed
				// in next raster scan
				cPixelTemp = pPixelMarker[x];
				if(pPixelMarker[x + 1] < cPixelTemp && pPixelMarker[x + 1] < pPixelMask[x + 1])
				{
					xyQueue.Put(nX,nY);
				}                   
				else if(pPixelMarker[x+step+1] < cPixelTemp && pPixelMarker[x+step+1] < pPixelMask[x+step+1])
				{
					xyQueue.Put(nX,nY);
				}
				else if(pPixelMarker[x+step]< cPixelTemp && pPixelMarker[x+step] < pPixelMask[x+step])
				{
					xyQueue.Put(nX,nY);
				}
				else if(pPixelMarker[x+step-1] < cPixelTemp && pPixelMarker[x+step-1] < pPixelMask[x+step-1])
				{
					xyQueue.Put(nX,nY);
				}

				nX--;   
			}
			nY--;
		}



		while(!xyQueue.Empty()) {
			xy = xyQueue.Get();
			x = xy.x;
			y = xy.y;
			pPixelMarker = oTempMarker + y*step;
			pPixelMask   = oTempMask + y*step;


			PropagationStep(pPixelMarker, pPixelMask , x, y, -1,  0, step, xyQueue);
			PropagationStep(pPixelMarker, pPixelMask , x, y, +1,  0, step, xyQueue);

			PropagationStep(pPixelMarker, pPixelMask , x, y,  0, -1, step, xyQueue);
			PropagationStep(pPixelMarker, pPixelMask , x, y,  0, +1, step, xyQueue);

			PropagationStep(pPixelMarker, pPixelMask , x, y, -1, -1, step, xyQueue);
			PropagationStep(pPixelMarker, pPixelMask , x, y, +1, +1, step, xyQueue);

			PropagationStep(pPixelMarker, pPixelMask , x, y,  1, -1, step, xyQueue);
			PropagationStep(pPixelMarker, pPixelMask , x, y, -1, +1, step, xyQueue);

		}



		CvRect oCvRect;
		oCvRect.x = nOffset; 
		oCvRect.y = nOffset; 

		oCvRect.width = nWidth; 
		oCvRect.height = nHeight; 
		cvSetImageROI( oTempMarkerImage, oCvRect );
		cvCopy( oTempMarkerImage, oMarkerImage, NULL );



		cvReleaseImage(&oTempMarkerImage);  
		cvReleaseImage(&oTempMaskImage);
		return 1;
	}



	else if( oMarkerImage->depth == IPL_DEPTH_16U && oMaskImage->depth == IPL_DEPTH_16U )
	{
		
		int         x,y;

		//int         cnc=4;
		XY          xy;
		int         nWidth = oMaskImage->width;
		int         nHeight = oMaskImage->height;
		

		int         idx[4][2]={{-1,0},{0,-1},{-1,-1},{1,-1}};
		CvPoint     cvPointOffset;
		
		int nOffset = 4;  
		cvPointOffset.x = nOffset;
		cvPointOffset.y = nOffset;
		
		//   CvMat *oTempMarkerImage      =  cvCreateMat(nHeight +2*nOffset, nWidth+2*nOffset, CV_8UC1  );
		IplImage* oTempMarkerImage      =  cvCreateImage(cvSize(nWidth+2*nOffset, nHeight +2*nOffset), IPL_DEPTH_16U , 1);      
		cvCopyMakeBorder( oMarkerImage, oTempMarkerImage, cvPointOffset, IPL_BORDER_CONSTANT, cvScalarAll(0));
		//IplImage* oTempMarkerImage = cvCloneImage( oMarkerImage);
		//   CvMat *oTempMaskImage      =  cvCreateMat(nHeight +2*nOffset, nWidth+2*nOffset, CV_8UC1  );
		IplImage* oTempMaskImage     =    cvCreateImage(cvSize(nWidth+2*nOffset, nHeight +2*nOffset), IPL_DEPTH_16U , 1);      
		cvCopyMakeBorder( oMaskImage, oTempMaskImage, cvPointOffset, IPL_BORDER_CONSTANT, cvScalarAll(0));
		// IplImage* oTempMaskImage = cvCloneImage( oMaskImage);


		CvSize size;
		int    step = 0;
		unsigned short *oTempMarker = NULL;
		unsigned short *oTempMask = NULL;

		cvGetRawData( oTempMarkerImage,(uchar**)&oTempMarker, &step, &size );
		step /= sizeof(oTempMarker[0]);

		int    nWidthTemp = size.width - nOffset;
		unsigned short* pcMarkerImgStart = oTempMarker + nOffset*step;
		//unsigned short* pcMarkerImgEnd =   oTempMarker + (size.height-nOffset)*step ;
		unsigned short* pcMarkerImgEnd = (unsigned short*)((char*)oTempMarker + oTempMarkerImage->imageSize) - nOffset*step  ;



		cvGetRawData( oTempMaskImage, (uchar**)&oTempMask, &step, &size );
		step /= sizeof(oTempMask[0]);
		
		unsigned short* pcMaskImgStart = oTempMask + nOffset*step;
		//unsigned short* pcMaskImgEnd = (char*)oTempMask + (size.height-2* nOffset)*step ;
		unsigned short* pcMaskImgEnd = (unsigned short*)((char*)oTempMask + oTempMaskImage->imageSize) - nOffset*step  ;
		unsigned short* pPixelMarker;
		unsigned short* pPixelMask;
		unsigned short    cPixelTemp;
		XYQueueC    xyQueue(size.height * size.width);

		
		for( pPixelMarker = pcMarkerImgStart, pPixelMask = pcMaskImgStart;
			pPixelMarker < pcMarkerImgEnd;
			pPixelMarker += step , pPixelMask += step)
		{
			int x;  
			
			for( x = nOffset; x < nWidthTemp; x += 1)
			{
				cPixelTemp = pPixelMarker[x];
				if(cPixelTemp < pPixelMarker[x - 1])
					cPixelTemp = pPixelMarker[x - 1];
				if(cPixelTemp < pPixelMarker[x-step-1])
					cPixelTemp = pPixelMarker[x-step-1];
				if(cPixelTemp < pPixelMarker[x-step])
					cPixelTemp = pPixelMarker[x-step];
				if(cPixelTemp < pPixelMarker[x-step+1])
					cPixelTemp = pPixelMarker[x-step+1];

				if( cPixelTemp > pPixelMask[x]) 
				{
					pPixelMarker[x] = pPixelMask[x] ;
				} else 
				{
					pPixelMarker[x] = cPixelTemp;
				}
			}
		}


		int nY = size.height-nOffset;
		
		for( pPixelMarker = pcMarkerImgEnd , pPixelMask = pcMaskImgEnd  ;
			pPixelMarker > pcMarkerImgStart ;
			pPixelMarker -= step , pPixelMask -= step)
		{
			int x; 

			int nX = nWidthTemp;
			for( x = nWidthTemp; x > 1; x -- )
			{

				cPixelTemp = pPixelMarker[x];
				if(cPixelTemp < pPixelMarker[x + 1])
					cPixelTemp = pPixelMarker[x + 1];
				if(cPixelTemp < pPixelMarker[x+step])
					cPixelTemp = pPixelMarker[x+step];
				if(cPixelTemp < pPixelMarker[x+step+1])
					cPixelTemp = pPixelMarker[x+step+1];
				if(cPixelTemp < pPixelMarker[x+step-1])
					cPixelTemp = pPixelMarker[x+step-1];

				if( cPixelTemp > pPixelMask[x]) 
				{					
					pPixelMarker[x] = pPixelMask[x] ;
				} else 
				{
					pPixelMarker[x] = cPixelTemp;
				}
				// fill queue with points which could be changed
				// in next raster scan
				cPixelTemp = pPixelMarker[x];
				if(pPixelMarker[x + 1] < cPixelTemp && pPixelMarker[x + 1] < pPixelMask[x + 1])
				{
					xyQueue.Put(nX,nY);
				}                   
				else if(pPixelMarker[x+step+1] < cPixelTemp && pPixelMarker[x+step+1] < pPixelMask[x+step+1])
				{
					xyQueue.Put(nX,nY);
				}
				else if(pPixelMarker[x+step]< cPixelTemp && pPixelMarker[x+step] < pPixelMask[x+step])
				{
					xyQueue.Put(nX,nY);
				}
				else if(pPixelMarker[x+step-1] < cPixelTemp && pPixelMarker[x+step-1] < pPixelMask[x+step-1])
				{
					xyQueue.Put(nX,nY);
				}

				nX--;   
			}
			nY--;
		}



		while(!xyQueue.Empty()) {
			xy = xyQueue.Get();
			x = xy.x;
			y = xy.y;
			pPixelMarker = (oTempMarker + y*step);
			pPixelMask   = (oTempMask + y*step);


			PropagationStep16(pPixelMarker, pPixelMask , x, y, -1,  0, step, xyQueue);
			PropagationStep16(pPixelMarker, pPixelMask , x, y, +1,  0, step, xyQueue);

			PropagationStep16(pPixelMarker, pPixelMask , x, y,  0, -1, step, xyQueue);
			PropagationStep16(pPixelMarker, pPixelMask , x, y,  0, +1, step, xyQueue);

			PropagationStep16(pPixelMarker, pPixelMask , x, y, -1, -1, step, xyQueue);
			PropagationStep16(pPixelMarker, pPixelMask , x, y, +1, +1, step, xyQueue);

			PropagationStep16(pPixelMarker, pPixelMask , x, y,  1, -1, step, xyQueue);
			PropagationStep16(pPixelMarker, pPixelMask , x, y, -1, +1, step, xyQueue);

		}



		CvRect oCvRect;
		oCvRect.x = nOffset; 
		oCvRect.y = nOffset; 

		oCvRect.width = nWidth; 
		oCvRect.height = nHeight; 
		cvSetImageROI( oTempMarkerImage, oCvRect );
		cvCopy( oTempMarkerImage, oMarkerImage, NULL );



		cvReleaseImage(&oTempMarkerImage);  
		cvReleaseImage(&oTempMaskImage);
		return 1;
	}
	else
	{
		return 0;
	}

}

void PropagationStep(unsigned char*pPixelMarker, unsigned char*pPixelMask ,int nX, int nY, int nOffsetX, int nOffsetY, int nStep , XYQueueC & xyQueue)
{
	if( pPixelMarker[nX + nOffsetX +(nStep * nOffsetY)] < pPixelMarker[nX])
	{
		if(pPixelMask[nX + nOffsetX +(nStep * nOffsetY)] != pPixelMarker[nX + nOffsetX +(nStep * nOffsetY)])
		{
			xyQueue.Put(nX + nOffsetX, nY + nOffsetY);
			if(pPixelMask[nX + nOffsetX +(nStep * nOffsetY)] < pPixelMarker[nX])
			{
				pPixelMarker[nX + nOffsetX +(nStep * nOffsetY)] = pPixelMask[nX + nOffsetX +(nStep * nOffsetY)];
			}
			else
			{
				pPixelMarker[nX + nOffsetX +(nStep * nOffsetY)] = pPixelMarker[nX];
			}
		}

	}
}

void PropagationStep16(unsigned short*pPixelMarker, unsigned short*pPixelMask ,int nX, int nY, int nOffsetX, int nOffsetY, int nStep , XYQueueC & xyQueue)
{
	if( ( pPixelMarker)[nX + nOffsetX +(nStep * nOffsetY)] < ( pPixelMarker)[nX])
	{
		if(  ( pPixelMask)[nX + nOffsetX +(nStep * nOffsetY)] != ( pPixelMarker)[nX + nOffsetX +(nStep * nOffsetY)])
		{

			xyQueue.Put(nX + nOffsetX, nY + nOffsetY);
			if(( pPixelMask)[nX + nOffsetX +(nStep * nOffsetY)] < ( pPixelMarker)[nX])
			{
				( pPixelMarker)[nX + nOffsetX +(nStep * nOffsetY)] = ( pPixelMask)[nX + nOffsetX +(nStep * nOffsetY)];
			}
			else
			{
				( pPixelMarker)[nX + nOffsetX +(nStep * nOffsetY)] = ( pPixelMarker)[nX];
			}
		}

	}
}

