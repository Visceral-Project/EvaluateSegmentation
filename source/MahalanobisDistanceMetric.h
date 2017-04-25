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

	typedef itk::Vector<double, 3> VectorType3D;
	typedef itk::Matrix<double, 3, 3> MatrixType3D;

	typedef itk::Vector<double, 2> VectorType2D;
	typedef itk::Matrix<double, 2, 2> MatrixType2D;


private:
	VoxelPreprocessor *voxelprocesser;
	bool fuzzy;
	double threshold;
	double thd;
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

	    thd = 0;
		if(!fuzzy && threshold!=-1){
		    thd = threshold*PIXEL_VALUE_RANGE_MAX;
		}

	}

	double CalcMahalanobisDistace(){
		IteratorType fixedIt(fixedImage, fixedImage -> GetRequestedRegion());
		IteratorType movingIt(movingImage, movingImage -> GetRequestedRegion());
		long int len_f=getImageSize(fixedIt);
		long int len_m=getImageSize(movingIt);
		bool image_2d = Is2DImage(fixedIt);

		if(image_2d){
			VectorType2D means_f = calcMean2D(fixedIt);
			VectorType2D means_m = calcMean2D(movingIt);
			MatrixType2D covariace_f = calcCovariance2D(fixedIt, means_f);
			MatrixType2D covariace_m  = calcCovariance2D(movingIt, means_m);
			MatrixType2D covariace_mat = commonCovarianceMatrix2D(covariace_f, covariace_m, len_f, len_m);
			MatrixType2D covariace_mat_inv  = (MatrixType2D)covariace_mat.GetInverse();
			return mahalanobis_dist2D(means_f, means_m, covariace_mat_inv);

		}
		else{
			VectorType3D means_f = calcMean3D(fixedIt);
			VectorType3D means_m = calcMean3D(movingIt);
			MatrixType3D covariace_f = calcCovariance3D(fixedIt, means_f);
			MatrixType3D covariace_m  = calcCovariance3D(movingIt, means_m);
			MatrixType3D covariace_mat = commonCovarianceMatrix3D(covariace_f, covariace_m, len_f, len_m);
			MatrixType3D covariace_mat_inv  = (MatrixType3D)covariace_mat.GetInverse();
			return mahalanobis_dist3D(means_f, means_m, covariace_mat_inv);
		}
		
	}

	bool Is2DImage(IteratorType it){
		double sum_z=0;
		it.GoToBegin();
		while (!it.IsAtEnd()){
			double val = it.Get();
			if(val>thd){
				ImageType::IndexType index = it.GetIndex();
				sum_z += index[2];

			}
			++it;
		}
		return sum_z==0;
	}
	
	long int getImageSize(IteratorType it){
		int len=0;
		it.GoToBegin();
		while (!it.IsAtEnd()){
			double val = it.Get();
			if(val>thd){
				len++;
			}
			++it;
		}
		return len;
	}


	//--------------------------- 2D ----------------------------

	VectorType2D calcMean2D(IteratorType it){
		VectorType2D mat;
		mat.Fill(0); 
		double count=0;
		it.GoToBegin();
		while (!it.IsAtEnd()){
			double val = it.Get();
			if(val>thd){
				ImageType::IndexType index = it.GetIndex();
				mat[0] += index[0];
				mat[1] += index[1];
				count++;
			}
			++it;
		}
		mat = mat/count;
		return mat;
	}

	MatrixType2D calcCovariance2D(IteratorType it, VectorType2D means){
		MatrixType2D covariance;
		covariance.Fill(0);
		double sampleCount=0;
		it.GoToBegin();
		while (!it.IsAtEnd()){
			double val = it.Get();
			if(val>thd){
				ImageType::IndexType index = it.GetIndex();
				VectorType2D delta;
				delta[0] = index[0] -  means[0];
				delta[1] = index[1] -  means[1];
				MatrixType2D dot;
				for(int i=0 ; i < 2; i++){
					for(int j=0 ;j<2 ; j++){
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


	MatrixType2D commonCovarianceMatrix2D(MatrixType2D covmat1, MatrixType2D  covmat2, long int len1, long int len2){
		MatrixType2D  covmat;
		covmat = (covmat1*len1+ covmat2*len2)/(len1+len2) ;
		return covmat;
	}


	double mahalanobis_dist2D(VectorType2D means1, VectorType2D means2, MatrixType2D covmat_inv){
		double val=0;
		VectorType2D temp;
		temp[0]= 0;
		temp[1]= 0;
		VectorType2D delta = means1-means2;
		for(int i=0 ; i < 2; i++){
			for(int j=0 ;j<2 ; j++){
				temp[i] += delta[j] * covmat_inv(i,j);
			}
		}
		val = temp * delta;
		return std::sqrt(val);

	}

	///--------------------- 3D -------------------------

	VectorType3D calcMean3D(IteratorType it){
		VectorType3D mat;
		mat.Fill(0); 
		double count=0;
		it.GoToBegin();
		while (!it.IsAtEnd()){
			double val =it.Get();
			if(val>thd){
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

	MatrixType3D calcCovariance3D(IteratorType it, VectorType3D means){
		MatrixType3D covariance;
		covariance.Fill(0);

		double sampleCount=0;
		it.GoToBegin();
		while (!it.IsAtEnd()){
			double val = it.Get();
			if(val>thd){
				ImageType::IndexType index = it.GetIndex();
				VectorType3D delta;
				delta[0] = index[0] -  means[0];
				delta[1] = index[1] -  means[1];
				delta[2] = index[2] -  means[2];
				MatrixType3D dot;
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


	MatrixType3D commonCovarianceMatrix3D(MatrixType3D covmat1, MatrixType3D  covmat2, long int len1, long int len2){
		MatrixType3D  covmat;
		covmat = (covmat1*len1+ covmat2*len2)/(len1+len2) ;
		return covmat;
	}


	double mahalanobis_dist3D(VectorType3D means1, VectorType3D means2, MatrixType3D covmat_inv){
		double val=0;
		VectorType3D temp;
		temp[0]= 0;
		temp[1]= 0;
		temp[2]= 0;
		VectorType3D delta = means1-means2;
		for(int i=0 ; i < 3; i++){
			for(int j=0 ;j<3 ; j++){
				temp[i] += delta[j] * covmat_inv(i,j);
			}
		}
		val = temp * delta;
		return std::sqrt(val);

	}

};

