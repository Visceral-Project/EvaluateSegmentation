/*
// ClassicMeasures.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 09.04.2013 
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
// This file contains methods for calculating the classic simple measurs like precision, recall, accuracy, etc..
// The algorithm uses the overlap values from the class contingencyTable.
//
*/


#include "itkImage.h"
class ClassicMeasures
{

private:
  ContingencyTable *contingencyTable;
  bool fuzzy;
  double threshold;
public: 
   	
	~ClassicMeasures(){

	}

	ClassicMeasures(ContingencyTable *contingencyTable, bool fuzzy, double threshold){
		this->contingencyTable= contingencyTable;
		this->fuzzy = fuzzy;
        this->threshold = threshold;
	}

	double getTP(){
		return contingencyTable->tp;
	}
	
	double getTN(){
		return contingencyTable->tn;
	}
	
	double getFP(){
		return contingencyTable->fp;
	}

	double getFN(){
		return contingencyTable->fn;
	}
	
	
	double CalcSegmentedVolumeInVoxel(){
	     double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		return tp + fp;
	}
	
	double CalcSegmentedVolumeInMl(){
	     double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double spx = contingencyTable->vspx;
		double spy = contingencyTable->vspy;
		double spz = contingencyTable->vspz;
		return (tp + fp) * spx * spy * spz / 1000.0;
	}
	
	double CalcReferenceVolumeInVoxel(){
	     double tp = contingencyTable->tp;
		double fn = contingencyTable->fn;
		return tp + fn;
	}
	
	double CalcReferenceVolumeInMl(){
	     double tp = contingencyTable->tp;
		double fn = contingencyTable->fn;
		double spx = contingencyTable->vspx;
		double spy = contingencyTable->vspy;
		double spz = contingencyTable->vspz;
		return (tp + fn) * spx * spy * spz / 1000.0;
	}	

	
	double CalcSensitivity(){
	     double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double fn = contingencyTable->fn;
		double tn = contingencyTable->tn;
		return tp/(tp + fn);
	}

	double CalcSpecificity(){
	     double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double fn = contingencyTable->fn;
		double tn = contingencyTable->tn;
		return tn/(tn + fp);
	}


	double CalcFallout(){
	     double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double fn = contingencyTable->fn;
		double tn = contingencyTable->tn;
		return 1 - CalcSpecificity(); //false positive rate (fallout = 1 - specificity) 
	}


	double CalcAUC(){
	     double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double fn = contingencyTable->fn;
		double tn = contingencyTable->tn;
		double tpr =CalcSensitivity(); //true positive rate (sensitivity)
        double fpr = CalcFallout(); //false positive rate (fallout)
		return (tpr - fpr + 1)/2;
	}

	double CalcPrecision(){
		double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double fn = contingencyTable->fn;
		double tn = contingencyTable->tn;
		if((tp + fp)!=0){
		   return tp/(tp + fp) ;
		} 
		   return 0;

	}


    double CalcRecall(){
		double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double fn = contingencyTable->fn;
		double tn = contingencyTable->tn;
		if((tp + fn)!=0){
    		return tp/(tp + fn) ;
		}
        return 0;
	}

   	double CalcFMeasure(double beta){
		double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double fn = contingencyTable->fn;
		double tn = contingencyTable->tn;
		double beta_s = beta*beta;
		double precision = CalcPrecision();
		double recall = CalcRecall();
		if((beta_s*precision + recall)!=0 ){
		      return (1 + beta_s ) * ((precision*recall)/(beta_s*precision + recall)) ;
		}
		return 0;

	}

   	double CalcAccuracy(){
		double tp = contingencyTable->tp;
		double fp = contingencyTable->fp;
		double fn = contingencyTable->fn;
		double tn = contingencyTable->tn;
		if((tp + tn + fp + fn)!=0){
		     return (tp + tn)/(tp + tn + fp + fn);
		}
		return 0;

	}



};

