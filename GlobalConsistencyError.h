/*
// GlobalConsistencyError.h
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
// This class calculates the global consistency error between two volumes.
// The algorithm uses the overlap values from the class contingencyTable.
//  
//
*/

#include "itkImage.h"


class GlobalConsistencyError
{

private:
  ContingencyTable *contingencyTable;
  bool fuzzy;
  double threshold;
public: 
   	
	~GlobalConsistencyError(){

	}

	GlobalConsistencyError(ContingencyTable *contingencyTable, bool fuzzy, double threshold){
		this->contingencyTable= contingencyTable;
		this->fuzzy = fuzzy;
        this->threshold = threshold;
	}

	double CalcGlobalConsistencyError(){
	  double n00 = contingencyTable->tn;
      double n10 = contingencyTable->fp;
      double n01 = contingencyTable->fn; 
      double n11 = contingencyTable->tp;
	  double num1 =  contingencyTable->numberElements_f;
	  double num2 =  contingencyTable->numberElements_m;

	  double n = std::min(num1, num2);
	  double e1 = 1 - (((n00*n00+n01*n01)/(n00+n01)) + ((n10*n10 + n11*n11)/(n10+n11)))/n;
	  double e2 = 1 - (((n00*n00+n10*n10)/(n00+n10)) + ((n01*n01 + n11*n11)/(n01+n11)))/n;
	  return std::min(e1, e2);
	}


};

