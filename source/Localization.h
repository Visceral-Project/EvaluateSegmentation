/*
// Localization.h
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
// This header file contains all functions needed to evaluate the quailty landmark localizations.
// Landmarks to be evaluated should be in a file and the ground truth landmarks should be in an other.
// Landmarks in both files are combined with the landmark id. The function in this header file parse
// the landmarks and their coordinates and compare them with the groundtuth landmarks.
//
*/


using namespace std;

typedef struct Landmark{
	string Id;
	double x;
	double y;
	double z;
} Landmark;

void parseLocalizationFile(string l_file, vector<Landmark> *landmarks){
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
		  Landmark *landmark = new Landmark();
		 
          while (getline(inputss, token, ',') )
          {
			   trim(token);

			  if(pos==0){
				  landmark->Id=token;
			  }
			  else if(pos==1){
				  landmark->x= atof(token.c_str());
			  }
			  else  if(pos==2){
				  landmark->y= atof(token.c_str());
			  }
			  else  if(pos==3){
				  landmark->z= atof(token.c_str());
			  }

             pos++;
          }
		   landmarks->push_back((Landmark)*landmark);

     }

  }

}


Landmark* findMatchingLandmark(Landmark *testLandmark, vector<Landmark> landmarks){
	for(vector<Landmark>::iterator it = landmarks.begin(); it != landmarks.end(); it++){
		 Landmark *landmark = &(*it);
		 if(testLandmark->Id == landmark->Id){
			 Landmark *l = new Landmark();
			 l->Id=landmark->Id;
			 l->x=landmark->x;
			 l->y=landmark->y;
			 l->z=landmark->z;
		    return l;
		 }
		 
	}
	return NULL;
}

itk::DOMNode::Pointer  OpenLocalizationResultXML(const char* targtfile, const char* truthLocFile, const char* testLocFile){
	if(targtfile != NULL){

		itk::DOMNode::Pointer dOMObject;
		const char* xMLFileName = targtfile;
		if(itksys::SystemTools::FileExists(xMLFileName,true)){
			itksys::SystemTools::RemoveFile(xMLFileName);
		}
		dOMObject = itk::DOMNode::New();
		char *nodename= "measurement";
		dOMObject->SetName( nodename);
				
		itk::DOMNode::Pointer n = itk::DOMNode::New();
		itk::DOMNode* node = (itk::DOMNode*)n;
		dOMObject->AddChildAtEnd( node );
		node->SetName( "ground-truth"); 
 	    node->SetAttribute( "filename", truthLocFile );

		n = itk::DOMNode::New();
		node = node = (itk::DOMNode *)n;
		dOMObject->AddChildAtEnd( node );
		node->SetName( "test-landmarks"); 
 	    node->SetAttribute( "filename", testLocFile );
			
		n = itk::DOMNode::New();
		itk::DOMNode* landmarksnode =(itk::DOMNode*)n;
		dOMObject->AddChildAtEnd( landmarksnode);
		landmarksnode->SetName( "Landmarks"); 
		return dOMObject;
	}
	
}
void SaveLocalizationResultXML(itk::DOMNode::Pointer xmlObject, const char* targtfile){
	if(targtfile != NULL && xmlObject != NULL){
	    itk::DOMNodeXMLWriter::Pointer writer = itk::DOMNodeXMLWriter::New();
		writer->SetInput(xmlObject );
		writer->SetFileName( targtfile );
		writer->Update();
	}

}


void compareLandmarks(Landmark* truth, Landmark* test, itk::DOMNode::Pointer xmlObject){

	if(truth!=NULL){
		double value =  sqrt((double)((truth->x-test->x)*(truth->x-test->x) + (truth->y-test->y)*(truth->y-test->y) + (truth->z-test->z)*(truth->z-test->z)));
		char val [50];
		sprintf(val, "%.6f", value);
		std::cout << val << "\t= " << test->Id << std::endl;
		char s_truth[100];
		sprintf(s_truth, "(%.0f,%.0f,%.0f)", truth->x, truth->y, truth->z);
		char s_test[100];
		sprintf(s_test, "(%.0f,%.0f,%.0f)", test->x, test->y, test->z);

		if(xmlObject != NULL){
			itk::DOMNode * landmarksnode= xmlObject->GetChild("Landmarks");
			if(landmarksnode != NULL){
				itk::DOMNode::Pointer n = itk::DOMNode::New();
				itk::DOMNode* node =(itk::DOMNode*)n;
				node->SetName( "Landmark"); 
				node->SetAttribute( "id", test->Id );
				node->SetAttribute( "distance", val );
				node->SetAttribute( "metric", "Euclidean" );
				node->SetAttribute( "truth-point", s_truth );
				node->SetAttribute( "test-point", s_test );
				landmarksnode->AddChildAtEnd( node );
			}
		}
	}
	else{
		std::cout << "????????" << "\t= " << test->Id << " (No ground truth for this Landmark!)" << std::endl;
		char s_test[100];
		sprintf(s_test, "(%.0f,%.0f,%.0f)", test->x, test->y, test->z);
		if(xmlObject != NULL){
			itk::DOMNode * landmarksnode= xmlObject->GetChild("Landmarks");
			if(landmarksnode != NULL){
				itk::DOMNode::Pointer n = itk::DOMNode::New();
				itk::DOMNode* node =(itk::DOMNode*)n;
				node->SetName( "Landmark"); 
				node->SetAttribute( "id", test->Id );
				node->SetAttribute( "test-point", s_test );
				node->SetAttribute( "truth-point", "No ground truth for this Landmark!" );
				landmarksnode->AddChildAtEnd( node );
			}
		}
	}

}




int validateLocalization(const char* f1, const char* f2, const char* targetFile, char *options)
{
	

	string l_truth_file = "";
	string l_test_file = "";

	std::vector<Landmark> truth_landmarks;
	std::vector<Landmark> test_landmarks;

	bool truth_url=isUrl(f1);
	if(truth_url){
		l_truth_file = download_image(f1, "__temp_lcalization_file_truth.fcsv"); 
		parseLocalizationFile(l_truth_file, &truth_landmarks);
		remove(l_truth_file.c_str()) ;
	} 
	else{
		if(!itksys::SystemTools::FileExists(f1,true)){
		     cout << "Ground truth file doesn't exist: " << f1 << std::endl;
			 return 0;
		}
		parseLocalizationFile(f1, &truth_landmarks);
	}
	
	bool test_url=isUrl(f2);
	if(test_url){
	  l_test_file = download_image(f2, "__temp_lcalization_file_test.nii");
	  parseLocalizationFile(l_test_file, &test_landmarks);
	  remove(l_test_file.c_str());
	}
	else{
		if(!itksys::SystemTools::FileExists(f2,true)){
		     cout << "Test landmark file doesn't exist: " << f2 << std::endl;
			 return 0;
		}
	  parseLocalizationFile(f2, &test_landmarks);
	}

	itk::DOMNode::Pointer xmlObject = OpenLocalizationResultXML(targetFile, f1, f2 );

	std::cout << "\nLandmarks results:\n\n";

	for(vector<Landmark>::iterator it = test_landmarks.begin(); it != test_landmarks.end(); ++it){
		 Landmark *test_landmark = &(*it);
		 Landmark *truth_landmark = findMatchingLandmark(test_landmark, truth_landmarks);
		 compareLandmarks(truth_landmark, test_landmark, xmlObject);
	}


	SaveLocalizationResultXML(xmlObject, targetFile);
	std::cout << "\n  ---** VISCERAL 2013, www.visceral.eu **---\n" << std::endl;	
	return 0;
}





