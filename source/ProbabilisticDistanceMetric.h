/*
// ProbabilisticDistanceMetric.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 22.04.2013 
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
// This algorithm calculates the Probabilistic Distance between two volumes.
//
//  
//
*/

#include "itkImage.h"

class ProbabilisticDistanceMetric
{
	typedef itk::ImageRegionConstIterator<ImageType> FixedIteratorType;
	typedef itk::ImageRegionConstIterator<ImageType> MovingIteratorType;

private:
	VoxelPreprocessor *voxelprocesser;
	bool fuzzy;
	double threshold;
public: 

	~ProbabilisticDistanceMetric(){

	}

	ProbabilisticDistanceMetric(VoxelPreprocessor *voxelprocesser, bool fuzzy, double threshold){
		this->voxelprocesser= voxelprocesser;
		this->fuzzy = fuzzy;
		this->threshold = threshold;
	}

	double CalcJProbabilisticDistance(){ //[{00121}]
		double mean_f = voxelprocesser->mean_f;
		double mean_m = voxelprocesser->mean_m;
		int numberElements = std::min(voxelprocesser->numberElements_f, voxelprocesser->numberElements_m);

		double probability_joint = 0;
		double probability_diff = 0;  

		for (int i = 0; i < numberElements; i++)
		{
			double f =values_f[i];
			double m =values_m[i];
			probability_diff += abs(f - m);
			probability_joint += f * m;
		}

		double pd= -1;
		if(probability_joint != 0)
			pd = probability_diff/(2*probability_joint);
		return pd;

	}


};

