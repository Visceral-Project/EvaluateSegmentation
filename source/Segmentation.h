/*
// Segmentation.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 11.04.2013 
// Copyright 2013 Vienna University of Technology
// Institute of Software Technology and Interactive Systems
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// 
// Description:
//
// This file is responsible for evaluating two segmentations. It links the suitable evaluation 
// algorithm to perform the evaluation and then it links the program part responsible for 
// displaying the values or writting them in an xml file according to the set options.
//
*/

#ifndef _SEGMENTATION
#define _SEGMENTATION 

#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <time.h>
#include <sys/timeb.h>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"
#include "itkTextOutput.h"
#include "Metric_constants.h"
#include "VoxelPreprocessor.h"
#include "ContingencyTable.h"
#include "HausdorffDistanceMetric.h"
#include "InterclassCorrelationMetric.h"
#include "MahalanobisDistanceMetric.h"
#include "AverageDistanceMetric.h"
#include "DiceCoefficientMetric.h"
#include "JaccardCoefficientMetric.h"
#include "RandIndexMetric.h"
#include "CohinKappaMetric.h"
#include "VolumeSimilarityCoefficient.h"
#include "GlobalConsistencyError.h"
#include "ProbabilisticDistanceMetric.h"
#include "VariationOfInformationMetric.h"
#include "MutualInformationMetric.h"
#include "ClassicMeasures.h"
#include "Imagedownloader.h" 

#include "ImageStatistics.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkStreamingImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkCastImageFilter.h"



#ifdef _DEBUG
#include "HausdorffDistanceMetric_V1.h"
#include "HausdorffDistanceMetric_V2.h"
#include "HausdorffDistanceMetric_ITK.h"
#include "NaiveHausdorffDistanceMetric.h"
#include "TestHausdorff.h" 
#include "AverageDistanceMetric_V3.h"
#include "MeanToMeanDistanceMetric.h"
#include "TestMetric.h"
#include "AbstractValidationMetric.h"
#include "CohenKappaImageToImageMetric.h"
#include "CohenKappaImageToImageMetric.txx"
#endif


using namespace std;

int validateImage(const char* f1, const char* f2, double threshold, const char* targetFile, char *options);
bool shouldUse(MetricId, char* options);
std::string getOptions(MetricId id, char* options);
bool isUrl(const char* path);
void testHausdorf(ImageType::Pointer truthImg, ImageType::Pointer testImg, double threshold, bool fuzzy, MetricId metricId, itk::DOMNode::Pointer xmlObject, int option, VoxelPreprocessor *);
const std::string nooption = "NOOPTION";
ImageType::Pointer loadImage(const char* filename);


