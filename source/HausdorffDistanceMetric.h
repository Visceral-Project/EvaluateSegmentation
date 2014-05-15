/*
// HausdorffDistanceMetric.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 13.06.2013 
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
// This algorithm is responsible for calculating the Hausdorff distance between two volumes.
// The algorithm uses the ITK Library for accessing the image data (voxels), namely the voxel iterators
// It then performs the rest of the calculation by its own. 
//
*/

#include "itkImage.h"
#include <itkVector.h>


class HausdorffDistanceMetric
{

typedef struct VoxelInfo{
	int x;
	int y;
	int z;
	double value;
} VoxelInfo;

	

		typedef itk::Vector<double, 3> VectorType;
		typedef itk::ImageRegionConstIterator<ImageType> IteratorType;

private:
	ImageType *fixedImage;
	ImageType *movingImage;
	bool fuzzy;
	double threshold;
	bool empty_f;
	bool empty_m;
	int numberElements_f;
	int numberElements_m;
	double maxValue;
	int max_tries;

	
public:
     
	int N1;
	int N2;
	long long int tries;
 	long long int count_less;
	long long int count_more;
	double average;
    double sum;
	double max_dist;
	double min_dist;

	~HausdorffDistanceMetric(){

	}

	HausdorffDistanceMetric(ImageType *fixedImage, ImageType *movingImage, bool fuzzy, double threshold){
		this->fixedImage = fixedImage;
		this->movingImage = movingImage;
		this->fuzzy = fuzzy;
		this->threshold = threshold;
	}


	double CalcHausdorffDistace(double quantile){
		
		std::vector<double> distances1;
		double hd = calc0(fixedImage, movingImage, &distances1);

		if(quantile<1 && distances1.size()>1){
			std::sort(distances1.begin(), distances1.end());
			return distances1[(int)(quantile*(distances1.size() - 1))];
		}
		else{
		   return hd;
		}

	}


