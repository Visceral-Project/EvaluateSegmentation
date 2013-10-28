/*
// InterclassCorrelationMetric.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 07.05.2013 
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
// This algorithm calculates the interclass correlation between two volumes.
// The algorithm uses the overlap values from the class contingencyTable.
//  
//
*/

#include "itkImage.h"

class InterclassCorrelationMetric
{
	typedef itk::Image<double, 3> ImageType;	
	typedef itk::ImageRegionConstIterator<ImageType> FixedIteratorType;
	typedef itk::ImageRegionConstIterator<ImageType> MovingIteratorType;

private:
  VoxelPreprocessor *voxelprocesser;
  bool fuzzy;
  double threshold;
public: 
   	
	~InterclassCorrelationMetric(){

	}

	InterclassCorrelationMetric(VoxelPreprocessor *voxelprocesser, bool fuzzy, double threshold){
		this->voxelprocesser= voxelprocesser;
		this->fuzzy = fuzzy;
        this->threshold = threshold;
	}

	double CalcInterClassCorrelationCoeff(){
		double *values_f = voxelprocesser->values_f;
		double *values_m = voxelprocesser->values_m;
		double mean_f = voxelprocesser->mean_f;
		double mean_m = voxelprocesser->mean_m;
		int numberElements = std::min(voxelprocesser->numberElements_f, voxelprocesser->numberElements_m);

		double ssw = 0;
		double ssb = 0;
		double grandmean = (mean_f + mean_m)/2;
		for (int i = 0; i < numberElements; i++)
		{
			double val_f = values_f[i];
			double val_m = values_m[i];
			double m = (val_f + val_m)/2;
			ssw += pow(val_f - m, 2);
			ssw += pow(val_m - m, 2);
			ssb += pow(m - grandmean, 2);
		}
		ssw = ssw/numberElements;
		ssb = ssb/(numberElements-1) * 2;
		double icc = (ssb - ssw)/(ssb + ssw);
		return icc;
	  }

};

