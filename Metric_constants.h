/*
// Metric_constants.h
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
// This header file contains the metric identifiers globaly used in the program to call, display and save the metrics   
//
*/


#ifndef _METRIC_CONSTANTS
#define _METRIC_CONSTANTS

#include "Global.h"

typedef struct MetricInfo{
	char* metrId;
	char* metrSymb;
	char* metrInfo;
	char* help;
	bool similarity;
	bool testmetric;
} MetricInfo;

MetricInfo* metricInfo;

enum MetricId  {DICE,    JACRD, AUC, KAPPA, RNDIND, ADJRIND, ICCORR, VOLSMTY, MUTINF, 
                 MAHLNBS, AVGDIST, HDRFDST, VARINFO, GCOERR, PROBDST,       
                 SNSVTY, SPCFTY, PRCISON,  FMEASR, ACURCY, FALLOUT, MEANTOMEAN
				 ,HDRFDSTITK, HDRFDSTNAIVE
				 , LEN

};

void initMetricInfo(){
  metricInfo = new MetricInfo[LEN];
  MetricInfo* info = &metricInfo[DICE];
  info->metrId = "DICE";
  info->metrSymb="DICE";
  info->metrInfo ="Dice Coefficient (F1-Measure)"; //tested for binary with matlab + BRATS impl.
  info->help ="Dice Coefficient (F1-Measure)";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[JACRD];
  info->metrId = "JACRD";
  info->metrSymb="JACRD";
  info->metrInfo ="Jaccard Coefficient"; //tested against BRATS impl + by compairing with DIC (DICE=2*JACCARD/(JACC + 1)
  info->help ="Jaccard Coefficient";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[SNSVTY];
  info->metrId = "SNSVTY";
  info->metrSymb="SNSVTY";
  info->metrInfo ="Sensitivity (Recall, true positive rate)"; //tested against BRATS + by comparing fmeasure with precision and recall
  info->help ="Sensitivity (Recall, true positive rate)";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[SPCFTY];
  info->metrId = "SPCFTY";
  info->metrSymb="SPCFTY";
  info->metrInfo ="Specificity (true negative rate)"; //tested against BRATS implementation
  info->help ="Specificity (true negative rate)";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[GCOERR];
  info->metrId = "GCOERR";
  info->metrSymb="GCOERR";
  info->metrInfo ="Global Consistency Error"; //tested (compare_segmentations.m)
  info->help ="Global Consistency Error";
  info->similarity =false;
  info->testmetric =false;

  info = &metricInfo[VOLSMTY];
  info->metrId = "VOLSMTY";
  info->metrSymb="VOLSMTY";
  info->metrInfo ="Volumetric Similarity Coefficient"; // controlled [{00189}] ----------------
  info->help ="Volumetric Similarity Coefficient";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[KAPPA];
  info->metrId = "KAPPA";
  info->metrSymb="KAPPA";
  info->metrInfo ="Cohen Kappa";  //tested with matlab kappaindex.m
  info->help ="Cohen Kappa";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[AUC];
  info->metrId = "AUC";
  info->metrSymb="AUC";
  info->metrInfo ="Area under ROC Curve"; //calculated
  info->help ="Area under ROC Curve";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[RNDIND];
  info->metrId = "RNDIND";
  info->metrSymb="RNDIND";
  info->metrInfo ="Rand Index";  //tested with compare_segmentations.m, PartAgreeCoef.
  info->help ="Rand Index";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[ADJRIND];
  info->metrId = "ADJRIND";
  info->metrSymb="ADJRIND";
  info->metrInfo ="Adjusted Rand Index";  //tested for binary with matlab (adjrind, PartAgreeCoef.m)
  info->help ="Adjusted Rand Index";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[ICCORR];
  info->metrId = "ICCORR";
  info->metrSymb="ICCORR";
  info->metrInfo ="Interclass Correlation"; // tested mit online tool http://www.statstodo.com/ICC_Pgm.php (testimage7.nii, testimage8.nii)
  info->help ="Interclass Correlation";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[AVGDIST];
  info->metrId = "AVGDIST";
  info->metrSymb="AVGDIST";
  info->metrInfo ="Average Hausdorff Distance"; // tested with naive algorithm mit small image
  info->help ="Average Hausdorff Distance";
  info->similarity =false;
  info->testmetric =false;

  info = &metricInfo[HDRFDST];
  info->metrId = "HDRFDST";
  info->metrSymb="HDRFDST";
  info->metrInfo ="Hausdorff Distance";  //tested against ITK Hausdorff + naive algorithm mit small image
  info->help ="Hausdorff Distance, HDRFDST@0.95@ -> use 0.95 quantile to avoid outlier, default 1 (=exact distance)";
  info->similarity =false;
  info->testmetric =false;

  info = &metricInfo[MUTINF];
  info->metrId = "MUTINF";
  info->metrSymb="MUTINF";
  info->metrInfo ="Mutual Information"; //conflict with c3d, c
                                        //alculated successfully with excel I(X,Y) = H(X) + H(Y) - H(X,Y)
                                        //compared successfully with Variation of information VARINf(X,Y)=H(X)+H(Y)-2*I(X,Y)
  info->help ="Mutual Information";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[VARINFO];
  info->metrId = "VARINFO";
  info->metrSymb="VARINFO";
  info->metrInfo ="Variation of Information";//tested with compare_segmentations.m, PartAgreeCoef.m
  info->help ="Variation of Information";
  info->similarity =false;
  info->testmetric =false;

  info = &metricInfo[PROBDST];
  info->metrId = "PROBDST";
  info->metrSymb="PROBDST";
  info->metrInfo ="Probabilistic Distance";  // ---------------------------
  info->help ="Probabilistic Distance";
  info->similarity =false;
  info->testmetric =false;

  info = &metricInfo[MAHLNBS];
  info->metrId = "MAHLNBS";
  info->metrSymb="MAHLNBS";
  info->metrInfo ="Mahanabolis Distance"; // conflict with manova/matlab
  info->help ="Mahanabolis Distance";
  info->similarity =false;
  info->testmetric =false;

  info = &metricInfo[PRCISON];
  info->metrId = "PRCISON";
  info->metrSymb="PRCISON";
  info->metrInfo ="Precision (Confidence)" ; // tested by comparing to DICE at beta=1,
  info->help ="Precision (Confidence)";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[FMEASR];
  info->metrId = "FMEASR";
  info->metrSymb="FMEASR";
  info->metrInfo ="F-Measure" ; // tested by comparing with DICE (they are eqivalent) 
  info->help ="F-Measure (FMEASR@0.5@ -> beta=0.5, defalut beta=1)";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[ACURCY];
  info->metrId = "ACURCY";
  info->metrSymb="ACURCY";
  info->metrInfo ="Accuracy"; // formula checked -------------------
  info->help ="Accuracy";
  info->similarity =true;
  info->testmetric =false;

  info = &metricInfo[FALLOUT];
  info->metrId = "FALLOUT";
  info->metrSymb="FALLOUT";
  info->metrInfo ="Fallout (false positive rate)"; // calculated (=1-specificity)
  info->help ="Fallout (false positive rate)";
  info->similarity =false;
  info->testmetric =false;

  info = &metricInfo[HDRFDSTITK];
  info->metrId = "HDRFDSTIKT";
  info->metrSymb="HDRFDSTITK";
  info->metrInfo ="HDRFDSTITK";
  info->help ="HDRFDSTITK";
  info->similarity =false;
  info->testmetric =true;

  info = &metricInfo[HDRFDSTNAIVE];
  info->metrId = "HDRFDSTNAIVE";
  info->metrSymb="HDRFDSTNAIVE";
  info->metrInfo ="HDRFDSTNAIVE";
  info->help ="HDRFDSTNAIVE";
  info->similarity =false;
  info->testmetric =true;

  info = &metricInfo[MEANTOMEAN];
  info->metrId = "MEANTOMEAN";
  info->metrSymb="MEANTOMEAN";
  info->metrInfo ="MEANTOMEAN";
  info->help ="MEANTOMEAN";
  info->similarity =false;
  info->testmetric =true;

}

int METRIC_COUNT = LEN;

MetricId VisceralMetrics[] = {DICE, AVGDIST, MUTINF, ADJRIND};

#endif