int validateImage(const char* f1, const char* f2, double threshold, const char* targetFile, char *options)
{


	time_t begin;
	time(&begin);

	VoxelPreprocessor *voxelPreprocessor;
	ImageStatistics *imagestatistics;

	bool fuzzy  = (threshold == -1);
	if(!fuzzy){
		std::cout << "Crisp segmentation at threshold= " << threshold << "\n" << std::endl;
	}
	ImageType::Pointer truthImg = loadImage(f1);
	if(truthImg == (ImageType::Pointer) 0){
		return EXIT_FAILURE;
	}

	ImageType::Pointer testImg = loadImage(f2);

	if(testImg == (ImageType::Pointer) 0){
		return EXIT_FAILURE;
	}

	imagestatistics = new ImageStatistics(truthImg, testImg, fuzzy, threshold);

	values_f = (pixeltype*) malloc(imagestatistics->numberElements_f * sizeof(pixeltype));
	if(values_f == NULL){
		std::cout << "Memory allocation 1 !" << std::endl;
		return  EXIT_FAILURE ;
	}

	for(int i=0; i< imagestatistics->numberElements_f ; i++){
		values_f[i]=0;
	}
	values_m = (pixeltype*) malloc(imagestatistics->numberElements_m * sizeof(pixeltype));
	if(values_m == NULL){
		std::cout << "Memory allocation 2 !" << std::endl;
		return  EXIT_FAILURE;
	}
	for(int i=0; i< imagestatistics->numberElements_m ; i++){
		values_m[i]=0;
	}
	voxelPreprocessor = new VoxelPreprocessor(truthImg, testImg, fuzzy, threshold, imagestatistics);
	ContingencyTable *contingenceTable= new ContingencyTable(voxelPreprocessor, fuzzy, threshold);

	itk::DOMNode::Pointer xmlObject = OpenSegmentationResultXML(targetFile, f1, f2, voxelPreprocessor->num_nonzero_points_f, voxelPreprocessor->num_nonzero_points_m, voxelPreprocessor->num_intersection);

	if(voxelPreprocessor->IsDifferentImageSize()){
		std::ostringstream  message;
		message << "Images are from different Sizes: fixedImage=" << voxelPreprocessor->GetFixedImageVoxelCount() << " movingImage=" << voxelPreprocessor->GetMovingImageVoxelCount() ;
		pushMessage(message.str().c_str(), targetFile, f1, f2);
		return 0;
	}
	if(voxelPreprocessor->IsFixedImageEmpty()){
		std::ostringstream  message;
		message << "Fixed image is empty!" ;
		pushMessage(message.str().c_str(), targetFile, f1, f2);
		return 0;
	}
	if(voxelPreprocessor->IsMovingImageEmpty()){
		std::ostringstream  message;
		message << "Moving image is empty!" ;
		pushMessage(message.str().c_str(), targetFile, f1, f2);
		return 0;
	}

	std::cout << "Similarity:" << std::endl;
	double value=0;
	MetricId metricId = DICE;
	if(shouldUse(metricId, options)){
		DiceCoefficientMetric *diceCoefficient = new DiceCoefficientMetric(contingenceTable, fuzzy, threshold);
		value = diceCoefficient->CalcDiceCoeff();
		pushValue(metricId, value, xmlObject);
	}

	metricId = JACRD;
	if(shouldUse(metricId, options)){
		JaccardCoefficientMetric *jaccardCoefficient = new JaccardCoefficientMetric(contingenceTable, fuzzy, threshold);
		value =  jaccardCoefficient->CalcJaccardCoeff();
		pushValue(metricId, value, xmlObject);
	}

	ClassicMeasures *classicMeasures = new ClassicMeasures(contingenceTable, fuzzy, threshold);
	metricId = AUC;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcAUC();
		pushValue(metricId, value, xmlObject);
	}


	metricId = KAPPA;
	if(shouldUse(metricId, options)){
		CohinKappaMetric *cohinKappa = new CohinKappaMetric(contingenceTable, fuzzy, threshold);
		value =  cohinKappa->CalcCohenKappa();
		pushValue(metricId, value, xmlObject);
	}

	metricId = RNDIND;
	RandIndexMetric *randIndex = new RandIndexMetric(contingenceTable, fuzzy, threshold);
	if(shouldUse(metricId, options)){
		value =  randIndex->CalcRandIndex();
		pushValue(metricId, value, xmlObject);
	}
	metricId = ADJRIND;
	if(shouldUse(metricId, options)){
		value =  randIndex->CalcAdjustedRandIndex();
		pushValue(metricId, value, xmlObject);
	}


	metricId = ICCORR;
	if(shouldUse(metricId, options)){
		InterclassCorrelationMetric *interclassCorrelation = new InterclassCorrelationMetric(voxelPreprocessor, fuzzy, threshold);
		value =  interclassCorrelation->CalcInterClassCorrelationCoeff();
		pushValue(metricId, value, xmlObject);
	}

	metricId = VOLSMTY;
	if(shouldUse(metricId, options)){
		VolumeSimilarityCoefficient *volumetricSimilarity = new VolumeSimilarityCoefficient(contingenceTable, fuzzy, threshold);
		value =  volumetricSimilarity->CalcVolumeSimilarityCoefficient();
		pushValue(metricId, value, xmlObject);
	}

	metricId = MUTINF;
	if(shouldUse(metricId, options)){
		MutualInformationMetric *mutualInformation = new MutualInformationMetric(contingenceTable, fuzzy, threshold);
		value =  mutualInformation->CalcMutualInformation();
		pushValue(metricId, value, xmlObject);
	}




	//metricId = AUC;
	//if(shouldUse(metricId, options)){
	//    value = -1;
	//	pushValue(metricId, value, xmlObject);
	//}

	std::cout << "\nDistance:" << std::endl;


	metricId = HDRFDST;
	if(shouldUse(metricId, options)){
		std::string quantile_s=getOptions(metricId, options);

		double quantile=1;
		if(quantile_s!=nooption){
			std::istringstream stm(quantile_s);
			stm>>quantile;
		}
#ifdef _DEBUG
		std::cout << "TestHausdorff begin" << std::endl;
		testHausdorf( truthImg,  testImg,  threshold,  fuzzy,   metricId,  xmlObject, quantile, voxelPreprocessor);
		std::cout << "TestHausdorff end" << std::endl;
#endif
		HausdorffDistanceMetric *hausdorffDistanceMetric = new HausdorffDistanceMetric(truthImg, testImg, fuzzy, threshold);
		value =  hausdorffDistanceMetric->CalcHausdorffDistace(quantile);
		pushValue(metricId, value, xmlObject);
	}

	metricId = AVGDIST;
	if(shouldUse(metricId, options)){
		AverageDistanceMetric *averageDistanceMetric = new AverageDistanceMetric(truthImg, testImg, fuzzy, threshold);
		value =  averageDistanceMetric->CalcAverageDistace(false);
		pushValue(metricId, value, xmlObject);
#ifdef _DEBUG
		std::cout << "TestAverageDistance begin"<<  std::endl;
		FILETIME ft_now;
		GetSystemTimeAsFileTime(&ft_now);
		long long int s1 =(LONGLONG)ft_now.dwLowDateTime + ((LONGLONG)(ft_now.dwHighDateTime) << 32LL);
		AverageDistanceMetric_V3 *averageDistanceMetric3 = new AverageDistanceMetric_V3(truthImg, testImg, fuzzy, threshold); 
		value =  averageDistanceMetric3->CalcAverageDistace(false);
		GetSystemTimeAsFileTime(&ft_now);
		long long int s2 =(LONGLONG)ft_now.dwLowDateTime + ((LONGLONG)(ft_now.dwHighDateTime) << 32LL);
		int milliseconds =  (s2-s1)/10000;
		std::cout << "AVGDST V3:" << value<< " (" << milliseconds <<" sec)"<<  std::endl;
		std::cout << "TestAverageDistance end"<<  std::endl;
#endif
	}

	metricId = MAHLNBS;
	if(shouldUse(metricId, options)){
		MahalanobisDistanceMetric *mahalanobisDistance = new MahalanobisDistanceMetric(truthImg, testImg, voxelPreprocessor, fuzzy, threshold);
		value =  mahalanobisDistance->CalcMahalanobisDistace();
		pushValue(metricId, value, xmlObject);
	}
	metricId = VARINFO;
	if(shouldUse(metricId, options)){
		VariationOfInformationMetric *variationOfInformation = new VariationOfInformationMetric(contingenceTable, fuzzy, threshold);
		value =  variationOfInformation->CalcVariationOfInformation();
		pushValue(metricId, value, xmlObject);
	}


	metricId = GCOERR;
	if(shouldUse(metricId, options)){

		GlobalConsistencyError *globalConsistencyError = new GlobalConsistencyError(contingenceTable, fuzzy, threshold);
		value = globalConsistencyError->CalcGlobalConsistencyError();
		pushValue(metricId, value, xmlObject);
	}

	metricId = PROBDST;
	if(shouldUse(metricId, options)){
		ProbabilisticDistanceMetric *probabilisticDistance = new ProbabilisticDistanceMetric(voxelPreprocessor, fuzzy, threshold);
		value =  probabilisticDistance->CalcJProbabilisticDistance();
		pushValue(metricId, value, xmlObject);
	}

