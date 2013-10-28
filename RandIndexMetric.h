/*
// RandIndexMetric.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 14.05.2013 
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
// This algorithm calculates the Rand index and the adjusted rand index.
// The algorithm builds on the class contingencyTable to access the information of the volumes
// and calculate the rand index.
//  
//
*/

#include "itkImage.h"

class RandIndexMetric
{

private:
  ContingencyTable *contingencyTable;
  bool fuzzy;
  double threshold;
	double a;
	double b;
	double c;
	double d;

public: 
   	
	~RandIndexMetric(){

	}

	RandIndexMetric(ContingencyTable *contingencyTable, bool fuzzy, double threshold){
		this->contingencyTable= contingencyTable;
		this->fuzzy = fuzzy;
        this->threshold = threshold;

		this->a = contingencyTable->a;
		this->b = contingencyTable->b;
		this->c = contingencyTable->c;
		this->d = contingencyTable->d;

	}


	double CalcRandIndex(){
		
    
	  #ifdef _DEBUG
		double X= contingencyTable->tp;
		double Y= contingencyTable->tn;
		double K= contingencyTable->fn;
		double L= contingencyTable->fp;
		double n = contingencyTable->n;
		double ri = (
			    (Y*(Y-1) + K*(K-1) + L*(L-1) + X*(X-1))/2
                 + ( n*n + X*X + Y*Y + L*L + K*K 
				     - ( (Y + K)* (Y + K) + (L + X)*(L + X)) 
					 - ((Y + L)*(Y + L) + (K + X)*(K + X)) 
					 )/2
				 )/(n*(n-1)/2);

        std::cout << "RI controll: "<< ri << std::endl;
        #endif 

		return (a + d)/(a + b + c + d);

	}

	double CalcAdjustedRandIndex(){

		double x1 = a - ((a+c)*(a+b)/(a+b+c+d));
		double x2 = ( (a+c) + (a+b))/2.0;
		double x3 = ( (a+c)*(a+b))/(a+b+c+d);

		if(x2!=x3)
			return x1/(x2-x3);
		else
			return 0;
	}

};

