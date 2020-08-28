#ifndef _LESION_DETECTION
#define _LESION_DETECTION

/*
// LesionDetection.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 29.08.2013 
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
// This header file contains all functions needed to evaluate the quailty lesion detection.
// Lesion detections to be evaluated should be in a file and the ground truth lesion detection should be in an other.
// If the evaluation should be restricted to lesions in particular regions, a mask segmentation should be provided,
// which contains region of interest.
//
*/

#include "LesionDetectionConst.h" 
#include "LesionDetectionMask.h" 

using namespace std;

std::vector<GTLesion> truth_lesiondetections;
std::vector<TestLesion> test_lesiondetections;

GTLesion * createGTLesionIfNotExists(int region, int counter, vector<GTLesion> *lesions){
	GTLesion *lesion;
	for(vector<GTLesion>::iterator it = lesions->begin(); it != lesions->end(); it++){
		lesion = &(*it);
		if(lesion->region_id == region && lesion->counter == counter){
			return lesion;
		}

	}
	lesion = new GTLesion();
	lesion->counter=counter;
	lesion->region_id=region;
	lesion->m_x=0;
	lesion->m_y=0;
	lesion->m_z=0;
	lesion->d1_x=0;
	lesion->d1_y=0;
	lesion->d1_z=0;
	lesion->d2_x=0;
	lesion->d2_y=0;
	lesion->d2_z=0;
	lesions->push_back((GTLesion)*lesion);
	return createGTLesionIfNotExists(region, counter, lesions);

}

void parseGTLesionDetectionFile(string l_file, vector<GTLesion> *lesions){
	ifstream i_stream;
	std::string line, token;
	i_stream.open(l_file.c_str()); 

	if (i_stream.is_open()) {
		while (i_stream.good()) {
			getline(i_stream,line);

			trim(line);
			if(line==""){
				continue;
			}

			if(line.find("#")==0 ){
				continue;
			}


			std::istringstream inputss (line);

			int pos=0;


			string label;
			int region=0;
			int counter=0;
			int type=-1;
			double x =0;
			double y=0;
			double z=0;

			while (getline(inputss, token, ',') )
			{
				trim(token);
				if(pos==0){
					string t;
					std::istringstream labelstream (token);
					int i=0;
					while (getline(labelstream, t, '_') ){
						if(i==0){
							if(t=="LU"){
								region = REGION_ID_LUNG;
							}
							else if(t=="BO"){
								region = REGION_ID_BONE;
							} 
							else if(t=="LI"){
								region = REGION_ID_LEVER;
							}
							else if(t=="BR"){
								region = REGION_ID_BRAIN;
							}
							else if(t=="LN"){
								region = REGION_ID_LYMPHNODE;
							}
							label=t + "_";
						}
						else if(i==1){
							counter = atof(t.c_str());
							label += t;

						}
						else if(i==2){
							if(t=="C" || t=="c"){
								type = POINTTYPE_CENTER;
							}
							else  if(t=="D1" || t=="d1"){
								type = POINTTYPE_D1;
							}
							else  if(t=="D2" || t=="d2"){
								type = POINTTYPE_D2;
							}

						}
						++i;
					}

				}
				else if(pos==1){
					x= atof(token.c_str());
				}
				else  if(pos==2){
					y= atof(token.c_str());
				}
				else  if(pos==3){
					z= atof(token.c_str());
				}
				++pos;
			}

			if(pos< 3){
				continue;
			}

			GTLesion *lesion = createGTLesionIfNotExists(region, counter, lesions);
			lesion->label = label;
			if(type == POINTTYPE_CENTER){
				lesion->m_x = x;
				lesion->m_y = y;
				lesion->m_z = z;
			}
			else if(type == POINTTYPE_D1){
				lesion->d1_x = x;
				lesion->d1_y = y;
				lesion->d1_z = z;
			}
			else if(type == POINTTYPE_D2){
				lesion->d2_x = x;
				lesion->d2_y = y;
				lesion->d2_z = z;
			}
			type=-1;

		}

	}
	count_groundtruth_lesions_total = lesions->size();

}


