Before the run, it is necessary to install some software. Part of the method is implemented in C++ 
and next part is implemented in MATLAB. Hence, it is necessary to install MATLAB and some C++ compiler. 
It is also necessary to install OpenCV library because proposed method uses some computer vision 
methods from this library.


I add some testing images into folder dataset. 


After the successful installation, it is possible to run the program components. Any interface for 
the user was not created because our goal was to develop method, not application. Program can be 
tested by following these next steps:

1.	run MatLab_step1.m placed in folder MatLab_step1
	1.1.	copy input magnetic resonance images to folder MatLab_step1/data/ 
	1.2.	run code – input images are rescaled and statistical values for cranium removal 
			are counted by Mixture of Gaussians method
	1.3.	output are rescaled images inputName_rescaled.png and text files inputName_rescaled_statis.txt 
			which contain resulting values of Mixture of Gaussians method; it is saved in folder MatLab_step1/data/

2.	run c++ cranium removal step
	2.1.	copy rescaled images and text files from previous step to folder cppCode/ConsoleApplication2/brain 
	2.2.	set constant TUMOR to false value: bool TUMOR = FALSE;
	2.3.	run code – cranium is removed
	2.4.	output of this step are images which show the process of cranium removal step and final images with 
			removed cranium inputName_rescaled_RemCranium.png which are used for the next step; they are saved 
			in folder cppCode/ConsoleApplication2/results

3.	run MatLab-step2.m placed in folder MatLab_step2
	3.1.	copy images with removed cranium to folder MatLab_step2/data/
	3.2.	run code – statistical values tumor segmentation step are counted by Mixture of Gaussians method
	3.3.	output are text files inputName_rescaled_RemCranium_statis.txt which contain resulting values of
			Mixture of Gaussians method; it is saved in folder MatLab_step2/data/

4.	run C++ tumor segmentation step
	4.1.	copy text files from previous step with images which have removed cranium to clean folder 
			cppCode/ConsoleApplication2/brain
	4.2.	copy original input MRI images or images where you want visualize resulting segmentation to folder 
			cppCode/ConsoleApplication2/original
	4.3.	copy masks of tumors segmentations provided by the experts to folder cppCode/ConsoleApplication2/tumor
	4.4.	set constant TUMOR to true value: bool TUMOR = TRUE;
	4.5.	run the code – tumors are segmented
	4.6.	the results are: visualization of resulting segmentation, some images which show steps of the method 
			and statistical evaluation of resulting segmentations:
			
				•	inputName_rescaled_RemCranium_segm_T_GC.png – visualization of segmentation with and without 
					graph cut algorithm
				•	inputName_rescaled_RemCranium_t_comparision.png – division of pixels on TP, TN, FP, FN for 
					method without graph cut algorithm
				•	inputName_rescaled_RemCranium_gc_comparision.png – division of pixels on TP, TN, FP, FN for 
					method with graph cut algorithm
				•	inputName_rescaled_RemCranium_t_comparision.txt – statistical values of evaluation of method 
					without graph cut algorithm
				•	inputName_rescaled_RemCranium_gc_comparision.txt – statistical values of evaluation of method 
					with graph cut algorithm
				•	resultStatis.txt – average statistical evaluation of all input images
				•	data.txt – statistical values of all images formatted for .csv conversion and excel view 
					and other files
