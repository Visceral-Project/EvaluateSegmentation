/*
// MutualInformationMetric.h
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
// This algorithm calculates the Mutual Information between two volumes.
// The algorithm builds on the class contingencyTable to access the information of the volumes.
//  
//
*/

#include "itkImage.h"

class MutualInformationMetric
{

private:
  ContingencyTable *contingencyTable;
  bool fuzzy;
  double threshold;
public: 
   	
	~MutualInformationMetric(){

	}

	MutualInformationMetric(ContingencyTable *contingencyTable, bool fuzzy, double threshold){
		this->contingencyTable= contingencyTable;
		this->fuzzy = fuzzy;
        this->threshold = threshold;
	}



    double CalcMutualInformation(){
		double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double fn = contingencyTable->fn;
		double tn = contingencyTable->tn;
	    double num1 =  contingencyTable->numberElements_f;
	    double num2 =  contingencyTable->numberElements_m;

		double n = std::min(num1, num2);


		//std::cout << "MI controll: "<< -( (tp+fp)/n* log2( (tp+fp)/n)  + (tn+fn)/n* log2( (tn+fn)/n)) << std::endl;
		//std::cout << "tp: "<< tp << " fp:" << fp << " fn:" <<  fn << " tn:" << tn << std::endl;


		double row1 = tn + fn;
		double row2 = fp + tp;
		double H1 = - ( (row1/n)*log2(row1/n) + (row2/n)*log2(row2/n));

		double col1 = tn + fp;
		double col2 = fn + tp;
		double H2 = - ( (col1/n)*log2(col1/n) + (col2/n)*log2(col2/n));

		double p00 = tn==0?1:(tn/n);
		double p01 = fn==0?1:(fn/n);
		double p10 = fp==0?1:(fp/n);
		double p11 = tp==0?1:(tp/n);
		double H12= - ( (tn/n)*log2(p00) + (fn/n)*log2(p01) +  (fp/n)*log2(p10) + (tp/n)*log2(p11) );
		double MI=H1+H2-H12;
		return MI;

        #ifdef _DEBUG
          MI=0;
		 double x = tn;
		 double s1 = tn+fp;
		 double s2 = tn + fn;
		 MI += x * log2((x*n)/(s1*s2));
		 x = fp;
		 s1 = tn+fp;
		 s2 = fp + tp;
		 MI += x * log2((x*n)/(s1*s2));
		 x = fn;
		 s1 = fn + tp;
		 s2 = tn + fn;
		 MI += x * log2((x*n)/(s1*s2));
		 x = tp;
		 s1 = tp+fn;
		 s2 = fp+tp;
		 MI += x * log2((x*n)/(s1*s2));
		 MI /=n;
		 std::cout << "MI controll: "<< MI << std::endl;
        #endif 
	
	}



	static long double log2(long double val){
		return log(val)/log( (long double)2.0);
	}

};

