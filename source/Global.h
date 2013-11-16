/*
// Global.h
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
// Global constants that holds in the entire code  
//
*/

#ifndef _GLOBAL
#define _GLOBAL
#include <vector>
#include "itkImage.h"

// Please let the switch _DEBUG undefined to be sure that the code is compilable and correctly works
// This is because there may be files and functions that are not available  
// such as classes for testing and code under development.   
#undef _DEBUG

#define PI 3.14159265

static const int PIXEL_VALUE_RANGE_MIN=0;
static const int PIXEL_VALUE_RANGE_MAX=255;

//#define DATATYPE_UNSIGNED_INT
//#define DATATYPE_INT
//#define DATATYPE_FLOAT
//#define DATATYPE_DOUBLE
//#define DATATYPE_UNSIGNED_SHORT
#define DATATYPE_UNSIGNED_CHAR
//#define DATATYPE_BOOLEAN


#ifdef DATATYPE_UNSIGNED_INT
   typedef unsigned int  pixeltype;
   typedef itk::Image< pixeltype, 3> ImageType;
#endif

#ifdef DATATYPE_UNSIGNED_SHORT
   typedef unsigned short  pixeltype;
   typedef itk::Image< pixeltype, 3> ImageType;
#endif

#ifdef DATATYPE_INT
   typedef int  pixeltype;
   typedef itk::Image< pixeltype, 3> ImageType;
#endif

#ifdef DATATYPE_FLOAT
 typedef float  pixeltype;
 typedef itk::Image< pixeltype, 3> ImageType;
#endif

#ifdef DATATYPE_DOUBLE
   typedef double  pixeltype;
   typedef itk::Image< pixeltype, 3> ImageType;
#endif

#ifdef DATATYPE_UNSIGNED_CHAR
   typedef unsigned char  pixeltype;
   typedef itk::Image< pixeltype, 3> ImageType;
#endif


#ifdef DATATYPE_BOOLEAN
   typedef bool  pixeltype;
   typedef itk::Image< pixeltype, 3> ImageType;
#endif

static pixeltype *values_f;
static pixeltype *values_m;


#endif