/*
// ContingencyTable.h
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
// This class builds a contingency table holding statistic and information about the 
// compared images like overlapping voxels and the parameter a,b,c,d, etc. which are 
// used by evaluation algorithm.
//
*/

#ifndef _CONTINGENCYTABLE
#define _CONTINGENCYTABLE

#include "itkImage.h"
#include "itkImageRegionIterator.h"

class ContingencyTable
{


private:
	VoxelPreprocessor *voxelprocesser;

public: 
	int numberElements_f;
    int numberElements_m;
	double tn; //TN
	double fn; //FN
	double fp; //FP
	double tp; //TP
	double a; 
	double b;
	double c;
	double d;
	double n;
	
     double vspx; // Voxelspacing x
    double vspy; // Voxelspacing y
    double vspz; // Voxelspacing z
	typedef itk::ImageRegionConstIterator<ImageType> FixedIteratorType;
	typedef itk::ImageRegionConstIterator<ImageType> MovingIteratorType;
	~ContingencyTable(){

	}

	ContingencyTable(VoxelPreprocessor *voxelprocesser, bool fuzzy, double threshold){
		this->voxelprocesser= voxelprocesser;
		double mean_f= voxelprocesser->mean_f;
		double mean_m= voxelprocesser->mean_m;
		numberElements_f = voxelprocesser->numberElements_f;
		numberElements_m = voxelprocesser->numberElements_m;

		this->vspx = voxelprocesser->vspx;
		this->vspy = voxelprocesser->vspy;
		this->vspz = voxelprocesser->vspz;
		
		double sum_f = 0;
		double sum_m = 0;
		tn = 0;
		fp = 0;
		fn = 0;
		tp = 0;
		a = 0;
		b = 0;
		c = 0;
		d = 0;
		for (int i = 0; i < numberElements_f && i < numberElements_m; i++)
		{
				double x1 =((double)values_f[i])/((double)PIXEL_VALUE_RANGE_MAX);
				double y1 =1-x1;
				double x2 = ((double)values_m[i])/((double)PIXEL_VALUE_RANGE_MAX);
				double y2 =1-x2;
				tn += std::min(y1,y2);
				fn += x1>x2?x1-x2:0;
				fp += x2>x1?x2-x1:0;
				tp += std::min(x1,x2);
				
		}
		//-------------------
		n = std::min(numberElements_f, numberElements_m);
		double coltot1 = tn + fp;
		double coltot2 = fn + tp;
		double rowtot1 = tn + fn;
		double rowtot2 = fp + tp;
		double nis = rowtot1*rowtot1 + rowtot2*rowtot2;  
		double njs = coltot1*coltot1 + coltot2*coltot2;  
		double s = tp*tp + tn*tn + fp*fp + fn*fn ;
		a = ( binomial(tn) + binomial(fn) + binomial(fp) + binomial(tp) )/2.0;
		b = (njs - s)/2.0;
		c = (nis - s)/2.0;
		d = ( n*n + s - nis - njs )/2.0; 
		double t1=  (n * (n-1))/2.0 ;
	}

	static long double binomial(double val){
		return  val*(val-1);
	}

};

#endif