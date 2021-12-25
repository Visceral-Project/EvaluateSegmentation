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
itk::DOMNode::Pointer dOMObject;

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
		const char *nodename= "measurement";
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

}

void  pushTotalExecutionTime(long time,  itk::DOMNode::Pointer xmlObject){
	if(xmlObject != NULL){
		char val [50];
		sprintf(val, "%ld", time);
		const char* name="time";
		itk::DOMNode* node = dOMObject->GetChild(name);
		if(node == NULL){ 
			itk::DOMNode::Pointer n = itk::DOMNode::New();
			node = (itk::DOMNode *)n;
			dOMObject->AddChildAtEnd( node );
			node->SetName( name); 
		}
		node->SetAttribute( "total-executiontime", val );
	}
}

void  pushDimentions(int max_x, int max_y, int max_z, double vsp_x, double vsp_y, double vsp_z, itk::DOMNode::Pointer xmlObject){
	if(xmlObject != NULL){
		char* name="dimention";
		itk::DOMNode* node = dOMObject->GetChild(name);
		if(node == NULL){ 
			itk::DOMNode::Pointer n = itk::DOMNode::New();
			node = (itk::DOMNode *)n;
			dOMObject->AddChildAtEnd( node );
			node->SetName( name); 
		}
		char val [50];
		sprintf(val, "%d", max_x);
		node->SetAttribute( "max_x", val );
		sprintf(val, "%d", max_y);
		node->SetAttribute( "max_y", val );
		sprintf(val, "%d", max_z);
		node->SetAttribute( "max_z", val );

		sprintf(val, "%.6f", vsp_x);
		node->SetAttribute( "voxelspacing_x", val );

		sprintf(val, "%.6f", vsp_y);
		node->SetAttribute( "voxelspacing_y", val );

		sprintf(val, "%.6f", vsp_z);
		node->SetAttribute( "voxelspacing_z", val );

	}
}

void SaveXmlObject(itk::DOMNode::Pointer xmlObject, const char* targtfile){
	if(targtfile != NULL && xmlObject != NULL){
		itk::DOMNodeXMLWriter::Pointer writer = itk::DOMNodeXMLWriter::New();
		writer->SetInput(xmlObject );
		writer->SetFileName( targtfile );
		writer->Update();
	}

}

/*
void pushUnit(bool use_millimeter, itk::DOMNode::Pointer xmlObject){
	if(use_millimeter){
		std::cout << std::endl << "Units: \t millimeter for HDRFDST and AVGDIST, millliter for REFVOL and SEGVOL, otherwise voxel"<< std::endl;
	}
	else{
		std::cout << std::endl << "Units: \t voxel"<< std::endl;
	}
}
*/

static void pushValue(MetricId id, double value, itk::DOMNode::Pointer xmlObject, bool asInteger, const char* unit){
	char val [50];
        if(asInteger){
	  sprintf(val, "%.0f", value);
        }
        else{
	  sprintf(val, "%.6f", value);
        }

    char unit_s[50];
	if(unit != NULL)
	    sprintf(unit_s, " (in %s)", unit);
	else
		sprintf(unit_s, " %s", "");

	std::cout << metricInfo[id].metrSymb << "\t= " << val << "\t" << metricInfo[id].metrInfo << unit_s << std::endl;
	if(xmlObject != NULL){
		itk::DOMNode* metricnode = AddNodeWithAttributeIfNotExists((itk::DOMNode*)xmlObject, "metrics", NULL, NULL);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "name", metricInfo[id].metrInfo);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "symbol",  metricInfo[id].metrSymb);
		const char* type = metricInfo[id].similarity?"similarity":"distance";
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "type",  type);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "value", val);
		if(unit != NULL){
 	       AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "unit", unit);
		}
	}

}


static void pushValue(MetricId id, double value, double executiontime,  itk::DOMNode::Pointer xmlObject, const char* unit){
	char val [50];
	char exect [50];
	sprintf(val, "%.6f", value);
	
	char unit_s[50];
	if(unit != NULL)
	    sprintf(unit_s, " (in %s)", unit);
	else
		sprintf(unit_s, " %s", "");


	std::cout << metricInfo[id].metrSymb << "\t= " << val << "\t" << metricInfo[id].metrInfo << unit_s << std::endl;

	if(xmlObject != (itk::DOMNode*)NULL){
		itk::DOMNode* metricnode = AddNodeWithAttributeIfNotExists((itk::DOMNode*)xmlObject, "metrics", NULL, NULL);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "name", metricInfo[id].metrInfo);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "symbol",  metricInfo[id].metrSymb);
		const char* type = metricInfo[id].similarity?"similarity":"distance";
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "type",  type);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "value", val);

		sprintf(exect, "%.0f", executiontime);
		AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "executiontime", exect);

		if(unit != NULL){
 	       AddNodeWithAttributeIfNotExists(metricnode, metricInfo[id].metrId, "unit", unit);
		}
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
		const char *nodename= "measurement";
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

bool is2Dimage(const char* path){
	 
	string name = path;
	string extension = ".png";
	if (0 == name.compare (name.length() - extension.length(), extension.length(), extension))
		return true;
	extension = ".PNG";
	if (0 == name.compare (name.length() - extension.length(), extension.length(), extension))
		return true;

	 extension = ".jpg";
	if (0 == name.compare (name.length() - extension.length(), extension.length(), extension))
		return true;
		extension = ".JPG";
	if (0 == name.compare (name.length() - extension.length(), extension.length(), extension))
		return true;

	extension = ".gif";
	if (0 == name.compare (name.length() - extension.length(), extension.length(), extension))
		return true;
	extension = ".GIF";
	if (0 == name.compare (name.length() - extension.length(), extension.length(), extension))
		return true;

	 extension = ".bmp";
	if (0 == name.compare (name.length() - extension.length(), extension.length(), extension))
		return true;
	extension = ".BMP";
	if (0 == name.compare (name.length() - extension.length(), extension.length(), extension))
		return true;

	 extension = ".tiff";
	if (0 == name.compare (name.length() - extension.length(), extension.length(), extension))
		return true;
	extension = ".TIFF";
	if (0 == name.compare (name.length() - extension.length(), extension.length(), extension))
		return true;

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
