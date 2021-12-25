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
#include "AdditionalTestMetrics.h"
#endif

using namespace std;

int validateImage(const char* f1, const char* f2, double threshold, const char* targetFile, char *options);
bool shouldUse(MetricId, char* options);
std::string getOptions(MetricId id, char* options);
bool isUrl(const char* path);
void testHausdorf(ImageType::Pointer truthImg, ImageType::Pointer testImg, double threshold, bool fuzzy, MetricId metricId, itk::DOMNode::Pointer xmlObject, int option, VoxelPreprocessor *);
const std::string nooption = "NOOPTION";
ImageType::Pointer loadImage(const char* filename,  bool useStreamingFilter);


static int validateImage(const char* f1, const char* f2, double threshold, const char* targetFile, char *options, const char* unit, long long int time_start, bool useStreamingFilter)
{

    bool use_millimeter=false;
	string units = unit;
	int pos = units.find("millimeter");
	if(pos != string::npos){
	    use_millimeter = true;
	}
	VoxelPreprocessor *voxelPreprocessor;
	ImageStatistics *imagestatistics;

	bool fuzzy  = (threshold == -1);
	if(!fuzzy){
		std::cout << "Crisp segmentation at threshold= " << threshold << "\n" << std::endl;
	}
	ImageType::Pointer truthImg = loadImage(f1, useStreamingFilter);
	if(truthImg ==  0){
		return EXIT_FAILURE;
	}

	ImageType::Pointer testImg = loadImage(f2, useStreamingFilter);

	if(testImg ==  0){
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

#ifdef _DEBUG
	string opt = options;
	int ind1 = opt.find("TESTMETRICS");
	if(ind1 != string::npos){
		std::cout << "TestMetrics begin" << std::endl;
		double quantile = 1;
		StartAdditionalTestMetrics( truthImg,  testImg,  threshold,  fuzzy,   xmlObject, options, voxelPreprocessor);
		std::cout << "TestMetrics end" << std::endl;	
		
	    clock_t t = clock();
        long long time_end= ((double)t*1000)/CLOCKS_PER_SEC;	
		int total_milliseconds =  (time_end-time_start);
		std::cout << "\nTotal execution time= "<< total_milliseconds << " milliseconds\n"<< std::endl;
		std::cout << "\n  ---** VISCERAL 2013, www.visceral.eu **---\n" << std::endl;	
		pushTotalExecutionTime(total_milliseconds, xmlObject);
		pushDimentions(imagestatistics->max_x_f, imagestatistics->max_y_f, imagestatistics->max_z_f, xmlObject);
		
		SaveXmlObject(xmlObject, targetFile);
		return EXIT_SUCCESS;
	}
#endif


  
	//pushUnit(use_millimeter, xmlObject);
	
       

	std::cout << "Similarity:" << std::endl;
	double value=0;
	MetricId metricId = DICE;
	if(shouldUse(metricId, options)){
		DiceCoefficientMetric *diceCoefficient = new DiceCoefficientMetric(contingenceTable, fuzzy, threshold);
		value = diceCoefficient->CalcDiceCoeff();
		pushValue(metricId, value, xmlObject,false, NULL);
	}

	metricId = JACRD;
	if(shouldUse(metricId, options)){
		JaccardCoefficientMetric *jaccardCoefficient = new JaccardCoefficientMetric(contingenceTable, fuzzy, threshold);
		value =  jaccardCoefficient->CalcJaccardCoeff();
		pushValue(metricId, value, xmlObject,false, NULL);
	}

	ClassicMeasures *classicMeasures = new ClassicMeasures(contingenceTable, fuzzy, threshold);
	metricId = AUC;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcAUC();
		pushValue(metricId, value, xmlObject,false, NULL);
	}


	metricId = KAPPA;
	if(shouldUse(metricId, options)){
		CohinKappaMetric *cohinKappa = new CohinKappaMetric(contingenceTable, fuzzy, threshold);
		value =  cohinKappa->CalcCohenKappa();
		pushValue(metricId, value, xmlObject,false, NULL);
	}

	metricId = RNDIND;
	RandIndexMetric *randIndex = new RandIndexMetric(contingenceTable, fuzzy, threshold);
	if(shouldUse(metricId, options)){
		value =  randIndex->CalcRandIndex();
		pushValue(metricId, value, xmlObject,false, NULL);
	}
	metricId = ADJRIND;
	if(shouldUse(metricId, options)){
		value =  randIndex->CalcAdjustedRandIndex();
		pushValue(metricId, value, xmlObject,false, NULL);
	}


	metricId = ICCORR;
	if(shouldUse(metricId, options)){
		InterclassCorrelationMetric *interclassCorrelation = new InterclassCorrelationMetric(voxelPreprocessor, fuzzy, threshold);
		value =  interclassCorrelation->CalcInterClassCorrelationCoeff();
		pushValue(metricId, value, xmlObject,false, NULL);
	}

	metricId = VOLSMTY;
	if(shouldUse(metricId, options)){
		VolumeSimilarityCoefficient *volumetricSimilarity = new VolumeSimilarityCoefficient(contingenceTable, fuzzy, threshold);
		value =  volumetricSimilarity->CalcVolumeSimilarityCoefficient();
		pushValue(metricId, value, xmlObject,false, NULL);
	}

	metricId = MUTINF;
	if(shouldUse(metricId, options)){
		MutualInformationMetric *mutualInformation = new MutualInformationMetric(contingenceTable, fuzzy, threshold);
		value =  mutualInformation->CalcMutualInformation();
		pushValue(metricId, value, xmlObject,false, NULL);
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
	    clock_t t = clock();
        long long s1= ((double)t*1000)/CLOCKS_PER_SEC;	

	    double quantile=1;
		if(quantile_s!=nooption){
			std::istringstream stm(quantile_s);
			stm>>quantile;
		}
		HausdorffDistanceMetric *hausdorffDistanceMetric = new HausdorffDistanceMetric(truthImg, testImg, fuzzy, threshold, use_millimeter);  
		value =  hausdorffDistanceMetric->CalcHausdorffDistace(quantile);
	    t = clock();
        long long s2= ((double)t*1000)/CLOCKS_PER_SEC;	

	int milliseconds =  (s2-s1);
	    if(use_millimeter){
		   pushValue(metricId, value, milliseconds, xmlObject, "millimeter");
	    }
		else{
		    pushValue(metricId, value, milliseconds, xmlObject, "voxel");
		}
	}

	metricId = AVGDIST;
	if(shouldUse(metricId, options)){
	    clock_t t = clock();
        long long s1= ((double)t*1000)/CLOCKS_PER_SEC;	
		AverageDistanceMetric *averageDistanceMetric = new AverageDistanceMetric(truthImg, testImg, fuzzy, threshold, use_millimeter);
		value =  averageDistanceMetric->CalcAverageDistace(false);
	    t = clock();
        long long s2= ((double)t*1000)/CLOCKS_PER_SEC;	
		int milliseconds =  (s2-s1);
		if(use_millimeter){
		    pushValue(metricId, value, milliseconds, xmlObject, "millimeter");
		}
		else{
		    pushValue(metricId, value, milliseconds, xmlObject, "voxel");
		}
		
	}
	
	/*
	metricId = AVGDIST_D;
	if(shouldUse(metricId, options)){
	    clock_t t = clock();
        long long s1= ((double)t*1000)/CLOCKS_PER_SEC;	
		AverageDistanceMetric *averageDistanceMetric = new AverageDistanceMetric(truthImg, testImg, fuzzy, threshold, use_millimeter);
		value =  averageDistanceMetric->CalcAverageDistaceDirected();
	    t = clock();
        long long s2= ((double)t*1000)/CLOCKS_PER_SEC;	
		int milliseconds =  (s2-s1);
		if(use_millimeter){
		    pushValue(metricId, value, milliseconds, xmlObject, "millimeter");
		}
		else{
		    pushValue(metricId, value, milliseconds, xmlObject, "voxel");
		}
		
	}
	*/
	
	metricId = bAVD;
	if(shouldUse(metricId, options)){
	    clock_t t = clock();
        long long s1= ((double)t*1000)/CLOCKS_PER_SEC;	
		AverageDistanceMetric *averageDistanceMetric = new AverageDistanceMetric(truthImg, testImg, fuzzy, threshold, use_millimeter);
		value =  averageDistanceMetric->CalcBalancedAverageDistace();
	    t = clock();
        long long s2= ((double)t*1000)/CLOCKS_PER_SEC;	
		int milliseconds =  (s2-s1);
		if(use_millimeter){
		    pushValue(metricId, value, milliseconds, xmlObject, "millimeter");
		}
		else{
		    pushValue(metricId, value, milliseconds, xmlObject, "voxel");
		}
		
	}
	
	

	metricId = MAHLNBS;
	if(shouldUse(metricId, options)){
		MahalanobisDistanceMetric *mahalanobisDistance = new MahalanobisDistanceMetric(truthImg, testImg, voxelPreprocessor, fuzzy, threshold);
		value =  mahalanobisDistance->CalcMahalanobisDistace();
		pushValue(metricId, value, xmlObject,false, NULL);
	}
	metricId = VARINFO;
	if(shouldUse(metricId, options)){
		VariationOfInformationMetric *variationOfInformation = new VariationOfInformationMetric(contingenceTable, fuzzy, threshold);
		value =  variationOfInformation->CalcVariationOfInformation();
		pushValue(metricId, value, xmlObject,false, NULL);
	}


	metricId = GCOERR;
	if(shouldUse(metricId, options)){

		GlobalConsistencyError *globalConsistencyError = new GlobalConsistencyError(contingenceTable, fuzzy, threshold);
		value = globalConsistencyError->CalcGlobalConsistencyError();
		pushValue(metricId, value, xmlObject,false, NULL);
	}

	metricId = PROBDST;
	if(shouldUse(metricId, options)){
		ProbabilisticDistanceMetric *probabilisticDistance = new ProbabilisticDistanceMetric(voxelPreprocessor, fuzzy, threshold);
		value =  probabilisticDistance->CalcJProbabilisticDistance();
		pushValue(metricId, value, xmlObject,false, NULL);
	}


	std::cout << "\nClassic Measures:" << std::endl;

	metricId = SNSVTY;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcSensitivity();
		pushValue(metricId, value, xmlObject,false, NULL);
	}


	metricId = SPCFTY;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcSpecificity();
		pushValue(metricId, value, xmlObject,false, NULL);
	}


	metricId = PRCISON;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcPrecision();
		pushValue(metricId, value, xmlObject,false, NULL);
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
		pushValue(metricId, value, xmlObject,false, NULL);
	}
	metricId = ACURCY;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcAccuracy();
		pushValue(metricId, value, xmlObject,false, NULL);
	}

	metricId = FALLOUT;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->CalcFallout();
		pushValue(metricId, value, xmlObject,false, NULL);
	}

	metricId = TP;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->getTP();
		pushValue(metricId, value, xmlObject,true, "voxel");
	}

	metricId = FP;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->getFP();
		pushValue(metricId, value, xmlObject,true, "voxel");
	}

	metricId = TN;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->getTN();
		pushValue(metricId, value, xmlObject,true, "voxel");
	}

	metricId = FN;
	if(shouldUse(metricId, options)){
		value =  classicMeasures->getFN();
		pushValue(metricId, value, xmlObject,true, "voxel");
	}

	metricId = REFVOL;
	if(shouldUse(metricId, options)){
		if(use_millimeter){
			value =  classicMeasures->CalcReferenceVolumeInMl();
		    pushValue(REFVOL, value, xmlObject,false, "milliliter");
		}
		else{
			value =  classicMeasures->CalcReferenceVolumeInVoxel();
		    pushValue(REFVOL, value, xmlObject,true, "voxel");
		}
		
	}
	
	metricId = SEGVOL;
	if(shouldUse(metricId, options)){
		if(use_millimeter){
			value =  classicMeasures->CalcSegmentedVolumeInMl();
		    pushValue(SEGVOL, value, xmlObject,false, "milliliter");
		}
		else{
			value =  classicMeasures->CalcSegmentedVolumeInVoxel();
		    pushValue(SEGVOL, value, xmlObject,true, "voxel");
		}
		
	}
	
	
	clock_t t = clock();
    long long time_end= ((double)t*1000)/CLOCKS_PER_SEC;	
	int total_milliseconds =  (time_end-time_start);
	std::cout << "\nTotal execution time= "<< total_milliseconds << " milliseconds\n"<< std::endl;
	std::cout << "\n  ---** VISCERAL 2013, www.visceral.eu **---\n" << std::endl;	
	pushTotalExecutionTime(total_milliseconds, xmlObject);
	pushDimentions(imagestatistics->max_x_f, imagestatistics->max_y_f, imagestatistics->max_z_f, imagestatistics->vspx, imagestatistics->vspy, imagestatistics->vspz, xmlObject);
	SaveXmlObject(xmlObject, targetFile);

	return EXIT_SUCCESS;
}



