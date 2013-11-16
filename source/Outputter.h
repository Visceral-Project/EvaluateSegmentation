/*
// Outputter.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 11.04.2013 
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
// This header file contains functions that display metric valuse and save them in an xml file
// The function here use the ITK XML handlers to save values in an xml target file.
//
*/

#ifndef _OUTPUTTER
#define _OUTPUTTER

#include <itkDOMNode.h>
#include <itkDOMNodeXMLReader.h>
#include <itkDOMNodeXMLWriter.h>
#include "itkFileTools.h"
#include "Metric_constants.h"

using namespace std;

itk::DOMNode*  AddNodeWithAttributeIfNotExists(itk::DOMNode* parentNode, const char *nodename, const char * attributename, const char* attributeValue){
	itk::DOMNode* node = parentNode->GetChild( nodename);
	if(node == NULL){ 
		itk::DOMNode::Pointer n = itk::DOMNode::New();
		node = (itk::DOMNode *)n;
		parentNode->AddChildAtEnd( node );
		node->SetName( nodename); 
	}
	if(attributename != NULL && attributeValue != NULL){
		node->SetAttribute( attributename, attributeValue );
	}
	return node;
}


itk::DOMNode::Pointer  OpenSegmentationResultXML(const char* targtfile, const char* fixedImage, const char* movingImage, int num_pt_f, int num_pt_m, int num_intersec){
	if(targtfile != NULL){
		itk::DOMNode::Pointer dOMObject;
		const char* xMLFileName = targtfile;
		if(itksys::SystemTools::FileExists(xMLFileName,true)){
			itk::DOMNodeXMLReader::Pointer reader = itk::DOMNodeXMLReader::New();
			reader->SetFileName( xMLFileName );
			reader->Update();
			dOMObject = reader->GetOutput();
		}
		else{
			dOMObject = itk::DOMNode::New();
		}
		char *nodename= "measurement";
		dOMObject->SetName( nodename);

		char val [50];

		AddNodeWithAttributeIfNotExists(dOMObject, "fixed-image", "filename", fixedImage);
		sprintf(val, "%d", num_pt_f-num_intersec);
		AddNodeWithAttributeIfNotExists(dOMObject, "fixed-image", "nonzeropoints", val);
		sprintf(val, "%d", num_intersec);
		AddNodeWithAttributeIfNotExists(dOMObject, "fixed-image", "intersection", val);

		AddNodeWithAttributeIfNotExists(dOMObject, "moving-image", "filename", movingImage);
		sprintf(val, "%d", num_pt_m-num_intersec);
		AddNodeWithAttributeIfNotExists(dOMObject, "moving-image", "nonzeropoints", val);
		sprintf(val, "%d", num_intersec);
		AddNodeWithAttributeIfNotExists(dOMObject, "moving-image", "intersection", val);


		return dOMObject;
	}
	return NULL;
}



void SaveXmlObject(itk::DOMNode::Pointer xmlObject, const char* targtfile){
	if(targtfile != NULL && xmlObject != (itk::DOMNode::Pointer)NULL){
	    itk::DOMNodeXMLWriter::Pointer writer = itk::DOMNodeXMLWriter::New();
		writer->SetInput(xmlObject );
		writer->SetFileName( targtfile );
		writer->Update();
	}

}


void pushValue(MetricId id, double value, itk::DOMNode::Pointer xmlObject){
	char val [50];
	sprintf(val, "%.6f", value);
	std::cout << metricInfo[id].metrSymb << "\t= " << val << "\t" << metricInfo[id].metrInfo << " "<< std::endl;
	if(xmlObject != (itk::DOMNode::Pointer)NULL){
		itk::DOMNode* metricnode = AddNodeWithAttributeIfNotExists((itk::DOMNode*)xmlObject, "metrics", NULL, NULL);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "name", metricInfo[id].metrInfo);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "symbol",  metricInfo[id].metrSymb);
		const char* type = metricInfo[id].similarity?"similarity":"distance";
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "type",  type);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "value", val);
	}

}


void pushValue(MetricId id, double value, double executiontime,  itk::DOMNode::Pointer xmlObject){
	char val [50];
	sprintf(val, "%.6f", value);
	std::cout << metricInfo[id].metrSymb << "\t= " << val << "\t" << metricInfo[id].metrInfo << " "<< std::endl;
	if(xmlObject != (itk::DOMNode::Pointer)NULL){
		itk::DOMNode* metricnode = AddNodeWithAttributeIfNotExists((itk::DOMNode*)xmlObject, "metrics", NULL, NULL);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "name", metricInfo[id].metrInfo);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "symbol",  metricInfo[id].metrSymb);
		const char* type = metricInfo[id].similarity?"similarity":"distance";
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "type",  type);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "value", val);

		sprintf(val, "%.0f", executiontime);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "executiontime", val);
	}

}



void pushMessage(const char *message, const char* targtfile, const char* fixedImage, const char* movingImage){
	std::cout <<  message << std::endl;
		if(targtfile != NULL){
		itk::DOMNode::Pointer dOMObject;
		const char* xMLFileName = targtfile;
		if(itksys::SystemTools::FileExists(xMLFileName,true)){
			itk::DOMNodeXMLReader::Pointer reader = itk::DOMNodeXMLReader::New();
			reader->SetFileName( xMLFileName );
			reader->Update();
			dOMObject = reader->GetOutput();
		}
		else{
			dOMObject = itk::DOMNode::New();
		}
		char *nodename= "measurement";
		dOMObject->SetName( nodename);

		AddNodeWithAttributeIfNotExists(dOMObject, "fixed-image", "filename", fixedImage);
		AddNodeWithAttributeIfNotExists(dOMObject, "moving-image", "filename", movingImage);
		itk::DOMNode* messagenode = AddNodeWithAttributeIfNotExists(dOMObject, "message", NULL, NULL);
		itk::FancyString s;
		s << message;
		messagenode->AddTextChildAtEnd( s );

		itk::DOMNodeXMLWriter::Pointer writer = itk::DOMNodeXMLWriter::New();
		writer->SetInput( dOMObject );
		writer->SetFileName( xMLFileName );
		writer->Update();
	}

}

bool isUrl(const char* path){
   string p = path;
   int ind = p.find("http:");
   if( ind != string::npos){
      return true;
   }
   ind = p.find("https:");
   if( ind != string::npos){
      return true;
   }
   ind = p.find("ftp:");
   if( ind != string::npos){
      return true;
   }
   ind = p.find("ftps:");
   if( ind != string::npos){
      return true;
   }

   return false;

}

 void trim(string& s)
  {
     size_t p = s.find_first_not_of(" \t");
     s.erase(0, p);
       p = s.find_last_not_of(" \t");
     if (string::npos != p)
         s.erase(p+1);
   }


#endif