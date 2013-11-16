/*
// MahalanobisDistanceMetric.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 18.04.2013 
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
// This algorithm is responsible for calculating the Mahalanobis Distance Metric between two volumes.
// The algorithm uses the ITK Library for accessing the image data (voxels), namely the voxel iterators
// It then performs the rest of the calculation by its own. 
//
*/


#include "itkImage.h"
#include <itkMatrix.h>
#include <itkVector.h>

class MahalanobisDistanceMetric
{

	typedef itk::ImageRegionConstIterator<ImageType> IteratorType;
	typedef itk::Vector<double, 3> VectorType;
	typedef itk::Matrix<double, 3, 3> MatrixType;
	typedef itk::Matrix<double, 3, 1> VectorMatrixType;

private:
	VoxelPreprocessor *voxelprocesser;
	bool fuzzy;
	double threshold;
	ImageType *fixedImage; 
	ImageType *movingImage;

public: 

	~MahalanobisDistanceMetric(){

	}

	MahalanobisDistanceMetric(ImageType *fixedImage, ImageType *movingImage, VoxelPreprocessor *voxelprocesser, bool fuzzy, double threshold){
		this->voxelprocesser= voxelprocesser;
		this->fixedImage=fixedImage;
		this->movingImage=movingImage;
		this->fuzzy = fuzzy;
		this->threshold = threshold;
	}

	double CalcMahalanobisDistace(){
		IteratorType fixedIt(fixedImage, fixedImage->GetRequestedRegion());
		IteratorType movingIt(movingImage, movingImage->GetRequestedRegion());
		VectorType means_f = calcMean(fixedIt);
		VectorType means_m = calcMean(movingIt);
		MatrixType covariace_f = calcCovariance(fixedIt, means_f);
		MatrixType covariace_m  = calcCovariance(movingIt, means_m);
		long int len_f=getImageSize(fixedIt);
		long int len_m=getImageSize(movingIt);
		MatrixType covariace_mat = commonCovarianceMatrix(covariace_f, covariace_m, len_f, len_m);
		MatrixType covariace_mat_inv  = (MatrixType)covariace_mat.GetInverse();
		return mahalanobis_dist(means_f, means_m, covariace_mat_inv);
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

	long int getImageSize(IteratorType it){
		int len=0;
		it.GoToBegin();
		while (!it.IsAtEnd()){
			double val = it.Value();
			if(val!=0){
				len++;
			}
			++it;
		}
		return len;
	}

	MatrixType calcCovariance(IteratorType it, VectorType means){
		MatrixType covariance;
		covariance.Fill(0);
		
		double sampleCount=0;
		it.GoToBegin();
		while (!it.IsAtEnd()){
			double val = it.Value();
			if(val!=0){
				ImageType::IndexType index = it.GetIndex();
				VectorType delta;
				delta[0] = index[0] -  means[0];
				delta[1] = index[1] -  means[1];
				delta[2] = index[2] -  means[2];
				MatrixType dot;
				for(int i=0 ; i < 3; i++){
					for(int j=0 ;j<3 ; j++){
						dot(i,j) = delta[i]*delta[j];
					}
				}
				covariance += dot;
				sampleCount++;
			}
			++it;
		}
		covariance /= sampleCount;
		return covariance;
	}

	
	MatrixType commonCovarianceMatrix(MatrixType covmat1, MatrixType  covmat2, long int len1, long int len2){
		MatrixType  covmat;
		covmat = (covmat1*len1+ covmat2*len2)/(len1+len2) ;
		return covmat;
	}


	double mahalanobis_dist(VectorType means1, VectorType means2, MatrixType covmat_inv){
		double val=0;
		VectorType temp;
		temp[0]= 0;
		temp[1]= 0;
		temp[2]= 0;
		VectorType delta = means1-means2;
		for(int i=0 ; i < 3; i++){
			for(int j=0 ;j<3 ; j++){
				temp[i] += delta[j] * covmat_inv(i,j);
			}
		}
		val = temp * delta;
		return val;

	}

};

