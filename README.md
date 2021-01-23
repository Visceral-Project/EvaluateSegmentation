********************************************************************************
#### License, Copyright, and Authors

EvaluateSegmentation Tool, VISCERAL http://www.visceral.eu  
VISCERAL received funding from EU FP7, contract 318068   
Developed by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)   
24.02.2013  
Copyright 2013 Vienna University of Technology I
nstitute of Software Technology and Interactive Systems  

Licensed under the Apache License, Version 2.0 (the "License");  
you may not use this file except in compliance with the License.  
You may obtain a copy of the License at  

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software  
distributed under the License is distributed on an "AS IS" BASIS,  
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
See the License for the specific language governing permissions and  
limitations under the License.  

#### Citing this Software

Please cite the following paper to reference this software \[[pdf](http://www.biomedcentral.com/content/pdf/s12880-015-0068-x.pdf)\] \[[bibtex](bibtex.txt)\]:

*Abdel Aziz Taha and Allan Hanbury. Metrics for evaluating 3D medical image segmentation: analysis, selection, and tool. BMC Medical Imaging, 15:29, August 2015.*

For citing the new metric bAVD (balanced Average Hausdorff Distance), please cite the following article:

*O.  U.  Aydin,  A.  A.  Taha,  A.  Hilbert,  A.  A.  Khalil,  I.  Galinovic,  J.  B.Fiebach,  D.  Frey,  and  V.  I.  Madai,  “On  the  usage  of  average  hausdorffdistance for segmentation performance assessment:  Hidden bias when usedfor  ranking,”European  Radiology  Experimental,  vol.  5,  2021. Available:  https://doi.org/10.1186/s41747-020-00200-2*


#### Description 

EvaluateSegmentation is a tool that compares two volumes (a test segmentation and a ground truth segmentation) using 22 different metrics that were selected as a result of a comprehensive research into the metrics used in the medical volume segmentations. 

The article "Metrics for evaluating 3D medical image segmentation: analysis, selection, and tool" in the BMC Medical Imaging journal (available as open access here: http://http://www.biomedcentral.com/1471-2342/15/29) provides definitions and a comprehensive analysis of these metrics as well as guidelines for metric selection based on the properties of the segmentations being evaluated and the segmentation goal.

EvaluateSegmentation provides the following measures:  
 
Similarity  
1.	Dice Coefficient  
2.	Jaccard Coefficient  
3.	Area under ROC Curve (one system state)  
4.	Cohen Kappa  
5.	Rand Index  
6.	Adjusted Rand Index  
7.	Interclass Correlation  
8.	Volumetric Similarity Coefficient  
9.	Mutual Information

Distance   
10.	Hausdorff Distance (optionally in voxel or millimeter)  
11.	Average Hausdorff Distance  (optionally in voxel or millimeter)  
12.	Balanced Average Hausdorff Distance  (a novel metric, published in 2021, see reference above)  
13.	Mahanabolis Distance  
14.	Variation of Information  
15.	Global Consistency Error  
16.	Coefficient of Variation  
17.	Probabilistic Distance

Classic measures  
18.	Sensitivity (Recall, true positive rate)  
19.	Specificity (true negative rate)  
12.	Precision  
21.	F-Measure  
22.	Accuracy  
23.	Fallout (false positive rate)  
24.	true positives (in voxel)  
25.	true negatives (in voxel)  
26.	false positives (in voxel)  
27.	false negatives (in voxel)  
28.	segmented volume (optionally in voxel or milliliter)  
29.	reference volume (optionally in voxel or milliliter)  


#### Supported Images  

EvaluateSegmentation is built on top of the ITK Library thus it supports all 2D/3D file formats   
that are supported by ITK, e.g .nii, .mha, etc. The two Images should however have the  
same size (the same number of grid size). There should be only one label in an image,  
where a voxel value can be either zero (background) or a value between zero and  
one [0,1] that denotes the fuzzy membership or the probability that the  
corresponding voxel belongs to the label.  

#### Syntax

1) Evaluation of volume segmentations:

USAGE: 

		EvaluateSegmentation truthPath segmentPath [-thd threshold] [-xml xmlpath] [-use all|DICE,JACRD, ....]

		where:  
		truthPath:	path (or URL) to truth image. URLs should be enclosed with quotations.  
		segmentPath:	path (or URL) to image being evaluated. URLs should be enclosed with quotations.  
		-help:		information  
		-thd threshold:	before evaluation convert fuzzy images to binary using the given threshold.  
		-xml xmlpath:	path to xml file where results should be saved.  
		-nostreaming:	Don't use streaming filter! Streaming filter is used to handle very large images. Use this option with small images (up to 200X200X200 voxels) to avoid time efort related with streaming.
		-unit:		use millimeter or voxel for distances and  volumes (default is voxel)
		-use metriclist: this option can be used to specify which metrics should be used. 
		
		metriclist for the option -use consists of the codes of the desired metrics separated by commas. For those metrics that accept parameters, it is possible to pass these parameters  by writing them between two @ characters, e.g. –use MUTINF,FMEASR@0.5@. This option tells the tool to calculate the mutual information and the F-Measure at beta=0.5. The possible codes to be used for metriclist are:  

		all: all available metrics (default)  
		DICE: Dice Coefficient  
		JACRD: Jaccard Coefficient  
		GCOERR: Global Consistency Error  
		VOLSMTY: Volumetric Similarity Coefficient  
		KAPPA: Cohen Kappa  
		AUC: Area under ROC Curve (one system state)  
		RNDIND: Rand Index  
		ADJRIND: Adjusted Rand Index  
		ICCORR: Interclass Correlation   
		MUTINF: Mutual Information  
		FALLOUT: Fallout (false positive rate)  
		COEFVAR: Coefficient of Variation  
		AVGDIST: Average Hausdorff Distance (in voxel or millimeter according to -unit)
		bAVD: Balanced Average Hausdorff Distance
		HDRFDST: Hausdorff Distance in voxels HDRFDST@0.95@ means use 0.95 quantile to avoid outliers. Default is quantile of 1 which means exact Hausdorff distance  (in voxel or millimeter according to -unit)
		VARINFO: Variation of Information  
		PROBDST: Probabilistic Distance  
		MAHLNBS: Mahanabolis Distance  
		SNSVTY: Sensitivity (Recall, true positive rate)  
		SPCFTY: Specificity (true negative rate)  
		PRCISON: Precision  
		FMEASR: F-Measure  FMEASR@0.5@ means use 0.5 as a value for beta in the F-Measure  
		ACURCY: Accuracy 
		TP: true positives in voxel)
		TN: true negatives (in voxel)
		FP: false positives (in voxel)
		FN: false negatives (in voxel)
		SEGVOL: segmented volume (in voxel or milliliter according to -unit)
		REFVOL: reference volume (in voxel or milliliter according to -unit)

		Examples  
		Example 1: EvaluateSegmentation truth.nii segment.nii –use RNDIND,HDRFDST@0.96@, FMEASR@0.5@ -xml result.xml
		
		This example shows how to compare two NIFTI images providing the Rand Index, Hausdorff distance and the F-Measure and save the results in result.xml. The values between two @ symbols are parameters to the specific  measures in this case the quantile value used with Hausdorff distance to avoid outliers. The second value (0.5) is the beta value used with the F-Measure.

		Example 2: EvaluateSegmentation truth.nii segment.nii –use all  -thd 0.5  
		
		This example compares two images using all available metrics. Before comparing the images, they are converted to binary images using a threshold of 0.5, that is voxels with values in [0,0.5) are considered as background and those with values in [0.5,1] are assigned the label with a membership of 1. 

2) Evaluation of landmark localization:

USAGE:  

		EvaluateSegmentation -loc truth_landmark_path test_landmark_path [-xml output_xml_path]

		where:  
		truth_landmark_path =path (or URL) to file containing ground truth landmarks. URLs should be enclosed with quotations.

		test_landmark_path =path (or URL) to file containing landmarks to be evaluated. URLs should be enclosed with quotations.

		-xml =path to xml file where result should be saved.


3) Evaluation of lesion detection:

USAGE:  

		EvaluateSegmentation -det truth_lesions_path test_lesions_path [-xml output_xml_path]

		where:  
		truth_lesions_path =path (or URL) to file containing ground truth lesions. URLs should be enclosed with quotations.

		test_lesions_path =path (or URL) to file containing lesions to be evaluated. URLs should be enclosed with quotations.

		-xml =path to xml file where result should be saved.

4 Builds

The builds provided in the folder builds are not necessarily up to date for all OS. At the moment, we have only support for Windows and Ubunto. For other OS, the builds are either not available or not up to date. In this case, please use the source code to make your own build for your OS. Here are some links you may need for build the EvaluateSegmentation tool:

ITK Library: https://itk.org/itkindex.html  
CMake Framework: https://cmake.org/
