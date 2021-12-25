
#ifndef _LESION_DETECTION_MASK
#define _LESION_DETECTION_MASK

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

class LesionDetectionMask
{

private:
	ImageType::Pointer groundtruth_img;


public:

	~LesionDetectionMask(){

	}

	LesionDetectionMask(const char* filename){
		readImage(filename);
	}


	void readImage(const char*  filename){
		ReaderType::Pointer groundtruth_reader = ReaderType::New();
		groundtruth_reader->SetFileName(filename);
		groundtruth_reader->Update();
		groundtruth_img = groundtruth_reader->GetOutput();
	}



	bool isGroundtruthLesionInsideRegion(GTLesion * gt_lesion){
		if(gt_lesion->region_id == REGION_ID_LYMPHNODE)
			return true;
		ImageReaderIteratorType mask_iterator(groundtruth_img, groundtruth_img->GetRequestedRegion());
		PointType point;


		//ImageType::DirectionType direction = groundtruth_img->GetDirection();
		//cout << direction << " direction\n";

		point[0] = -gt_lesion->m_x; 
		point[1] = -gt_lesion->m_y; 
		point[2] = gt_lesion->m_z; 
		ImageType::IndexType testindex;
		const bool isInside = groundtruth_img->TransformPhysicalPointToIndex( point, testindex );
		//cout << "Index for point (" << point[0] << ", " << point[1] << ", " << point[2] << ") is (" << testindex << ")\n"; 
		if(isInside){
			mask_iterator.GoToBegin();
			while (!mask_iterator.IsAtEnd()){
				if(mask_iterator.Get()==gt_lesion->region_id){
					ImageType::IndexType index = mask_iterator.GetIndex();
					int i= index[0];
					int j= index[1];
					int k= index[2];
					if(i==testindex[0] && j==testindex[1] && k==testindex[2]){
						return true;
					}
				}
				++mask_iterator;
			}
		}
		return false;
	}


	bool isTestLesionInsideRegion(TestLesion * testlesion){
		if(!CHECK_TEST_LESIONS_FOR_ROI){
		    return true;
		}

		ImageReaderIteratorType mask_iterator(groundtruth_img, groundtruth_img->GetRequestedRegion());
		PointType point;
		point[0] = -testlesion->x; 
		point[1] = -testlesion->y; 
		point[2] = testlesion->z; 
		ImageType::IndexType testindex;
		const bool isInside = groundtruth_img->TransformPhysicalPointToIndex( point, testindex );
		if(isInside){
			mask_iterator.GoToBegin();
			while (!mask_iterator.IsAtEnd()){
				if(mask_iterator.Get() >= 1){
					ImageType::IndexType index = mask_iterator.GetIndex();
					int i= index[0];
					int j= index[1];
					int k= index[2];
					if(i==testindex[0] && j==testindex[1] && k==testindex[2]){
						return true;
					}
				}
				++mask_iterator;
			}
		}
		return false;
	}

};

#endif