	double calc0(ImageType *image1, ImageType *image2, std::vector<double> *distances){

	    double thd = 0;
		if(!fuzzy && threshold!=-1){
		    thd = threshold*PIXEL_VALUE_RANGE_MAX;
		}

		VoxelInfo* retrievedVoxels_1;
		int numberRetr_1;
		VoxelInfo* trueVoxels_1;
		int numberTrue_1;

		VoxelInfo* retrievedVoxels_2;
		int numberRetr_2;
		VoxelInfo* trueVoxels_2;
		int numberTrue_2;

		IteratorType fixedIt(image1, image1->GetRequestedRegion());
		IteratorType movingIt(image2, image2->GetRequestedRegion());

		numberElements_f = 0;
		numberTrue_1=0;
		empty_f=true;
		fixedIt.GoToBegin();
		movingIt.GoToBegin();
		while (!fixedIt.IsAtEnd()){
			numberElements_f++;
			if(fixedIt.Get()>thd && movingIt.Get()<=thd){
				empty_f=false;
				numberTrue_1++;
			}
			++movingIt;
			++fixedIt;
		}
		numberElements_m = 0;
		numberRetr_1=0;
		empty_m=true;
		movingIt.GoToBegin();
		while (!movingIt.IsAtEnd()){
			numberElements_m++;
			if(movingIt.Get()>thd){
				empty_m=false;
				numberRetr_1++;
			}
			++movingIt;
		}
		numberTrue_2=0;
		fixedIt.GoToBegin();
		movingIt.GoToBegin();
		while (!movingIt.IsAtEnd()){
			if(movingIt.Get()>thd && fixedIt.Get()<=thd){
				numberTrue_2++;
			}
			++movingIt;
			++fixedIt;
		}
		numberRetr_2=0;
		fixedIt.GoToBegin();
		while (!fixedIt.IsAtEnd()){
			if(fixedIt.Get()>thd){
				numberRetr_2++;
			}
			++fixedIt;
		}
		retrievedVoxels_1 = new VoxelInfo[numberRetr_1];
		trueVoxels_1 = new VoxelInfo[numberTrue_1];

		fixedIt.GoToBegin();
		movingIt.GoToBegin();
		int FP_index=0;
		int FN_index=0;
		while (!movingIt.IsAtEnd() && !fixedIt.IsAtEnd()){
			if(fixedIt.Get()>thd && movingIt.Get()<=thd){
				trueVoxels_1[FN_index].value = fixedIt.Get();
				trueVoxels_1[FN_index].x = fixedIt.GetIndex()[0];
				trueVoxels_1[FN_index].y = fixedIt.GetIndex()[1];
				trueVoxels_1[FN_index].z = fixedIt.GetIndex()[2];
				FN_index++;
			}
			if(movingIt.Get()>thd){
				retrievedVoxels_1[FP_index].value = movingIt.Get();
				retrievedVoxels_1[FP_index].x = movingIt.GetIndex()[0];
				retrievedVoxels_1[FP_index].y = movingIt.GetIndex()[1];
				retrievedVoxels_1[FP_index].z = movingIt.GetIndex()[2];
				FP_index++;
			}
			++movingIt;
			++fixedIt;
		}


		//------------------ begin

		retrievedVoxels_2 = new VoxelInfo[numberRetr_2];
		trueVoxels_2 = new VoxelInfo[numberTrue_2];

		fixedIt.GoToBegin();
		movingIt.GoToBegin();
		FP_index=0;
		FN_index=0;
		while (!movingIt.IsAtEnd() && !fixedIt.IsAtEnd()){
			if(movingIt.Get()>thd && fixedIt.Get()<=thd){
				trueVoxels_2[FN_index].value = fixedIt.Get();
				trueVoxels_2[FN_index].x = fixedIt.GetIndex()[0];
				trueVoxels_2[FN_index].y = fixedIt.GetIndex()[1];
				trueVoxels_2[FN_index].z = fixedIt.GetIndex()[2];
				FN_index++;
			}
			if(fixedIt.Get()>thd){
				retrievedVoxels_2[FP_index].value = movingIt.Get();
				retrievedVoxels_2[FP_index].x = movingIt.GetIndex()[0];
				retrievedVoxels_2[FP_index].y = movingIt.GetIndex()[1];
				retrievedVoxels_2[FP_index].z = movingIt.GetIndex()[2];
				FP_index++;
			}
			++movingIt;
			++fixedIt;
		}

		maxValue = 100000000;
		double globalmax = 0;
		int index_1=0;
		int index_2=0;
		max_tries=0;
		shuttle(retrievedVoxels_1,numberRetr_1);
        shuttle(retrievedVoxels_2, numberRetr_2);
		shuttle(trueVoxels_1, numberTrue_1);
		shuttle(trueVoxels_2, numberTrue_2);
		while(index_1 < numberTrue_1 || index_2 < numberTrue_2){

			if(index_1 < numberTrue_1){
				VoxelInfo p = trueVoxels_1[index_1];
				double min = maxValue;

				 for(int x=0; x < numberRetr_1 ; x++){
					 VoxelInfo testpoint = retrievedVoxels_1[x];    
 			         double dist =std::sqrt((double)((testpoint.x-p.x)*(testpoint.x-p.x) + (testpoint.y-p.y)*(testpoint.y-p.y) + (testpoint.z-p.z)*(testpoint.z-p.z)));
					 min = std::min(min, dist);
					 if(dist < globalmax){
					     break;
					 } else{
					     //std::cout <<"dist" <<dist<< std::endl; 
					 }
					 if(dist > 15.2971){
						 count_more++;
					 }
					 else{
					   count_less++;
					 }
				 }
				 distances->push_back(min);
				  if(min > globalmax){
						globalmax  = min;
				  }
				index_1++;
			}

			if(index_2 < numberTrue_2){

				VoxelInfo p = trueVoxels_2[index_2];
				double min = maxValue;
				 for(int x=0; x < numberRetr_2 ; x++){
					 VoxelInfo testpoint = retrievedVoxels_2[x];    
 			         double dist =std::sqrt((double)((testpoint.x-p.x)*(testpoint.x-p.x) + (testpoint.y-p.y)*(testpoint.y-p.y) + (testpoint.z-p.z)*(testpoint.z-p.z)));
					 min = std::min(min, dist);
					 if(dist < globalmax){
					     break;
					 }
					 else{
					    // std::cout <<"dist" <<dist<< std::endl; 
					 }
				 }
				 distances->push_back(min);
				  if(min > globalmax){
						globalmax  = min;
				  }
				index_2++;
			}
		}

		//  std::cout << " true "<< numberTrue_1<< std::endl;
		return globalmax;
	}

	double s1(double s01, double max, int maxMax, VoxelInfo p, VoxelInfo *comparretriecedVoxels , int step){
		double min=maxValue;
		s01 = maxValue;
		int x = 0;
		for(int j=0 ; j< maxMax && s01 > max; j=j+step){
			VoxelInfo testpoint = comparretriecedVoxels[j];    
			double dist =std::sqrt((double)((testpoint.x-p.x)*(testpoint.x-p.x) + (testpoint.y-p.y)*(testpoint.y-p.y) + (testpoint.z-p.z)*(testpoint.z-p.z)));
			x++;
			if(dist<max){
				max_tries = std::max(x,max_tries);
                //std::cout << x<< std::endl;
				return -1;

			}
			else{
			   min = std::min(dist, min);
			}
			
		}

		if(step > 1){
			//std::cout << "--------->" << step << std::endl;
		   return -2;
		}
		return min;
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


	VectorType calcMean(IteratorType it){
		VectorType mat;
		mat.Fill(0); 
		double count=0;
		it.GoToBegin();
		while (!it.IsAtEnd()){
			double val = it.Value();
			if(val!=0){
				ImageType::IndexType index = it.GetIndex();
				mat[0] += index[0];
				mat[1] += index[1];
				mat[2] += index[2];
				count++;
			}
			++it;
		}
		mat = mat/count;
		return mat;
	}

	void shuttle(VoxelInfo* arr, int len){
		   srand (time(NULL));
		   for(int i=0 ; i< len ; i++){
			   int r1 = std::abs(rand()*rand() + rand());
		       r1 = r1 %len;
			   VoxelInfo v1 = arr[i];
			   VoxelInfo v2 = arr[r1];
			   arr[i]=v2;
               arr[r1]=v1;

		   }
	}


};