#ifdef _DEBUG
	metricId = MEANTOMEAN;
	if(shouldUse(metricId, options)){
		MeanToMeanDitanceMetric *meanToMeanDitanceMetric = new MeanToMeanDitanceMetric(truthImg, testImg, voxelPreprocessor, fuzzy, threshold);
		value =  meanToMeanDitanceMetric->CalcMeanToMeanDistace();
		pushValue(metricId, value, xmlObject);
	}

	metricId = TEST;
	if(shouldUse(metricId, options)){
        typedef CohenKappaImageToImageMetric<ImageType, ImageType>  CohenKappaMetricType;
        CohenKappaMetricType::Pointer kappaMetric = CohenKappaMetricType::New();
		kappaMetric->SetFixedImage(truthImg);
		kappaMetric->SetMovingImage(testImg);
		value =  kappaMetric->GetValue();
		pushValue(metricId, value, xmlObject);
	}

#endif

	std::cout << "\nClassic Measures:" << std::endl;

	metricId = SNSVTY;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcSensitivity();
		pushValue(metricId, value, xmlObject);
	}


	metricId = SPCFTY;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcSpecificity();
		pushValue(metricId, value, xmlObject);
	}


	metricId = PRCISON;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcPrecision();
		pushValue(metricId, value, xmlObject);
	}

	metricId = FMEASR;
	if(shouldUse(metricId, options)){
		std::string beta_s=getOptions(metricId, options);
		double beta=1;
		if(beta_s!=nooption){
			std::istringstream stm(beta_s);
			stm>>beta;
		}
		value =  classicMeasures->CalcFMeasure(beta);
		pushValue(metricId, value, xmlObject);
	}
	metricId = ACURCY;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcAccuracy();
		pushValue(metricId, value, xmlObject);
	}

	metricId = FALLOUT;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcFallout();
		pushValue(metricId, value, xmlObject);
	}


	SaveXmlObject(xmlObject, targetFile);

	time_t end;
	time(&end);
	double seconds = difftime(end, begin);
	std::cout << "\nExecution time= "<< seconds << " seconds\n"<< std::endl;
	std::cout << "\n  ---** VISCERAL 2013, www.visceral.eu **---\n" << std::endl;	


	return EXIT_SUCCESS;
}



