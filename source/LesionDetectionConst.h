
#ifndef _LESION_DETECTION_CONST
#define _LESION_DETECTION_CONST

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"


#include "itkOutputWindow.h"
#include "itkTextOutput.h"

#include <exception>
#include <iostream>
#include <string> 

typedef itk::ImageRegionConstIterator<ImageType> ImageReaderIteratorType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageRegionConstIterator<ImageType> ImageReaderIteratorType;
typedef itk::Point< double, ImageType::ImageDimension > PointType;

static int REGION_ID_BACKGROUND=0;
static int REGION_ID_BONE=1;
static int REGION_ID_BRAIN=2;
static int REGION_ID_LUNG=3;
static int REGION_ID_LEVER=4;
static int REGION_ID_LYMPHNODE=5;

static int POINTTYPE_CENTER=1;
static int POINTTYPE_D1=2;
static int POINTTYPE_D2=3;

static bool CHECK_TEST_LESIONS_FOR_ROI=true;

typedef struct GTLesion{
	string label;
	int region_id;	
	int counter;
	double m_x;
	double m_y;
	double m_z;
	double d1_x;
	double d1_y;
	double d1_z;
	double d2_x;
	double d2_y;
	double d2_z;

} GTLesion;

typedef struct TestLesion{
	string label;
	int region_id;	
	int counter;
	double x;
	double y;
	double z;

} TestLesion;

int count_groundtruth_lesions_total=0;
int count_lymphnode_lesions=0;
int count_test_lesions_total=0;
int count_test_lesions_excluded=0;
int count_lesion_detected=0;
int count_lymphnode_detected=0;


#endif