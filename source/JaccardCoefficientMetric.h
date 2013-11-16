/*
// DiceCoefficientMetric.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 02.05.2013 
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
// This algorithm calculates the JACCARD coefficient between two volumes.
// The algorithm uses the overlap values from the class contingencyTable.
//  
//
*/

#include "itkImage.h"

class JaccardCoefficientMetric
{

	typedef itk::ImageRegionConstIterator<ImageType> FixedIteratorType;
	typedef itk::ImageRegionConstIterator<ImageType> MovingIteratorType;

private:
	ContingencyTable *contingencyTable;
	bool fuzzy;
	double threshold;
public: 

	~JaccardCoefficientMetric(){

	}

	JaccardCoefficientMetric(ContingencyTable *contingencyTable, bool fuzzy, double threshold){
		this->contingencyTable= contingencyTable;
		this->fuzzy = fuzzy;
		this->threshold = threshold;
	}

	double CalcJaccardCoeff(){
		double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double fn = contingencyTable->fn;
		double tn = contingencyTable->tn;

		#ifdef _DEBUG
			cout << "JACC: " << ( contingencyTable->a/( contingencyTable->a + contingencyTable->b +  contingencyTable->c)) << endl;
        #endif

		return tp/(tp + fp + fn);

	}

};

