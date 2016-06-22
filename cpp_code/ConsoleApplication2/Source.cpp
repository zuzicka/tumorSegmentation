#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "ImageReconstruct.h"

#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <iomanip>
#include <fstream>

using namespace std;
using namespace cv;


/* 
 * Set TUMOR = FALSE --> if you want to remove cranium 
 * 
 * Set TUMOR = TRUE --> if you want to segment tumor 
 *
 */
bool TUMOR = TRUE;
//bool TUMOR = FALSE;


/* the method for tumor segmentation and cranium removal */
Mat CalculateBrain(char *cSourcem_oImageName, char *tumorFileName, char *originalFileName,
				   list<float> *Tsensitivity, list<float> *Tspecificy,
				   list<float> *Tprecision, list<float> *Tpvn, list<float> *Taccurancy,
				   list<float> *Tdice, list<float> *Toverlap,
				   list<float> *GCsensitivity, list<float> *GCspecificy,
				   list<float> *GCprecision, list<float> *GCpvn, list<float> *GCaccurancy,
				   list<float> *GCdice, list<float> *GCoverlap,
				   int *countOfSubjects, int *countOfGraBCutSeg, double *timeT, double *timeGC, list<String> *Tzoznam, list<String> *GCzoznam);

/* the method calculate statistical values of tumor segmentation (segmentation of proposed method is compared to manual segmentations provided by the specialist) */
void compareResultsAndSegmentation(Mat seg, Mat hSeg, char *temName, char *aka, 
								   list<float> *sensitivity, list<float> *specificy,
								   list<float> *precision, list<float> *pvn, list<float> *accurancy, list<float> *dice, list<float> *overlap);

/* the method calculate average values of statistical methods of tumor segmentation */
void statis(list<float> *Tsensitivity, list<float> *Tspecificy,
			list<float> *Tprecision, list<float> *Tpvn, list<float> *Taccurancy,
			list<float> *Tdice, list<float> *Toverlap,
			list<float> *GCsensitivity, list<float> *GCspecificy,
			list<float> *GCprecision, list<float> *GCpvn, list<float> *GCaccurancy,
			list<float> *GCdice, list<float> *GCoverlap,
			int *countOfSubjects, int *countOfGraBCutSeg, list<String> *Tzoznam, list<String> *GCzoznam);


