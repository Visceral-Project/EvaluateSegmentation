/*
// EvaluateSegmentation.cpp
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 22.03.2013 
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
// This File is responsible for generating the EvaluateSegmentation executable which 
// takes the segmentation files to be evaluated as arguments. It then links the suitable evaluation 
// algorithm to perform the evaluation and then it links the program part responsible for 
// displaying the values or writting them in an xml file according to the set options.
//
*/


#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <time.h>
#include <sys/timeb.h>
#include "Global.h"
#include "Metric_constants.h"
#include "Outputter.h"
#include "Segmentation.h" 
#include "Localization.h" 


using namespace std;

void usage(int argc, char** argv){
	std::cout << "\nUSAGE:\n\n1) For volume segmentation:\n\n"  
		<<argv[0]<< " truthPath segmentPath [-thd threshold] [-xml xmlpath] [-use all|fast|DICE,JACRD,....]" << std::endl;
	std::cout << "\nwhere:" << std::endl;
	std::cout << "truthPath	=path (or URL) to truth image. URLs should be enclosed with quotations" << std::endl;
	std::cout << "segmentPath	=path (or URL) to image beeing evaluated. URLs should be enclosed with quotations" << std::endl;
	std::cout << "-th	=before evaluation convert fuzzy images to binary using threshold" << std::endl;
	std::cout << "-xml	=path to xml file where result should be saved" << std::endl;
	std::cout << "-help	=more information" << std::endl;	
	std::cout << "-use	=the metrics to be used. Note that additional options can be given between two @ characters:\n" << std::endl;
	std::cout << "	all	:use all available metrics (default)" << std::endl;
	for(int i=0 ; i< METRIC_COUNT ; i++){
		if(!metricInfo[i].testmetric){
			std::cout << "	" << metricInfo[i].metrId << "	:" << metricInfo[i].help<< std::endl;
		}
	}
	std::cout << "\n-default or -def =reads default options from a file default.txt in the current folder. All the options above except image filenames can be used as defaults. Default options are overridden by options given in the command line." << std::endl;

	std::cout << "\nExample:\n"<< argv[0]<< " truth.nii segment.nii -use RNDIND,HDRFDST@0.96@,FMEASR@0.5@ -xml result.xml" << std::endl;	

	std::cout << "\n\n2)For Localization:\n\n"  
		<<argv[0]<< " -loc truthLandmarkPath testLandmarkPath [-xml xmlpath]" << std::endl;
	std::cout << "\nwhere:" << std::endl;
	std::cout << "truthLandmarkPath	=path (or URL) to truth localizations. URLs should be enclosed with quotations" << std::endl;
	std::cout << "testLandmarkPath	=path (or URL) to testlandmarks beeing evaluated. URLs should be enclosed with quotations" << std::endl;
	std::cout << "-xml	=path to xml file where result should be saved" << std::endl;

	std::cout << "\n  ---** VISCERAL 2013, www.visceral.eu **---\n" << std::endl;	
}

bool shouldUse(MetricId id, char* options){
	string opt = options;
	if(opt.compare ("all") == 0){
		return true;
	}
	if(opt.compare ("visceral") == 0){
		int size = sizeof(VisceralMetrics)/sizeof(MetricId);
		for(int i=0 ; i< size; i++){
			if(VisceralMetrics[i] == id){
				return false;
			}
		}
		return true;
	}
	const char *metricid = metricInfo[id].metrId;
	if(opt.find(metricid) != string::npos){
		return true;
	}
	return false;
}

std::string getOptions(MetricId id, char* options){

	string opt = options;
	string metricid = metricInfo[id].metrId;
	int len = metricid.size();
	int ind1 = opt.find(metricid);
	int opt_len = opt.size();

	if(ind1 != string::npos){
		if(opt_len > (ind1+len) && opt.at(ind1+len) == '@'){
			ind1 = ind1 + len + 1;
			int ind2 = opt.find("@", ind1);
			if(ind2 != string::npos){
				std::string s = opt.substr(ind1, ind2-ind1);
				return s;
			}
		}
	}

	return nooption;
}

