/*
// ImageStatistics.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 08.11.2013 
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
// This class counts some statistics like number of pixels etc.
//
*/

#ifndef _IMAGESTATISTICS
#define _IMAGESTATISTICS
#include "itkImage.h"

class ImageStatistics
{
	

public: 
	int numberElements_f;
	int numberElements_m;
	int num_nonzero_points_f;
	int num_nonzero_points_m;
	int num_intersection;
	int max_x_f;
	int max_y_f;
	int max_z_f;
	int max_x_m;
	int max_y_m;
	int max_z_m;

    double vspx; // Voxelspacing x
    double vspy; // Voxelspacing y
    double vspz; // Voxelspacing z

	~ImageStatistics(){

	}

	ImageStatistics(ImageType *fixedImage, ImageType *movingImage, bool fuzzy, double threshold){
		typedef itk::ImageRegionConstIterator<ImageType> FixedIteratorType;
		typedef itk::ImageRegionConstIterator<ImageType> MovingIteratorType;
		FixedIteratorType fixedIt(fixedImage, fixedImage->GetRequestedRegion());
		MovingIteratorType movingIt(movingImage, movingImage->GetRequestedRegion());


        const ImageType::SpacingType & ImageSpacing = fixedImage->GetSpacing();
		this->vspx = ImageSpacing[0];
		this->vspy = ImageSpacing[1];
		this->vspz = ImageSpacing[2];

		if(this->vspx==0){
			this->vspx=1;
		}
		if(this->vspy==0){
			this->vspy=1;
		}
		if(this->vspz==0){
			this->vspz=1;
		}

		double thd = 0;
		if(!fuzzy && threshold!=-1){
		    thd = threshold*PIXEL_VALUE_RANGE_MAX;
		}
		numberElements_f = 0;
		num_nonzero_points_f=0;
		fixedIt.GoToBegin();
	    max_x_f = 0;
	    max_y_f = 0;
	    max_z_f = 0;
	    max_x_m = 0;
	    max_y_m = 0;
	    max_z_m = 0;
		while (!fixedIt.IsAtEnd()){
			numberElements_f++;
			if(fixedIt.Get()>thd){
				num_nonzero_points_f++;
			}

			ImageType::IndexType ind = fixedIt.GetIndex();
			if(ind[0]>max_x_f){
			   max_x_f=ind[0];
			}
			if(ind[1]>max_y_f){
			   max_y_f=ind[1];
			}
			if(ind[2]>max_z_f){
			   max_z_f=ind[2];
			}
			++fixedIt;
		}

		numberElements_m = 0;
		num_nonzero_points_m=0;
		movingIt.GoToBegin();
		while (!movingIt.IsAtEnd()){
			numberElements_m++;
		   if(movingIt.Get()>thd){
				num_nonzero_points_m++;
			}
		   ImageType::IndexType ind = movingIt.GetIndex();
			if(ind[0]>max_x_m){
			   max_x_m=ind[0];
			}
			if(ind[1]>max_y_m){
			   max_y_m=ind[1];
			}
			if(ind[2]>max_z_m){
			   max_z_m=ind[2];
			}

			++movingIt;
		}

		num_intersection=0;
		movingIt.GoToBegin();
		fixedIt.GoToBegin();
		while (!movingIt.IsAtEnd() && !fixedIt.IsAtEnd()){
		   if(movingIt.Get()>thd && fixedIt.Get()>thd){
				num_intersection++;
			}
			++movingIt;
			++fixedIt;
		}
	}



};

#endif
