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
	~ImageStatistics(){

	}

	ImageStatistics(ImageType *fixedImage, ImageType *movingImage, bool fuzzy, double threshold){
		typedef itk::ImageRegionConstIterator<ImageType> FixedIteratorType;
		typedef itk::ImageRegionConstIterator<ImageType> MovingIteratorType;

		FixedIteratorType fixedIt(fixedImage, fixedImage->GetRequestedRegion());

		MovingIteratorType movingIt(movingImage, movingImage->GetRequestedRegion());

		numberElements_f = 0;
		num_nonzero_points_f=0;
		fixedIt.GoToBegin();
		while (!fixedIt.IsAtEnd()){
			numberElements_f++;
			if(fixedIt.Get()!=0){
				num_nonzero_points_f++;
			}
			++fixedIt;
		}

		numberElements_m = 0;
		num_nonzero_points_m=0;
		movingIt.GoToBegin();
		while (!movingIt.IsAtEnd()){
			numberElements_m++;
		   if(movingIt.Get()!=0){
				num_nonzero_points_m++;
			}
			++movingIt;
		}

		num_intersection=0;
		movingIt.GoToBegin();
		fixedIt.GoToBegin();
		while (!movingIt.IsAtEnd() && !fixedIt.IsAtEnd()){
		   if(movingIt.Get()!=0 && fixedIt.Get()!=0){
				num_intersection++;
			}
			++movingIt;
			++fixedIt;
		}
	}



};

#endif