void parseTestLesionDetectionFile(string l_file, vector<TestLesion> *lesions){
	ifstream i_stream;
	std::string line, token;
	i_stream.open(l_file.c_str()); 
	int numOfLine=0;

	if (i_stream.is_open()) {
		while (i_stream.good()) {
			getline(i_stream,line);

			trim(line);
			if(line==""){
				continue;
			}

			if(line.find("#")==0 ){
				continue;
			}
			std::istringstream inputss (line);
			int pos=0;
			string label;
			int type=-1;
			double x =0;
			double y=0;
			double z=0;

			while (getline(inputss, token, ',') )
			{
				trim(token);
				if(pos==0){
					label = token;
				}
				else if(pos==1){
					x= atof(token.c_str());
				}
				else  if(pos==2){
					y= atof(token.c_str());
				}
				else  if(pos==3){
					z= atof(token.c_str());
				}
				++pos;
			}
			if(pos< 3){
				continue;
			}
			TestLesion *lesion = new TestLesion();
			lesion->label = label;
			lesion->x = x;
			lesion->y = y;
			lesion->z = z;
			test_lesiondetections.push_back((TestLesion)*lesion);

		}

	}
	count_test_lesions_total = lesions->size();

}


TestLesion* findNearestTestLesion(GTLesion *truth, vector<TestLesion> *lesions){

	double nearest_dist = 99999;
	TestLesion * nearest=NULL;
	for(vector<TestLesion>::iterator it = lesions->begin(); it != lesions->end(); it++){
		TestLesion *test = &(*it);
		double dist =  sqrt((double)((truth->m_x-test->x)*(truth->m_x-test->x) + (truth->m_y-test->y)*(truth->m_y-test->y) + (truth->m_z-test->z)*(truth->m_z-test->z)));
		if(dist < nearest_dist){
			nearest = test;
			nearest_dist = dist;
		}
	}
	return  nearest;
}

itk::DOMNode::Pointer  OpenLesionDetectionResultXML(const char* targtfile, const char* truthLocFile, const char* testLocFile){
	const char* xMLFileName = targtfile;
	itk::DOMNode::Pointer dOMObject;
	if(itksys::SystemTools::FileExists(xMLFileName,true)){
		itksys::SystemTools::RemoveFile(xMLFileName);
	}

	dOMObject = itk::DOMNode::New();
	char *nodename= "measurement";
	dOMObject->SetName( nodename);

	itk::DOMNode::Pointer n = itk::DOMNode::New();
	itk::DOMNode* node = (itk::DOMNode*)n;
	dOMObject->AddChildAtEnd( node );
	node->SetName( "Groundtruth-file"); 
	node->SetAttribute( "filename", truthLocFile );

	n = itk::DOMNode::New();
	node = node = (itk::DOMNode *)n;
	dOMObject->AddChildAtEnd( node );
	node->SetName( "Test-file"); 
	node->SetAttribute( "filename", testLocFile );

	n = itk::DOMNode::New();
	itk::DOMNode* retrieved_node =(itk::DOMNode*)n;
	dOMObject->AddChildAtEnd( retrieved_node);
	retrieved_node->SetName( "Retrieved-Lesions"); 

	n = itk::DOMNode::New();
	itk::DOMNode* lesion_node =(itk::DOMNode*)n;
	dOMObject->AddChildAtEnd( lesion_node);
	lesion_node->SetName( "Detection"); 

	return dOMObject;

}

void SaveLesionDetectionResultXML(itk::DOMNode::Pointer xmlObject, const char* targtfile){
	if(targtfile != NULL && xmlObject != NULL){
		cout << "\nWriting result in file: " << targtfile << "\n";
		itk::DOMNodeXMLWriter::Pointer writer = itk::DOMNodeXMLWriter::New();
		writer->SetInput(xmlObject );
		writer->SetFileName( targtfile );
		writer->Update();
	}
}