/* MAIN function */
int main( int argc, char** argv ){

	/* lists for results of statistical methods counted after thresholding (method without graph cut segmentation) */
	list<float> Tsensitivity;
	list<float> Tspecificy;
	list<float> Tprecision;
	list<float> Tpvn;
	list<float> Taccurancy;
	list<float> Tdice;
	list<float> Toverlap;

	/* lists for results of statistical methods counted after graph cut segmentation (method without graph cut segmentation) */
	list<float> GCsensitivity;
	list<float> GCspecificy;
	list<float> GCprecision;
	list<float> GCpvn;
	list<float> GCaccurancy;
	list<float> GCdice;
	list<float> GCoverlap;

	/* List of file names where method without graph cut segmentation worked (method does not work for all images) */
	list<String> Tzoznam;
	
	/* List of file names where method with graph cut segmentation worked (method does not work for all images) */
	list<String> GCzoznam;

	/* counters */ 
	int countOfAllSubjects = 0;
	int countOfSubjects = 0;
	int countOfGraBCutSeg = 0;

	/* variables for time evaluation */
	double timeT = 0;
	double timeGC = 0;


	/* initialization necessary for reading all images in folders */
	WIN32_FIND_DATA FindFileDataBrain;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	WIN32_FIND_DATA FindFileDataTumor;
	HANDLE sFind = INVALID_HANDLE_VALUE;

	WIN32_FIND_DATA FindFileDataOriginal;
	HANDLE oFind = INVALID_HANDLE_VALUE;
	
	/* find the first file in the folder */
	hFind = FindFirstFile(_T("C:\\Users\\Zuzana\\Documents\\Visual Studio 2012\\Projects\\ConsoleApplication2\\ConsoleApplication2\\brain\\brain*.png"), &FindFileDataBrain);
	if(TUMOR){
		sFind = FindFirstFile(_T("C:\\Users\\Zuzana\\Documents\\Visual Studio 2012\\Projects\\ConsoleApplication2\\ConsoleApplication2\\tumor\\tumor*.png"), &FindFileDataTumor);
		oFind = FindFirstFile(_T("C:\\Users\\Zuzana\\Documents\\Visual Studio 2012\\Projects\\ConsoleApplication2\\ConsoleApplication2\\original\\brain*.png"), &FindFileDataOriginal);
	}

	if (hFind == INVALID_HANDLE_VALUE || (TUMOR && sFind == INVALID_HANDLE_VALUE) || (TUMOR && oFind == INVALID_HANDLE_VALUE)) {
		printf ("Invalid file handle. Error is %u.\n", GetLastError());
	} else{
		countOfAllSubjects++;

		/* opening images */
		TCHAR* tmpBrainFileName = FindFileDataBrain.cFileName;
		char brainFileName[MAX_PATH];
		wcstombs(brainFileName, tmpBrainFileName, MAX_PATH);
		printf ("First file name is %s.\n", brainFileName);

		char tumorFileName[MAX_PATH];
		if(TUMOR){ //this file is necessary to load only when we want to segment tumor not remove cranium
			TCHAR* tmpTumorFileName = FindFileDataTumor.cFileName;
			wcstombs(tumorFileName, tmpTumorFileName, MAX_PATH);
			printf ("First file name is %s.\n", tumorFileName);
		}

		char originalFileName[MAX_PATH];
		if(TUMOR){ //this file is necessary to load only when we want to segment tumor not remove cranium
			TCHAR* tmpOriginalFileName = FindFileDataOriginal.cFileName;
			wcstombs(originalFileName, tmpOriginalFileName, MAX_PATH);
			printf ("First file name is %s.\n", originalFileName);
		}

		/* call the method for cranium removal or tumor segmentation */
		CalculateBrain(brainFileName, tumorFileName, originalFileName, &Tsensitivity, &Tspecificy,
			       &Tprecision, &Tpvn, &Taccurancy, &Tdice, &Toverlap,
				   &GCsensitivity, &GCspecificy,
				   &GCprecision, &GCpvn, &GCaccurancy, &GCdice, &GCoverlap,
				   &countOfSubjects, &countOfGraBCutSeg, &timeT, &timeGC, &Tzoznam, &GCzoznam);
		printf ("************************\n");
		waitKey(); //this only wait for button press (it is here to se results of steps)

		/* List all the other files in the folder */
		while (FindNextFile(hFind, &FindFileDataBrain) != 0 && (FindNextFile(sFind, &FindFileDataTumor) != 0 || !TUMOR) && (FindNextFile(oFind, &FindFileDataOriginal) != 0 || !TUMOR)) 
		{
			countOfAllSubjects++;

			TCHAR* tmpBrainFileName = FindFileDataBrain.cFileName;
			char brainFileName[MAX_PATH];
			wcstombs(brainFileName, tmpBrainFileName, MAX_PATH);
			printf ("Next file name is %s.\n", brainFileName);

			char tumorFileName[MAX_PATH];
			if(TUMOR){ //this file is necessary to load only when we want to segment tumor not remove cranium
				TCHAR* tmpTumorFileName = FindFileDataTumor.cFileName;
				wcstombs(tumorFileName, tmpTumorFileName, MAX_PATH);
				printf ("Next file name is %s.\n", tumorFileName);
			}
			
			char originalFileName[MAX_PATH];
			if(TUMOR){ //this file is necessary to load only when we want to segment tumor not remove cranium
				TCHAR* tmpOriginalFileName = FindFileDataOriginal.cFileName;
				wcstombs(originalFileName, tmpOriginalFileName, MAX_PATH);
				printf ("First file name is %s.\n", originalFileName);
			}

			/* call the method for cranium removal or tumor segmentation */
			CalculateBrain(brainFileName, tumorFileName, originalFileName, &Tsensitivity, &Tspecificy,
				   &Tprecision, &Tpvn, &Taccurancy, &Tdice, &Toverlap,
				   &GCsensitivity, &GCspecificy,
				   &GCprecision, &GCpvn, &GCaccurancy, &GCdice, &GCoverlap,
				   &countOfSubjects, &countOfGraBCutSeg, &timeT, &timeGC, &Tzoznam, &GCzoznam);
			printf ("************************\n");
			waitKey(); //this only wait for button press (it is here to se results of steps)
		}
		FindClose(hFind);

		/* call the method which calculate average values of statistical methods of tumor segmentation which were counted for all images in folder */
		if(TUMOR)statis(&Tsensitivity, &Tspecificy, &Tprecision, &Tpvn, &Taccurancy, &Tdice, &Toverlap,
			&GCsensitivity, &GCspecificy, &GCprecision, &GCpvn, &GCaccurancy, &GCdice, &GCoverlap,
			&countOfSubjects, &countOfGraBCutSeg, &Tzoznam, &GCzoznam);
	}

	cout << countOfSubjects << '\n';
	cout << countOfGraBCutSeg << '\n';
	
	/* average time measurements */
	cout << time << '\n';
	cout << (timeGC/countOfGraBCutSeg) << '\n';
	waitKey();

	return EXIT_SUCCESS;
}

