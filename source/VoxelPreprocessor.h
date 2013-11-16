/*
// VoxelProcessor.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 20.03.2013 
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
// This class performs some preprocessing on the volumes to be compared such as counting 
// the voxels and saving them in the memory. The class is used from almost all evaluation algorithms. 
//
*/

#ifndef _VOXELPROCESSOR
#define _VOXELPROCESSOR
#include "itkImage.h"
#include "ImageStatistics.h"

class VoxelPreprocessor
{
private:
	bool empty_f;
	bool empty_m;	
public: 
	int numberElements_f;
	int numberElements_m;
	int num_nonzero_points_f;
	int num_nonzero_points_m;
	int num_intersection;
	double mean_f;
	double mean_m;
	~VoxelPreprocessor(){

	}
	VoxelPreprocessor(ImageType *fixedImage, ImageType *movingImage, bool fuzzy, double threshold, ImageStatistics *imagestatistics){
		typedef itk::ImageRegionConstIterator<ImageType> FixedIteratorType;
		typedef itk::ImageRegionConstIterator<ImageType> MovingIteratorType;
		
		this->numberElements_f = imagestatistics->numberElements_f;
		this->numberElements_m = imagestatistics->numberElements_m;
		this->num_nonzero_points_f = imagestatistics->num_nonzero_points_f;
		this->num_nonzero_points_m = imagestatistics->num_nonzero_points_m;
		this->num_intersection = imagestatistics->num_intersection;

		FixedIteratorType fixedIt(fixedImage, fixedImage->GetRequestedRegion());

		MovingIteratorType movingIt(movingImage, movingImage->GetRequestedRegion());

		int ind=0;
		double sum_f = 0;
		empty_f=true;
        fixedIt.GoToBegin();
		while (!fixedIt.IsAtEnd()){
			  //std::cout << "i:" << ind << std::endl;
			if(fuzzy){
				values_f[ind] = fixedIt.Value();
				if(values_f[ind]>PIXEL_VALUE_RANGE_MAX)
					values_f[ind] = PIXEL_VALUE_RANGE_MAX;
				if(values_f[ind]<PIXEL_VALUE_RANGE_MIN)
					values_f[ind] = PIXEL_VALUE_RANGE_MIN;

			}
			else{
				values_f[ind] =  fixedIt.Value()<(threshold*PIXEL_VALUE_RANGE_MAX)?PIXEL_VALUE_RANGE_MIN:PIXEL_VALUE_RANGE_MAX;
				
			}

			if(values_f[ind] !=0){
					empty_f=false;
			} 

			sum_f += ((double)values_f[ind])/((double)PIXEL_VALUE_RANGE_MAX);
			ind++;
			++fixedIt;
		}

		movingIt.GoToBegin();
		double sum_m = 0;
		empty_m=true;
		ind=0; 
	    while (!movingIt.IsAtEnd()){
			if(fuzzy){
				values_m[ind] = movingIt.Value();
				if(values_m[ind]>PIXEL_VALUE_RANGE_MAX)
					values_m[ind] = PIXEL_VALUE_RANGE_MAX;
				if(values_m[ind]<PIXEL_VALUE_RANGE_MIN)
					values_m[ind] = PIXEL_VALUE_RANGE_MIN;
	
			}
			else{
				values_m[ind] = movingIt.Value()<(threshold*PIXEL_VALUE_RANGE_MAX)?PIXEL_VALUE_RANGE_MIN:PIXEL_VALUE_RANGE_MAX;
			}
			if(values_m[ind] !=0){
				empty_m=false;
			} 


			sum_m += ((double)values_m[ind])/((double)PIXEL_VALUE_RANGE_MAX);
			ind++;
			++movingIt;
		}

		////-----------------
		mean_f = sum_f/numberElements_f;
		mean_m = sum_m/numberElements_m;
	}
	int GetFixedImageVoxelCount(){
		return numberElements_f;
	}
	int GetMovingImageVoxelCount(){
		return numberElements_m;
	}
	bool IsDifferentImageSize(){
		return numberElements_f != numberElements_m;
	}
	bool IsFixedImageEmpty(){
		return empty_f;
	}
	bool IsMovingImageEmpty(){
		return empty_m;
	}
};

#endif