void display(itk::DOMNode::Pointer xmlObject){

	cout << "\nRetrieved Points:\n";
	itk::DOMNode * lesion_node= xmlObject->GetChild("Retrieved-Lesions");
	if(lesion_node != NULL){
		vector<itk::DOMNode *> retrieved_lesions;
		lesion_node->GetChildren("Lesion", retrieved_lesions);
		for( vector<itk::DOMNode *>::iterator it=retrieved_lesions.begin(); it != retrieved_lesions.end(); ++it){
			itk::DOMNode * node = *it;
			cout << "Point: ";
			cout << node->GetAttribute("point");
			if(node->GetAttribute("inside-mask") == "false"){
				cout << " | outside mask";
			}
			cout << std::endl;
		}
	}

	cout << "\nDetection (Brain, Lungs, Liver, Bonse):\n";
	if(xmlObject != NULL){
		itk::DOMNode * lesion_node= xmlObject->GetChild("Detection");
		if(lesion_node != NULL){
			vector<itk::DOMNode *> gt_lesions;
			lesion_node->GetChildren("Groundtruth-Lesion", gt_lesions);
			for( vector<itk::DOMNode *>::iterator it=gt_lesions.begin(); it != gt_lesions.end(); ++it){
				itk::DOMNode * node = *it;
				if( node->GetAttribute("lymphnode") == "true"){
					continue;
				}
				cout << node->GetAttribute("id") << " ";
				cout << node->GetAttribute("groundtruth-pnt") << " | ";
				if(node->GetAttribute("detected") == "true"){
					cout << "detected | ";
				}
				else{
					cout << " NOT detected | ";
				}
				cout << "distance= " << node->GetAttribute("dist-to-nearest-pnt");
				cout << std::endl;
			}
		}
	}

	cout << "\nScores (Brain, Lungs, Liver, Bonse):\n";
	if(xmlObject != NULL){
		itk::DOMNode * lesion_node= xmlObject->GetChild("Metrics");
		if(lesion_node != NULL){
			cout << "Precision= " << lesion_node->GetAttribute("precision") << std::endl;
			cout << "Recall= " << lesion_node->GetAttribute("recall") << std::endl;
		}
	}

	cout << "\nDetection (lymphnodes):\n";
	if(xmlObject != NULL){
		itk::DOMNode * lesion_node= xmlObject->GetChild("Detection");
		if(lesion_node != NULL){
			vector<itk::DOMNode *> gt_lesions;
			lesion_node->GetChildren("Groundtruth-Lesion", gt_lesions);
			for( vector<itk::DOMNode *>::iterator it=gt_lesions.begin(); it != gt_lesions.end(); ++it){
				itk::DOMNode * node = *it;
				if( node->GetAttribute("lymphnode") != "true"){
					continue;
				}
				cout << node->GetAttribute("id") << " ";
				cout << node->GetAttribute("groundtruth-pnt") << " | ";
				if(node->GetAttribute("detected") == "true"){
					cout << "detected | ";
				}
				else{
					cout << " NOT detected | ";
				}
				cout << "distance= " << node->GetAttribute("dist-to-nearest-pnt");
				cout << std::endl;
			}
		}
	}

	cout << "\nScore (lymphnode):\n";
	if(xmlObject != NULL){
		itk::DOMNode * lesion_node= xmlObject->GetChild("Metrics");
		if(lesion_node != NULL){
			cout << "Detection rate= " << lesion_node->GetAttribute("lymphnode-detection-rate") << std::endl;
		}
	}





}