/* the method for tumor segmentation and cranium removal */
Mat CalculateBrain(char *cSourcem_oImageName, char *tumorFileName, char *originalFileName,
				   list<float> *Tsensitivity, list<float> *Tspecificy,
				   list<float> *Tprecision, list<float> *Tpvn, list<float> *Taccurancy,
				   list<float> *Tdice, list<float> *Toverlap,
				   list<float> *GCsensitivity, list<float> *GCspecificy,
				   list<float> *GCprecision, list<float> *GCpvn, list<float> *GCaccurancy,
				   list<float> *GCdice, list<float> *GCoverlap,
				   int *countOfSubjects, int *countOfGraBCutSeg, double *timeT,double *timeGC, list<String> *Tzoznam, list<String> *GCzoznam){	
	
	
	int SizeStructElem  = 7;
	Mat StructElem = getStructuringElement(MORPH_ELLIPSE, Size(SizeStructElem,SizeStructElem));
	Mat StructElem3 = getStructuringElement(MORPH_ELLIPSE, Size(3,3));

	char fileName[1000];
	sprintf(fileName, "brain/%s",cSourcem_oImageName);
	cv::Mat  InpImg3C= imread(fileName);

	cv::Mat InpImg3Ccopy =InpImg3C.clone();
	cv::Mat TempImage = InpImg3C.clone();
	imshow("input",TempImage);
	Mat Mask    = Mat(InpImg3C.size(), CV_8UC1) ;
	int MaxNoOfChannels = 3;

	/* declaration of variables for values from previous Mixture of Gaussian step, values are read from the file created in previous step */
	int NoOfGaussians;
	float SortMu[3];
	int SortMuIdx[3];
	float SortSigma[3];
	int SortSigmaIdx[3];
	float P99;
	float Percentil;
	float timeMOG;
	float timeCE;

	char StatistFileName[1000];
	char TempName[1000];
	int nl= strlen(cSourcem_oImageName);
	strncpy(TempName,cSourcem_oImageName,nl-4);
	TempName[nl-4]=0;
	sprintf(StatistFileName, "brain/%s_statis.txt",TempName);
	FILE* fid = fopen(StatistFileName, "r");
	printf ("Text file name is %s.\n", StatistFileName);

	fscanf(fid,"%d\n", &NoOfGaussians);

	/* read the values from the file with results of Mixture of Gaussians method */
	for (int nind = 0;nind<NoOfGaussians;nind++)
	{
		fscanf(fid,"%f ", SortMu+nind);
	}
	for (int nind = 0;nind<NoOfGaussians;nind++)
	{
		fscanf(fid,"%d ", SortMuIdx+nind);
	}
	for (int nind = 0;nind<NoOfGaussians;nind++)
	{
		fscanf(fid,"%f ", SortSigma+nind);
	}
	for (int nind = 0;nind<NoOfGaussians;nind++)
	{
		fscanf(fid,"%d ", SortSigmaIdx+nind);
	}

	fscanf(fid,"%f ", &P99);

	fscanf(fid,"%f ", &Percentil);

	fscanf(fid,"%f ", &timeCE);

	fscanf(fid,"%f ", &timeMOG);

	fclose(fid);

	int SubtractConst = P99 - SortMu[0];
	
	/**********************************************************************
	***************** STEPS FOR CRANIUM REMOVAL ***************************
	**********************************************************************/
	if(TUMOR==FALSE){
		
		// time measurement
		double t1 = (double)getTickCount();
		(*countOfSubjects)++;
		
		//clock_t beginT = clock();
		clock_t beginGC = clock();
		
		
		vector <Mat> channels;
		vector <Mat> tempchannels;
		cv::split(InpImg3Ccopy,channels);

		/* declare initial mask - mask is created in graph cut segmentation step and used as initial mask for next itaretion in graph cut segmentation */
		Mat FilterInpIm;
		Mat Mask    = Mat(InpImg3C.size(), CV_8UC1);
		Mat TempMat = Mask.clone();
		Mat TempMatDraw = Mask.clone();


		/* find correct value for thresholding */
		int thresholdValue = 0;
		if(SortMu[1]>20){
			thresholdValue = SortMu[1];
		}else{
			thresholdValue = SortMu[0];
		}

		/* threshold image */
		threshold(channels[0], TempMat, thresholdValue-10, 255, THRESH_BINARY);
		imshow("Threhold",TempMat);

		/* save thresholded image */
		sprintf(StatistFileName, "result/%s_Threshold.png",TempName);
		imwrite(StatistFileName,TempMat);

		/* find contours in thresholded image */
		TempMatDraw.setTo(0);
		Mat canny_output;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		
		findContours( TempMat, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

		/* draw rectangle around the cranium - initial rectangle for graph cut segmentation */
		int minXRect= TempMatDraw.cols;
		int maxXRect= 0;
		int minYRect= TempMatDraw.rows;
		int maxYRect= 0;
		Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
		bool CountoursDraw= false;

		if(contours.size()<1) return TempMat; //if any contour were not find
		
		/* find limit points of the cranium to paint initial rectangle for graph cut  */
		for( int i = 0; i< contours.size(); i++ ){
			float area = cv::contourArea(contours[i]);
			if(area >25){
				CountoursDraw = true;
				Rect BoundRect = cv::boundingRect(contours[i])	;
				minXRect = (std::min)(minXRect,BoundRect.x);
				minYRect = (std::min)(minYRect,BoundRect.y);
				maxXRect = (std::max)(maxXRect,BoundRect.x + BoundRect.width);
				maxYRect = (std::max)(maxYRect,BoundRect.y + BoundRect.height);

				Scalar color = Scalar( 255 );
				drawContours( TempMatDraw, contours, i, color, CV_FILLED, 8, hierarchy, 0, Point() );
			}
		}
		
		
		if (CountoursDraw){ 
			/* paint initial rectangle for graph cut */
			int RectLargeX = 0; 
			int RectLargeY = 0;
			minXRect = (std::max)(minXRect-RectLargeX, 0);
			minYRect = (std::max)(minYRect-RectLargeY, 0);
			maxXRect = (std::min)(maxXRect+RectLargeX, TempMatDraw.cols);
			maxYRect = (std::min)(maxYRect+RectLargeY, TempMatDraw.rows);
			Rect IniRect = Rect(minXRect, minYRect, maxXRect-minXRect, maxYRect-minYRect); 

			/* show and save initial rectangle */
			cv::rectangle(TempMatDraw,IniRect, Scalar(255));

			imshow("TempMatDraw", TempMatDraw);
			sprintf(StatistFileName, "result/%s_rect.png",TempName);
			imwrite(StatistFileName,TempMatDraw);

			Mat bgdModel;
			Mat fgdModel;

			/* graph cut segmentation initialized with rectangle
			*  algorithm creates a mask which is used for the next iteration of graph cut segmentation
			*/
			grabCut( InpImg3C, Mask, IniRect, bgdModel, fgdModel, 3, GC_INIT_WITH_RECT );
			imshow("MaskIter", Mask*50);
			Mat tmpTest;
			compare(Mask,cv::GC_PR_FGD,tmpTest,cv::CMP_EQ);
			int countOfNonZero = cv::countNonZero(tmpTest);
			
			/* if first iteration of graph cut segmentation found some mask then next iteration is initialized with that mask */
			if(countOfNonZero > 50){
				grabCut( InpImg3C, Mask, IniRect, bgdModel, fgdModel, 3, GC_INIT_WITH_MASK);
				(*countOfGraBCutSeg)++;
			}

			compare(Mask,cv::GC_PR_FGD,Mask,cv::CMP_EQ);
			Mat foreground(InpImg3C.size(),CV_8UC3, cv::Scalar(0,0,0));
			InpImg3C.copyTo(foreground,Mask);
			
			/* find contours of final graph cut segmentation */
			findContours( Mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
			/* draw found contour, but thicker to remove cranium */
			drawContours( InpImg3C, contours, -1, Scalar(0,255,0), 1, 8, 0, 0, Point() );
		
			/* mask used in previous step for cranium removal */
			Mask.setTo(0);
			drawContours( Mask, contours, -1, Scalar(255), 21, 8, 0, 0, Point() );
			imshow("MaskLebka",Mask);

			sprintf(StatistFileName, "result/%s_RemovedCraniumMask.png",TempName);
			imwrite(StatistFileName,Mask);

			InpImg3C.setTo(0,Mask);
			imshow("InpImg3C_bezlebky",InpImg3C);
			sprintf(StatistFileName, "result/%s_RemCranium.png",TempName);
			imwrite(StatistFileName,InpImg3C);
		}
		double t4 = (double)getTickCount();

		clock_t endGC = clock();

		(*timeGC) += (double(endGC-beginGC)/CLOCKS_PER_SEC);
		
	}else{
	
	/**********************************************************************
	***************** STEPS FOR TUMOR SEGMENTATION ************************
	**********************************************************************/

		// time measurement
		double t1 = (double)getTickCount();
		clock_t beginGC = clock();


		/* Image preprocessing using Morphological grayscale reconstruction */

		vector <Mat> channels;
		vector <Mat> tempchannels;
		cv::split(InpImg3Ccopy,channels);

		Mat sourceReconstr = Mat(InpImg3C.size(), CV_8U) ;
		Mat FilterInpIm;

		cv::split(InpImg3Ccopy,channels);
		cv::split(TempImage,tempchannels);
		imshow("tinp",InpImg3Ccopy);
		for(int ch = 0; ch < 3; ch++){
			cv::bitwise_not(channels[ch],channels[ch]);
			sourceReconstr = channels[ch] - SortMu[0];
			ImReconstruct( &(IplImage)(sourceReconstr), &(IplImage)(channels[ch]));
			sourceReconstr.copyTo(channels[ch]);

			cv::bitwise_not(channels[ch],channels[ch]);
			channels[ch].copyTo(tempchannels[ch]);
			sourceReconstr = channels[ch] - (SubtractConst); 
			imshow("test0",channels[ch]);
			ImReconstruct  ( &(IplImage)(sourceReconstr),&(IplImage)(channels[ch]));
			sourceReconstr.copyTo(channels[ch]);
		}
		merge(channels, InpImg3Ccopy);
		merge(tempchannels, TempImage);
		
		/* show image after morphological reconstruction - local maximas are removed */
		imshow("test1",TempImage);
		
		/* substrastion of image after morphological reconstruction from input image to get local maximal */
		InpImg3Ccopy = TempImage - InpImg3Ccopy;

		/* show image with local maxima */
		imshow("test2",InpImg3Ccopy*2);

		//GC_BGD    = 0,  //!< background
		//GC_FGD    = 1,  //!< foreground
		//GC_PR_BGD = 2,  //!< most probably background
		//GC_PR_FGD = 3   //!< most probably foreground
		sprintf(StatistFileName, "segm/%s_rec.png",TempName);
		imwrite(StatistFileName,InpImg3Ccopy*2);

		/* thresholding create a mask of the most intesive regions */
		Mat TempMat = Mask.clone() ;
		Mat TempMatDraw = Mask.clone() ;
		Mat bgdModel;
		Mat fgdModel;
		
		// Foreground
		cv::split(InpImg3Ccopy,tempchannels);
		threshold(tempchannels[0], TempMat, SubtractConst , 255, THRESH_OTSU); 

		/* morphological operations remove small seed from foreground mask created using thresholding */
		erode(TempMat, TempMat, StructElem3);
		dilate(TempMat, TempMat, StructElem3); 
		//imshow("MaskGC_FGDD",TempMat);
		
		fgdModel.setTo(GC_PR_FGD,TempMat);
		sprintf(StatistFileName, "segm/%s_mask_regions.png",TempName);
		imwrite(StatistFileName,TempMat);
		
		
		TempMatDraw.setTo(0);
		Mat canny_output;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		
		/* find all contorurs in region mask */
		findContours( TempMat, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

		/* find the best region - big or very intensive region, and mark it as a tumor = first tumor segmentation and also surly foreground
		 * 
		 * for the best region a rectangle for probably background is created
		 *
		*/
		int minXRect= TempMatDraw.cols;
		int maxXRect= 0;
		int minYRect= TempMatDraw.rows;
		int maxYRect= 0;
		Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
		bool FindCont = false;
		float meanMax = 0;
		Mat maxDraw;
		TempMatDraw.copyTo(maxDraw);
		maxDraw.setTo(0);
		for( int i = 0; i< contours.size(); i++ ){
			float area = cv::contourArea(contours[i]);
			if(area >150){ /* big region */

				FindCont = true;
				
				Scalar color = Scalar( 255 );
				TempMatDraw.setTo(0);
				drawContours( TempMatDraw, contours, i, color, CV_FILLED, 8, hierarchy, 0, Point() );
				
				Scalar meanRes = mean(InpImg3C, TempMatDraw);
				if(meanRes.val[0] > meanMax){ /* for intensive region */
					meanMax = meanRes.val[0];
					TempMatDraw.copyTo(maxDraw);
					/* creating rectangle for probably background */
					Rect BoundRect = cv::boundingRect(contours[i])	;
					minXRect = BoundRect.x;
					minYRect = BoundRect.y;
					maxXRect = BoundRect.x + BoundRect.width;
					maxYRect = BoundRect.y + BoundRect.height;
			
				}	
			}
		}
		maxDraw.copyTo(TempMatDraw);
		
		/* if some good region was found */
		if (FindCont > 0){
			
			/* enalarge probably background rectangle */
			int RectLargeX = TempMat.cols/50;
			int RectLargeY = TempMat.rows/50;
			minXRect = (std::max)(minXRect-RectLargeX, 0);
			minYRect = (std::max)(minYRect-RectLargeY, 0);
			maxXRect = (std::min)(maxXRect+RectLargeX, TempMatDraw.cols);
			maxYRect = (std::min)(maxYRect+RectLargeY, TempMatDraw.rows);
			Rect IniRect = Rect(minXRect, minYRect, maxXRect-minXRect, maxYRect-minYRect);

			Mat maskTreshVis;
			TempMatDraw.copyTo(maskTreshVis);
			
			/* save first tumor segmentation - segmentation without graph cut */
			sprintf(StatistFileName, "segm/%s_first_tumor_mask.png",TempName);
			imwrite(StatistFileName,TempMatDraw);

			/* read manual tumor segmentation for comparision */
			(*countOfSubjects)++;
			char fileName[1000];
			sprintf(fileName, "tumor/%s",tumorFileName);
			Mat handlySegm = imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
			cout << tumorFileName << '\n';
			
			/* comparision of our segmentation with manual segmentation */
			(*Tzoznam).push_front(originalFileName);
			compareResultsAndSegmentation(TempMatDraw, handlySegm, TempName, "t", Tsensitivity, Tspecificy, Tprecision, Tpvn, Taccurancy, Tdice, Toverlap);

			/* read original image to paint and save resulting contours of tumor ()without graph cut */
			sprintf(fileName, "original/%s",originalFileName);
			cv::Mat Result = imread(fileName);
			
			/* find contours of the first tumor segmentation */
			vector<vector<Point> > contoursTresh;
			findContours( maskTreshVis, contoursTresh, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
			for( int i = 0; i< contoursTresh.size(); i++ ){
			   drawContours( Result, contoursTresh, i, Scalar(0,0,255), 2, 8, hierarchy, 0, Point() );
			}

			/* save painted resulting contours into original image */
			sprintf(StatistFileName, "segm/%s_segm_T.png",TempName);
			imwrite(StatistFileName,Result);
			
			/* initialize mask for the graph cut segmentation */
			Mask.setTo(GC_BGD); // background
			Mask(IniRect).setTo(GC_PR_BGD); // probably background - rectangle created in previous steps
			Mask.setTo(GC_PR_FGD,TempMatDraw); // foreground - first tumor segmentation

			cv::rectangle(TempMatDraw,IniRect, Scalar(255));
			imshow("MaskGC_FGDD_1", TempMatDraw);
			
			/* save initial mask */
			sprintf(StatistFileName, "segm/%s_mask_GC.png",TempName);
			imwrite(StatistFileName,TempMatDraw);
			imshow("MaskSKUSKA", Mask*100);
			
			/* graph cut segmentation initialized with the mask */
			grabCut( InpImg3C, Mask, IniRect, bgdModel, fgdModel, 3, GC_INIT_WITH_MASK);
			
			Mat tmpTest;
			compare(Mask,cv::GC_PR_FGD,tmpTest,cv::CMP_EQ);
			int countOfNonZero = cv::countNonZero(tmpTest);
			
			/* if graph cut failed - did not find anything */
			if(countOfNonZero < 150){
				clock_t endGC = clock();
				(*timeGC) += (double(endGC-beginGC)/CLOCKS_PER_SEC);
				return tmpTest;
			}

			imshow("Mask Grab Cut", Mask*100);
			compare(Mask,cv::GC_PR_FGD,Mask,cv::CMP_EQ);
			Mat foreground(InpImg3C.size(),CV_8UC3, cv::Scalar(0,0,0));
			InpImg3C.copyTo(foreground,Mask);
			imshow("Foreground",foreground);
			imshow("Final mask after GC",Mask);
			vector<vector<Point> > contoursSeg;

			/* find final contours of tumor after the graph cut*/
			findContours( Mask, contoursSeg, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
			
			sprintf(StatistFileName, "segm/%s_mask_gc.png",TempName);
			imwrite(StatistFileName,Mask*50);
			 
			/* paint these contours into original image */
			for( int i = 0; i< contoursSeg.size(); i++ ){
			   drawContours( Result, contoursSeg, i, Scalar(0,255,0), 1.8, 8, hierarchy, 0, Point() );
			 }

			/* compare our segmentation (with graph cut)  with manual segmentation */
			if(contoursSeg.size() >= 1){
				(*countOfGraBCutSeg)++;
				(*GCzoznam).push_front(originalFileName);
				compareResultsAndSegmentation(Mask, handlySegm, TempName, "gc", GCsensitivity, GCspecificy, GCprecision, GCpvn, GCaccurancy, GCdice, GCoverlap);
			}
			
			imshow("segmInp",InpImg3C);
			sprintf(StatistFileName, "segm/%s_segm_T_GC.png",TempName);
			imwrite(StatistFileName,Result);
			
			
			}

			clock_t endGC = clock();

			(*timeGC) += (double(endGC-beginGC)/CLOCKS_PER_SEC);

			double t4 = (double)getTickCount();
		
	}
	waitKey(1);
	return Mask;
}

/* the method calculate statistical values of tumor segmentation (segmentation of proposed method is compared to manual segmentations provided by the specialist) */
void compareResultsAndSegmentation(Mat seg, Mat hSeg, char *temName, char *aka, 
								   list<float> *sensitivity, list<float> *specificy,
								   list<float> *precision, list<float> *pvn, list<float> *accurancy, list<float> *dice, list<float> *overlap){
	Mat handlySegm;
	Mat segmentation;
	int i,j;
	ofstream outputFile;
	char fileName[1000];

	/* read manual segmentation */
	sprintf(fileName, "segm/%s_%s_comparision.txt",temName, aka);
	outputFile.open(fileName);
	
	threshold(hSeg, handlySegm, 0, 255, CV_THRESH_BINARY);
      
	threshold(seg, segmentation, 0, 255, CV_THRESH_BINARY);
    
	outputFile << "Vyska: " << handlySegm.rows << '\n';
	outputFile << "Sirka: " << handlySegm.cols*handlySegm.channels() << '\n';
	
	int truePositive = 0;
	int trueNegative = 0;
	int falsePositive = 0;
	int falseNegative = 0;
	int dice_H = 0;
	int dice_S = 0;
	int dice_I = 0;

	Mat image(handlySegm.rows, handlySegm.cols, CV_8UC3, Scalar(0, 0, 0));
		
	/* count true positive, true negative, false positive and false negative */
	for(i = 0; i < handlySegm.rows; i++){
		for(j = 0; j < handlySegm.cols; j++){
			int pixHAND = (int)handlySegm.at<uchar>(i,j);
			int pixSEG = (int)segmentation.at<uchar>(i,j);
			if(pixHAND != 0){
				dice_H++;
			}
			if(pixSEG != 0){
				dice_S++;
			}
			if(pixHAND != 0 && pixSEG != 0){
				truePositive++;
				dice_I++;
				image.at<Vec3b>(i,j) = Vec3b(255,255,255);
			}
			if(pixHAND == 0 && pixSEG == 0){
				trueNegative++;
				image.at<Vec3b>(i,j) = Vec3b(0,0,0);
			}
			if(pixHAND == 0 && pixSEG != 0){
				falsePositive++;
				image.at<Vec3b>(i,j) = Vec3b(0,255,0);
			}
			if(pixHAND != 0 && pixSEG == 0){
				falseNegative++;
				image.at<Vec3b>(i,j) = Vec3b(255,0,0);
			}
		}
	}
	sprintf(fileName, "result_%s", aka);
	imshow(fileName, image );
	
	sprintf(fileName, "segm/%s_%s_comparision.png",temName, aka);
	imwrite(fileName,image);

	outputFile << "True Positive: " << truePositive;
	outputFile << " je " << (float)truePositive/(float)(truePositive+falsePositive+trueNegative+falseNegative)*100 << " % z obrazka" << '\n';
	outputFile << "True Negative: " << trueNegative;
	outputFile << " je " << (float)trueNegative/(float)(truePositive+falsePositive+trueNegative+falseNegative)*100 << " % z obrazka" << '\n';
	outputFile << "False Positive: " << falsePositive;
	outputFile << " je " << (float)falsePositive/(float)(truePositive+falsePositive+trueNegative+falseNegative)*100 << " % z obrazka" << '\n';
	outputFile << "False Negative: " << falseNegative;
	outputFile << " je " << (float)falseNegative/(float)(truePositive+falsePositive+trueNegative+falseNegative)*100 << " % z obrazka" << '\n';
	
	/* calculate statistical methods for one image */
	float sensitivityTmp = (float)truePositive/(float)(truePositive+falseNegative);
	(*sensitivity).push_front(sensitivityTmp);
	sensitivityTmp *= 100;
	outputFile << fixed << setprecision(2) << "Sensitivity (true positive rate): " << sensitivityTmp << "%" << '\n';

	float specificyTmp = (float)trueNegative/(float)(trueNegative+falsePositive);
	(*specificy).push_front(specificyTmp);
	specificyTmp *= 100;
	outputFile << fixed << setprecision(2) << "Specificy (true negative rate): " << specificyTmp << "%" << '\n';
	
	float precisionTmp = (float)truePositive/(float)(truePositive+falsePositive);
	(*precision).push_front(precisionTmp);
	precisionTmp *= 100;
	outputFile << fixed << setprecision(2) << "Precision (predictive value positive): " << precisionTmp << "%" << '\n';

	float pvnTmp = (float)trueNegative/(float)(trueNegative+falseNegative);
	(*pvn).push_front(pvnTmp);
	pvnTmp *= 100;
	outputFile << fixed << setprecision(2) << "Predictive value negative: " << pvnTmp << "%" << '\n';

	float accurancyTmp = (float)(truePositive+trueNegative)/(float)(truePositive+falsePositive+trueNegative+falseNegative);
	(*accurancy).push_front(accurancyTmp);
	accurancyTmp *= 100;
	outputFile << fixed << setprecision(2) << "Accurancy: " << accurancyTmp << "%" << '\n';

	float diceR = (float)(2*dice_I)/(float)(dice_H+dice_S);
	(*dice).push_front(diceR);
	diceR*=100;
	outputFile << fixed << setprecision(2) << "Dice: " << diceR << "%" << '\n';

	float overlapR = (float)(truePositive)/(float)(truePositive+falseNegative+falsePositive);
	(*overlap).push_front(overlapR);
	overlapR*=100;
	outputFile << fixed << setprecision(2) << "Overlap: " << overlapR << "%" << '\n';

	outputFile.close();

	return;
}

/* the method calculate average values of statistical methods of tumor segmentation */
void statis(list<float> *Tsensitivity, list<float> *Tspecificy,
			list<float> *Tprecision, list<float> *Tpvn, list<float> *Taccurancy,
			list<float> *Tdice, list<float> *Toverlap,
			list<float> *GCsensitivity, list<float> *GCspecificy,
			list<float> *GCprecision, list<float> *GCpvn, list<float> *GCaccurancy,
			list<float> *GCdice, list<float> *GCoverlap,
			int *countOfSubjects, int *countOfGraBCutSeg, list<String> *Tzoznam, list<String> *GCzoznam){
	ofstream outputFile;
	outputFile.open("resultStatis.txt"); //file with average results of statistical methods of all images in folder

	ofstream data;
	data.open("data.txt"); //file with all results of statistical methods of all images in folder

	data << "NUMBER;NAME;SENSITIVITY;SPECIFICY;PRECISION;PVN;ACCURANCY;DICE;OVERLAP" << '\n'; //thanks this formation it can be opened in excel

	int i;
	float sumSensitivity = 0;
	float sumSpecificy = 0;
	float sumPrecision = 0;
	float sumPvn = 0;
	float sumAccurancy = 0;
	float sumDice = 0;
	float sumOverlap = 0;

	
	/********************************
	*   METHOD WITHout GRAPH CUT    *
	*********************************/

	outputFile << "Pocet subjektov count: " << (*countOfSubjects) << '\n';
	outputFile << "Pocet subjektov: " << (*Tsensitivity).size() << '\n';
	
	outputFile << "THRESHOLD" << '\n';

	/* count sumary values of statistical methods which were counted only after thresholding (method without graph cut) */
	for(int i = 0; i < (*Tsensitivity).size(); i++){
		data << i+1 << ";";

		list<String>::iterator bla = (*Tzoznam).begin();
		advance(bla, i);
		data << fixed << *bla << ";";

		list<float>::iterator it = (*Tsensitivity).begin();
		advance(it, i);
		sumSensitivity += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*Tspecificy).begin();
		advance(it, i);
		sumSpecificy += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*Tprecision).begin();
		advance(it, i);
		sumPrecision += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*Tpvn).begin();
		advance(it, i);
		sumPvn += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*Taccurancy).begin();
		advance(it, i);
		sumAccurancy += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*Tdice).begin();
		advance(it, i);
		sumDice += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*Toverlap).begin();
		advance(it, i);
		sumOverlap += *it;
		data << fixed << setprecision(4) << *it << "\n";
	}

	data << "\n" << "\n";
	data << "NUMBER;NAME;SENSITIVITY; SPECIFICY; PRECISION; PVN; ACCURANCY; DICE; OVERLAP" << '\n';

	/* count average values of statistical methods which were counted only after thresholding (method without graph cut) */
	float sensitivity = sumSensitivity/(*Tsensitivity).size()*100;
	outputFile << fixed << setprecision(2) << "Sensitivity (true positive rate): " << sensitivity << "%" << '\n';

	float specificy = sumSpecificy/(*Tspecificy).size()*100;
	outputFile << fixed << setprecision(2) << "Specificy (true negative rate): " << specificy << "%" << '\n';

	float precision = sumPrecision/(*Tprecision).size()*100;
	outputFile << fixed << setprecision(2) << "Precision (predictive vlue positive): " << precision << "%" << '\n';

	float pvn = sumPvn/(*Tpvn).size()*100;
	outputFile << fixed << setprecision(2) << "Predictive vlue negative: " << pvn << "%" << '\n';

	float accurancy = sumAccurancy/(*Tsensitivity).size()*100;
	outputFile << fixed << setprecision(2) << "Accurancy: " << accurancy << "%" << '\n';

	float dice = sumDice/(*Tdice).size()*100;
	outputFile << fixed << setprecision(2) << "Dice score: " << dice << "%" << '\n';

	float overlap = sumOverlap/(*Toverlap).size()*100;
	outputFile << fixed << setprecision(2) << "Overlap: " << overlap << "%" << '\n';

	sumSensitivity = 0;
	sumSpecificy = 0;
	sumPrecision = 0;
	sumPvn = 0;
	sumAccurancy = 0;
	sumDice = 0;
	sumOverlap = 0;


	/*****************************
	*   METHOD WITH GRAPH CUT    *
	*****************************/

	outputFile << "GRAB CUT" << '\n';

	outputFile << "Pocet subjektov, na ktorych fungoval grab cut: " << (*GCsensitivity).size() << " alebo counter dava " << (*countOfGraBCutSeg) <<'\n';
	outputFile << (*GCsensitivity).size() << " z " << (*Tsensitivity).size() << " je "; 
	float uspesnst = (float)(*GCsensitivity).size()/(float)(*Tsensitivity).size()*100;
	outputFile << fixed << setprecision(2)	<< uspesnst << " %" << '\n';


	/* count sumary values of statistical methods which were counted after graph cut segmentation (method with graph cut) */
	for(int i = 0; i < (*GCsensitivity).size(); i++){
		data << i+1 << ";";

		list<String>::iterator bla = (*GCzoznam).begin();
		advance(bla, i);
		data << fixed << setprecision(4) << *bla << ";";

		list<float>::iterator it = (*GCsensitivity).begin();
		advance(it, i);
		sumSensitivity += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*GCspecificy).begin();
		advance(it, i);
		sumSpecificy += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*GCprecision).begin();
		advance(it, i);
		sumPrecision += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*GCpvn).begin();
		advance(it, i);
		sumPvn += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*GCaccurancy).begin();
		advance(it, i);
		sumAccurancy += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*GCdice).begin();
		advance(it, i);
		sumDice += *it;
		data << fixed << setprecision(4) << *it << ";";

		it = (*GCoverlap).begin();
		advance(it, i);
		sumOverlap += *it;
		data << fixed << setprecision(4) << *it << "\n";
	}

	/* count average values of statistical methods which were counted after graph cut segmentation (method with graph cut) */
	sensitivity = sumSensitivity/(*GCsensitivity).size()*100;
	outputFile << fixed << setprecision(2) << "Sensitivity (true positive rate): " << sensitivity << "%" << '\n';

	specificy = sumSpecificy/(*GCspecificy).size()*100;
	outputFile << fixed << setprecision(2) << "Specificy (true negative rate): " << specificy << "%" << '\n';

	precision = sumPrecision/(*GCprecision).size()*100;
	outputFile << fixed << setprecision(2) << "Precision (predictive vlue positive): " << precision << "%" << " a sucet precision: " << sumPrecision << " a pocet "<< (*GCprecision).size() << '\n';

	pvn = sumPvn/(*GCpvn).size()*100;
	outputFile << fixed << setprecision(2) << "Predictive vlue negative: " << pvn << "%" << '\n';

	accurancy = sumAccurancy/(*GCsensitivity).size()*100;
	outputFile << fixed << setprecision(2) << "Accurancy: " << accurancy << "%" << '\n';

	dice = sumDice/(*GCdice).size()*100;
	outputFile << fixed << setprecision(2) << "Dice score: " << dice << "%" << '\n';

	overlap = sumOverlap/(*GCoverlap).size()*100;
	outputFile << fixed << setprecision(2) << "Overlap: " << overlap << "%" << '\n';

	outputFile.close();
	data.close();
}