vector<string> parseDefaults(){
	vector<string> options;
	ifstream i_stream;
	std::string line, token;
	char* filename = (char*)"default.txt";
	i_stream.open(filename); 

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
			while (getline(inputss, token, ' ') )
			{
				trim(token);
				options.push_back(token);  
			}
		}
	}
	return options;
}

int main(int argc, char** argv)
{
	initMetricInfo();

	if(argc < 3){
		usage(argc, argv);
		return 0;
	}
	else if(std::string(argv[1]) == "-loc"){
		char* truthfile =  argv[2];
		char* testfile =  argv[3];
		char* targetfile = NULL;
		char* options = "all";
		for (int i = 1; i < argc; i++) { 
			if (std::string(argv[i]) == "-xml") {
				targetfile = argv[i + 1];
			}else if (std::string(argv[i]) == "-use") {
				options = argv[i + 1];
			}
		}


		try 
		{
			validateLocalization(truthfile, testfile, targetfile, options);
		} 
		catch (itk::ExceptionObject& e)
		{
			std::cerr << e << std::endl;
			return -1;
		}
		catch (std::exception& e)
		{
			cout << "Exception: " << e.what() << std::endl;
			return -1;
		}
		catch (std::string& s)
		{
			cout << "Exception: " << s << std::endl;
			return -1;
		}
		catch (...)
		{
			cout << "Unknown exception" << std::endl;
			return -1;
		}
		return 0;

	}
	else { 
		char* truthfile =  argv[1];
		char* testfile =  argv[2];
		char* targetfile = NULL;
		char* options = "all";
		double threshold = -1;
		bool use_default_config =false;

		for (int i = 1; i < argc; i++) { 
			if (std::string(argv[i]) == "-def" || std::string(argv[i]) == "-default") {
				use_default_config = true;
			}
		}

		if(use_default_config){
			vector<string> default_options = parseDefaults();
			if(default_options.size()>0){
				for(int i=0; i<default_options.size(); i++){
					if ( (i + 1) != default_options.size()){ 
						if (default_options[i] == "-th") {
							threshold = atof(default_options[i + 1].c_str());
						}else if (default_options[i] == "-xml") {
							 targetfile = new char[default_options[i + 1].length()];
                             strcpy(targetfile,default_options[i + 1].c_str());
						}else if (default_options[i] == "-use") {
							 options = new char[default_options[i + 1].length()];
                             strcpy(options,default_options[i + 1].c_str());
						}
					}
				}
			}
		}

		for (int i = 1; i < argc; i++) { 
			if ( (i + 1) != argc){ 
				if (std::string(argv[i]) == "-thd") {
					threshold = atof(argv[i + 1]);
				}else if (std::string(argv[i]) == "-xml") {
					targetfile = argv[i + 1];
				}else if (std::string(argv[i]) == "-use") {
					options = argv[i + 1];
				}
			}
			if (std::string(argv[i]) == "-def" || std::string(argv[i]) == "-default") {
				use_default_config = true;
			}
		}

		if(use_default_config){
		    cout << "\nUsing defalut.txt options: -use " << options;
			if(targetfile!=NULL){
				cout << " -xml " << targetfile;
			}
			if(threshold!=-1){
				cout << " -thd " << threshold;
			}

			cout << std::endl << std::endl;
		}

		try 
		{
			validateImage(truthfile, testfile, threshold, targetfile, options);
		} 
		catch (itk::ExceptionObject& e)
		{
			std::cerr << e << std::endl;
			return -1;
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << std::endl;
			return -1;
		}
		catch (std::string& s)
		{
			std::cerr << "Exception: " << s << std::endl;
			return -1;
		}
		catch (...)
		{
			std::cerr << "Unknown exception" << std::endl;
			return -1;
		}
		return 0;
	}
}