bool compareLesions(GTLesion* truth, TestLesion* test, itk::DOMNode::Pointer xmlObject,  LesionDetectionMask *organmask){
	bool detected = false;

	if(test!=NULL){
		double distance =  sqrt((double)((truth->m_x-test->x)*(truth->m_x-test->x) + (truth->m_y-test->y)*(truth->m_y-test->y) + (truth->m_z-test->z)*(truth->m_z-test->z)));
		char val [50];
		sprintf(val, "%.6f", distance);
		char s_truth[100];
		sprintf(s_truth, "(%.3f,%.3f,%.3f)", truth->m_x, truth->m_y, truth->m_z);
		char s_test[100];
		sprintf(s_test, "(%.3f,%.3f,%.3f)", test->x, test->y, test->z);

		double diameter = sqrt((double)((truth->d1_x-truth->d2_x)*(truth->d1_x-truth->d2_x) + (truth->d1_y-truth->d2_y)*(truth->d1_y-truth->d2_y) + (truth->d1_z-truth->d2_z)*(truth->d1_z-truth->d2_z)));
		double radius = diameter/2;
		char s_diam [50];
		sprintf(s_diam, "%.6f", diameter);

		if(xmlObject != NULL){
			itk::DOMNode * lesion_node= xmlObject->GetChild("Detection");
			if(lesion_node != NULL){
				itk::DOMNode::Pointer n = itk::DOMNode::New();
				itk::DOMNode* node =(itk::DOMNode*)n;
				node->SetName( "Groundtruth-Lesion"); 
				node->SetAttribute( "id", truth->label );
				node->SetAttribute( "dist-to-nearest-pnt", val );
				node->SetAttribute( "metric", "Euclidean" );
				node->SetAttribute( "groundtruth-pnt", s_truth );
				node->SetAttribute( "lesion-diamater", s_diam );
				node->SetAttribute( "point", s_test );
				if(truth->region_id == REGION_ID_LYMPHNODE){
					node->SetAttribute( "lymphnode", "true" );
					count_lymphnode_lesions++;
				}
				else{
					node->SetAttribute( "lymphnode", "false" );

				}

				if(distance <= radius){
					node->SetAttribute( "detected", "true" );
					detected = true;
				}
				else{
					node->SetAttribute( "detected", "false" );
					detected = false;
				} 
				lesion_node->AddChildAtEnd( node );

			}
		}
	}
	else{
		char s_truth[100];
		sprintf(s_truth, "(%.0f,%.0f,%.0f)", truth->m_x, truth->m_y, truth->m_z);
		if(xmlObject != NULL){
			itk::DOMNode * lesion_node= xmlObject->GetChild("Detection");
			if(lesion_node != NULL){
				itk::DOMNode::Pointer n = itk::DOMNode::New();
				itk::DOMNode* node =(itk::DOMNode*)n;
				node->SetName( "Groundtruth-Lesion"); 
				node->SetAttribute( "id", truth->label );
				node->SetAttribute( "groundtruth-point", s_truth );
				node->SetAttribute( "detected", "false" );
				if(truth->region_id == REGION_ID_LYMPHNODE){
					node->SetAttribute( "lymphnode", "true" );
					count_lymphnode_lesions++;
				}
				else{
					node->SetAttribute( "lymphnode", "false" );
				}
				detected = false;
				lesion_node->AddChildAtEnd( node );
				std::cout << truth->label << " (" << s_truth << ") \t | POINT NOT FOUND (this case should niot occurr)"; 
				cout << std::endl;
			}
		}

	}
	return detected;

}