ImageType::Pointer loadImage( const char* filename){
	bool truth_url=isUrl(filename);
	if(truth_url){
		string temp_file = download_image(filename, "__temp_image.nii"); 
		cout << "loading " << filename << std::endl;
		ImageType::Pointer img = loadImage(temp_file.c_str());
	    remove(temp_file.c_str()) ;
		return img;
	} 
	else{
		if(!itksys::SystemTools::FileExists(filename,true)){
			cout << "Image doesn't exist: " << filename << std::endl;
			return 0;
		}
		try
		{
			ImageType::Pointer img ;
            typedef itk::Image<float, 3> FloatImageType;	
			typedef itk::ImageFileReader<FloatImageType> FloatFileReaderType;
			typedef itk::RescaleIntensityImageFilter<FloatImageType,FloatImageType > FloatScalingFilterType;
            typedef itk::CastImageFilter< FloatImageType, ImageType > CastFilterType;
			typedef itk::StreamingImageFilter<ImageType, ImageType> StreamingFilterType;
	        FloatFileReaderType::Pointer reader1 = FloatFileReaderType::New();
	        reader1->SetFileName(filename);
			StreamingFilterType::Pointer streamingFilter = StreamingFilterType::New();
			FloatScalingFilterType::Pointer scalingfilter = FloatScalingFilterType::New();
			scalingfilter->SetOutputMinimum( PIXEL_VALUE_RANGE_MIN );
			scalingfilter->SetOutputMaximum( PIXEL_VALUE_RANGE_MAX );
			scalingfilter->SetInput( reader1->GetOutput() );
            CastFilterType::Pointer castFilter = CastFilterType::New();
            castFilter->SetInput(scalingfilter->GetOutput());
			streamingFilter->SetInput(castFilter->GetOutput());
			streamingFilter->Update();
			img = streamingFilter->GetOutput();

            #ifdef _DEBUG
  		    cout << "saving image .." << std::endl;
			typedef  itk::ImageFileWriter< ImageType  > FileWriterType;
			FileWriterType::Pointer writer = FileWriterType::New();
			writer->SetFileName("debug_image.nii");
			writer->SetInput(streamingFilter->GetOutput());
			writer->Update();
	       cout << "Image saved" << std::endl;
            #endif

			return img;
		}
		catch( itk::ExceptionObject & err )
		{
					std::cerr << "Unable to load image!" << std::endl;
					std::cerr << err << std::endl;
		}


	}


}



#endif