ImageType::Pointer loadImage( const char* filename, bool useStreamingFilter){
	bool truth_url=isUrl(filename);
	if(truth_url){
		string temp_file = download_image(filename, "__temp_image.nii"); 
		cout << "loading " << filename << std::endl;
		ImageType::Pointer img = loadImage(temp_file.c_str(), useStreamingFilter);
		remove(temp_file.c_str()) ;
		return img;
	} 
	else{
		if(itksys::SystemTools::FileExists(filename,true)){
			
			
		
		try
		{
			ImageType::Pointer img ;
			if(useStreamingFilter){
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
			}
			else{
				typedef itk::Image<float, 3> FloatImageType;	
				typedef itk::ImageFileReader<FloatImageType> FloatFileReaderType;
				typedef itk::RescaleIntensityImageFilter<FloatImageType,FloatImageType > FloatScalingFilterType;
				typedef itk::CastImageFilter< FloatImageType, ImageType > CastFilterType;
				FloatFileReaderType::Pointer reader1 = FloatFileReaderType::New();
				reader1->SetFileName(filename);
				FloatScalingFilterType::Pointer scalingfilter = FloatScalingFilterType::New();
				scalingfilter->SetOutputMinimum( PIXEL_VALUE_RANGE_MIN );
				scalingfilter->SetOutputMaximum( PIXEL_VALUE_RANGE_MAX );
				scalingfilter->SetInput( reader1->GetOutput() );
				CastFilterType::Pointer castFilter = CastFilterType::New();
				castFilter->SetInput(scalingfilter->GetOutput());
				castFilter->Update();
				img = castFilter->GetOutput();
			}

#ifdef _DEBUG
			/*
			cout << "saving image .." << std::endl;
			typedef  itk::ImageFileWriter< ImageType  > FileWriterType;
			FileWriterType::Pointer writer = FileWriterType::New();
			writer->SetFileName("debug_image.nii");
			writer->SetInput(streamingFilter->GetOutput());
			writer->Update();
			cout << "Image saved" << std::endl;
			*/
#endif

			return img;
		}
		catch( itk::ExceptionObject & err )
		{
			std::cerr << "Unable to load image!" << std::endl;
			std::cerr << err << std::endl;
		}
		}
		else{
			
			cout << "Image doesn't exist: " << filename << std::endl;
		}

	}
	return ITK_NULLPTR;
}



#endif