int validateLesionDetection(const char* f1, const char* f2, const char* targetFile, const char* maskFile, char *options)
{
	LesionDetectionMask * organmask=NULL;
	string l_truth_file = "";
	string l_test_file = "";
	string mask_file = "";
	bool truth_url=isUrl(f1);
	if(truth_url){
		l_truth_file = download_image(f1, "__temp_lesiondetection_file_truth.fcsv"); 
		parseGTLesionDetectionFile(l_truth_file, &truth_lesiondetections);
		remove(l_truth_file.c_str()) ;
	} 
	else{
		if(!itksys::SystemTools::FileExists(f1,true)){
			return 0;
		}
		parseGTLesionDetectionFile(f1, &truth_lesiondetections);
	}

	bool test_url=isUrl(f2);
	if(test_url){
		l_test_file = download_image(f2, "__temp_lesiondetection_file_test.nii");
		parseTestLesionDetectionFile(l_test_file, &test_lesiondetections);
		remove(l_test_file.c_str());
	}
	else{
		if(!itksys::SystemTools::FileExists(f2,true)){
			return 0;
		}
		parseTestLesionDetectionFile(f2, &test_lesiondetections);
	}

	if(maskFile!=NULL && maskFile!=""){
		bool mask_url=isUrl(maskFile);
		if(mask_url){
			mask_file = download_image(maskFile, "__temp_lesiondetection_file_test.nii");
			organmask = new LesionDetectionMask(mask_file.c_str());
			remove(l_test_file.c_str());
		}
		else{
			if(!itksys::SystemTools::FileExists(maskFile,true)){
				return 0;
			}
			organmask = new LesionDetectionMask(maskFile);
		}
	}

	if(targetFile == NULL){
		char temp[500];
		strcpy(temp,f2);
		strcat(temp, ".result.xml");
		targetFile = temp;
	}
	itk::DOMNode::Pointer xmlObject = OpenLesionDetectionResultXML(targetFile, f1, f2 );


	itk::DOMNode * retrieved_node= NULL;
	if(xmlObject!= NULL){
		retrieved_node = xmlObject->GetChild("Retrieved-Lesions");
	}

	if(retrieved_node != NULL){
		for(vector<TestLesion>::iterator it = test_lesiondetections.begin(); it != test_lesiondetections.end(); ++it){
			TestLesion *lesion;
			lesion = &(*it);
			bool isInRegion = true;
			if(organmask!=NULL){
				if(!organmask->isTestLesionInsideRegion(lesion) ){
					count_test_lesions_excluded++; 
					isInRegion = false;
				}
			}

			char s_test[100];
			sprintf(s_test, "(%.3f,%.3f,%.3f)", lesion->x, lesion->y, lesion->z);
			if(xmlObject != NULL){
				itk::DOMNode::Pointer n = itk::DOMNode::New();
				itk::DOMNode* node =(itk::DOMNode*)n;
				node->SetName( "Lesion"); 
				node->SetAttribute( "point", s_test );

				if(isInRegion){
					node->SetAttribute( "inside-mask", "true" );
				}
				else{
					node->SetAttribute( "inside-mask", "false" );
				} 

				retrieved_node->AddChildAtEnd( node );
			}
		}
	}


	for(vector<GTLesion>::iterator it = truth_lesiondetections.begin(); it != truth_lesiondetections.end(); ++it){
		GTLesion *truth_lesion = &(*it);
		TestLesion *test_lesion = findNearestTestLesion(truth_lesion, &test_lesiondetections);
		if(truth_lesion != NULL){
			bool detected = compareLesions(truth_lesion, test_lesion, xmlObject, organmask);
			if(detected && truth_lesion->region_id != REGION_ID_LYMPHNODE){
				count_lesion_detected++;
			}
			else if(detected && truth_lesion->region_id == REGION_ID_LYMPHNODE){
				count_lymphnode_detected++;
			}
		}
	}


	if(xmlObject != NULL){
		itk::DOMNode::Pointer 	n = itk::DOMNode::New();
		itk::DOMNode* node = (itk::DOMNode*)n;
		xmlObject->AddChildAtEnd( node );
		node->SetName( "Statistics"); 
		char val [50];
		sprintf(val, "%i",count_groundtruth_lesions_total);
		node->SetAttribute( "groundtruth_lesion-total", val );

		sprintf(val, "%i", count_lymphnode_lesions);
		node->SetAttribute( "groundtruth-lemphnodes", val );

		sprintf(val, "%i", count_test_lesions_total);
		node->SetAttribute( "retieved-lesions-total", val );

		sprintf(val, "%i", count_test_lesions_excluded);
		node->SetAttribute( "retieved-lesions-excluded", val );

		sprintf(val, "%i", count_lesion_detected);
		node->SetAttribute( "lesions-detected", val );

		sprintf(val, "%i", count_lymphnode_detected);
		node->SetAttribute( "lymphnodes-detected", val );

		double total =  count_groundtruth_lesions_total - count_lymphnode_lesions;
		double TP = count_lesion_detected;
		double FP = count_test_lesions_total - count_test_lesions_excluded - count_lesion_detected;
		double FN = total - count_lesion_detected;

		n = itk::DOMNode::New();
		node = (itk::DOMNode*)n;
		xmlObject->AddChildAtEnd( node );
		node->SetName( "Metrics"); 

		sprintf(val, "%.6f", TP);
		node->SetAttribute( "true-positive", val );

		sprintf(val, "%.6f", FP);
		node->SetAttribute( "false-positive", val );

		sprintf(val, "%.6f", FN);
		node->SetAttribute( "false-negative", val );

		sprintf(val, "%.6f", TP/(TP+FP));
		node->SetAttribute( "precision", val );

		sprintf(val, "%.6f", TP/(TP+FN));
		node->SetAttribute( "recall", val );

		if(count_lymphnode_lesions!=0){
			sprintf(val, "%.6f", (double)count_lymphnode_detected/(double)count_lymphnode_lesions);
			node->SetAttribute( "lymphnode-detection-rate", val );
		}

	}
	display(xmlObject);
	SaveLesionDetectionResultXML(xmlObject, targetFile);
	std::cout << "\n  ---** VISCERAL 2013, www.visceral.eu **---\n" << std::endl;	
	return 0;
}

#endif

