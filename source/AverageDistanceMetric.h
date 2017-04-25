/*
// AverageDistanceMetric.h
// VISERAL Project http://www.viceral.eu
// VISCERAL received funding from EU FP7, contract 318068 
// Created by Abdel Aziz Taha (taha@ifs.tuwien.ac.at)
// on 28.05.2013 
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
// This algorithm is responsible for calculating the Average Hausdorff Distance Metric between two volumes.
// The algorithm uses the ITK Library for accessing the image data (voxels), namely the voxel iterators
// It then performs the rest of the calculation by its own. 
//
*/


#include "itkImage.h"
#include <itkVector.h>
#include "itkLineIterator.h"
#include "itkImageFileWriter.h"
#include "itkConnectedComponentImageFilter.h"


class AverageDistanceMetric
{

	int grid_len;
	int grid_num_x;
	int grid_num_y;
	int grid_num_z;

	double max_extent;
	int max_x;
	int max_y;
	int max_z;
	
    double spx;
    double spy;
    double spz;

	typedef struct VoxelInfo{
		int x;
		int y;
		int z;
		double value;
	} VoxelInfo;

	typedef struct Cell{
		double x1;
		double y1;
		double z1;
		double x2;
		double y2;
		double z2;
		std::vector<VoxelInfo> voxels;
		bool emp;
	} Cell;


	typedef itk::Vector<double, 3> V2;
	typedef itk::ImageRegionConstIterator<ImageType> IteratorType;


private:
	ImageType *fixedImage;
	ImageType *movingImage;
	bool fuzzy;
	double thd;
	bool emp_f;
	bool emp_m;
	double maxValue;

public:
	~AverageDistanceMetric(){

	}

	AverageDistanceMetric(ImageType *fixedImage, ImageType *movingImage, bool fuzzy, double threshold, bool millimeter){
		this->fixedImage = fixedImage;
		this->movingImage = movingImage;
		this->fuzzy = fuzzy;
		grid_len =7;
        this->thd = 0;
		if(!fuzzy && threshold!=-1){
		    this->thd = threshold*PIXEL_VALUE_RANGE_MAX;
		}
		else{
		    this->thd = 0.5*PIXEL_VALUE_RANGE_MAX;
		}
		
		const ImageType::SpacingType & ImageSpacing = fixedImage->GetSpacing();
		if(millimeter){
           spx = ImageSpacing[0];
           spy = ImageSpacing[1];
           spz = ImageSpacing[2];
		   if(spx==0){
			   spx=1;
		   }
		   if(spy==0){
			  spy=1;
		   }
		   if(spz==0){
			  spz=1;
		   }

		}
		else{
           spx = 1;
           spy = 1;
           spz = 1;
		
		}

	}

	double CalcAverageDistace(bool prune){
		double dist1 = calc(fixedImage, movingImage, false);
		double dist2 = calc(movingImage, fixedImage, false);
		return (dist1 + dist2)/2;
	}

	std::vector<Cell> build(){
		std::vector<Cell> cs;
		grid_num_x = max_x/grid_len+1;
		grid_num_y = max_y/grid_len+1;
		grid_num_z = max_z/grid_len+1;
		for(double x=0; x < grid_num_x; x++){
			for(double y=0; y < grid_num_y; y++){
				for(double z=0; z < grid_num_z; z++){
					Cell gc;
					gc.x1=x*grid_len;
					gc.x2=x*grid_len+grid_len;
					gc.y1=y*grid_len;
					gc.y2=y*grid_len+grid_len;
					gc.z1=z*grid_len;
					gc.z2=z*grid_len+grid_len;
					cs.push_back(gc);
					gc.emp=true;
				}
			}
		}
		return cs;
	}


	double calc(ImageType *image1, ImageType *image2, bool exhaust_search){
		std::vector<VoxelInfo> empSeg;
		int numberfalseNegatives=0;
		int numberTruePositives=0;
		int numberFalsePositives=0;
		max_x =0;
		max_y =0;
		max_z =0;
		IteratorType fixedIt(image1, image1->GetRequestedRegion());
		IteratorType movingIt(image2, image2->GetRequestedRegion());
		emp_f=true;
		emp_m=true;
		fixedIt.GoToBegin();
		movingIt.GoToBegin();
		while (!fixedIt.IsAtEnd() && !movingIt.IsAtEnd()){
			ImageType::IndexType index = movingIt.GetIndex();
			if(fixedIt.Get()>thd && movingIt.Get()<=thd){
				emp_f=false;
				numberfalseNegatives++;
			}
			if(movingIt.Get()>thd){
				emp_m=false;
				if(fixedIt.Get()>thd){
					emp_f=false;
					numberTruePositives++;
				}
				if(index[0]>max_extent){
					max_extent = index[0];
				}
				if(index[1]>max_extent){
					max_extent = index[1];
				}
				if(index[2]>max_extent){
					max_extent = index[2];
				}
			}
			if(index[0]>max_x){
				max_x = index[0];
			}
			if(index[1]>max_y){
				max_y = index[1];
			}
			if(index[2]>max_z){
				max_z = index[2];
			}

#ifdef _DEBUG
			if(movingIt.Get()>thd && fixedIt.Get()<=thd ){
				numberFalsePositives++;
			}
#endif

			++movingIt;
			++fixedIt;
		}
		if(numberfalseNegatives==0){
			return 0;
		}
		std::vector<Cell> index = build();
		VoxelInfo* falseNegatives = new VoxelInfo[numberfalseNegatives];
		int fp_ind=0;
		int tp_ind=0;
#ifdef _DEBUG
		VoxelInfo* truePositives;
		VoxelInfo* falsePositives;
		falsePositives = new VoxelInfo[numberFalsePositives];
		truePositives = new VoxelInfo[numberTruePositives];
#endif

		fixedIt.GoToBegin();
		movingIt.GoToBegin();
		int FP_index=0;
		int FN_index=0;

		while (!fixedIt.IsAtEnd() && !movingIt.IsAtEnd()){
			if(fixedIt.Get()>thd && movingIt.Get()<=thd){
				falseNegatives[FN_index].value = fixedIt.Get();
				falseNegatives[FN_index].x = fixedIt.GetIndex()[0];
				falseNegatives[FN_index].y = fixedIt.GetIndex()[1];
				falseNegatives[FN_index].z = fixedIt.GetIndex()[2];
				FN_index++;
			}

			if(movingIt.Get()>thd){
				VoxelInfo vi;
				vi.value = movingIt.Get();
				vi.x = movingIt.GetIndex()[0];
				vi.y = movingIt.GetIndex()[1];
				vi.z = movingIt.GetIndex()[2];
				bool surface = isBoundary(movingIt.GetIndex(), image2);
				if(surface){
					empSeg.push_back(vi);
					int x_ind = vi.x/grid_len;
					int y_ind = vi.y/grid_len;
					int z_ind = vi.z/grid_len;
					Cell *gc = &index[z_ind + y_ind * grid_num_z + x_ind * grid_num_y*grid_num_z];
					gc->voxels.push_back(vi);
					gc->emp=false;
					FP_index++;
				}
			}

#ifdef _DEBUG
			if(fixedIt.Get()<=thd && movingIt.Get()>thd){
				falsePositives[fp_ind].value = fixedIt.Get();
				falsePositives[fp_ind].x = fixedIt.GetIndex()[0];
				falsePositives[fp_ind].y = fixedIt.GetIndex()[1];
				falsePositives[fp_ind].z = fixedIt.GetIndex()[2];
				fp_ind++;
			}
			else if(fixedIt.Get()>thd && movingIt.Get()>thd){
				truePositives[tp_ind].value = fixedIt.Get();
				truePositives[tp_ind].x = fixedIt.GetIndex()[0];
				truePositives[tp_ind].y = fixedIt.GetIndex()[1];
				truePositives[tp_ind].z = fixedIt.GetIndex()[2];
				tp_ind++;

			}
#endif
			++movingIt;
			++fixedIt;
		}

		double AVD_SUM = 0;
		double AVD_NUM = 0;
		maxValue = 10000000000;
		V2 mean = calcMean(image2);

		int rcount=0;

		for(int ind=0 ; ind< numberfalseNegatives ; ind++){
			VoxelInfo p1 = falseNegatives[ind];   
			double x = p1.x;
			double y = p1.y;
			double z = p1.z;
			double min = maxValue;
			VoxelInfo closest;
			bool found =false;

			double radius = -1;
			if(!exhaust_search){
				radius = calcSph(p1, image2, mean); 
			}


			if(radius!=-1){
				//double radius = 50;
				int x_min_index = (x - radius)/(grid_len);
				if(x_min_index<0) 
					x_min_index=0;
				int x_max_index = (x + radius)/(grid_len);
				if(x_max_index>=grid_num_x) 
					x_max_index=grid_num_x - 1;

				int y_min_index = (y - radius)/(grid_len);
				if(y_min_index<0) 
					y_min_index=0;
				int y_max_index = (y + radius)/(grid_len);
				if(y_max_index >= grid_num_y) 
					y_max_index = grid_num_y - 1;

				int z_min_index = (z - radius)/(grid_len);
				if(z_min_index < 0) 
					z_min_index = 0;
				int z_max_index = (z + radius)/(grid_len);
				if(z_max_index >= grid_num_z) 
					z_max_index = grid_num_z - 1;

				for(int i_x=x_min_index ; i_x <= x_max_index ; i_x++ ){
					for(int i_y=y_min_index ; i_y <= y_max_index ; i_y++ ){
						for(int i_z=z_min_index ; i_z <= z_max_index ; i_z++ ){
							Cell *gc = &index[i_z + i_y * grid_num_z + i_x * grid_num_y*grid_num_z];

							if(gc->emp){
								continue;
							}
							int size=gc->voxels.size();
							//std::cout << " size: "<< size<< std::endl;
							for(int i=0; i< size;i++){
								VoxelInfo p2 = gc->voxels[i];
								
								double dist =std::sqrt((double)(
					               (p2.x-x)*(p2.x-x) *this->spx*this->spx
					               + (p2.y-y)*(p2.y-y) *this->spy*this->spy
					               + (p2.z-z)*(p2.z-z) *this->spz*this->spz
					               ));
					   
								//double dist = (p2.x-x)*(p2.x-x) + (p2.y-y)*(p2.y-y) + (p2.z-z)*(p2.z-z);
								if(dist<min){
									closest = p2;
									min = dist;
									found=true;
								}
							}

						}
					}
				}

			}
			else{
				rcount++;
				int size=empSeg.size();
				for(int i=0; i< size;i++){
					VoxelInfo p2 = empSeg[i];
					
					double dist =std::sqrt((double)(
					               (p2.x-x)*(p2.x-x) *this->spx*this->spx
					               + (p2.y-y)*(p2.y-y) *this->spy*this->spy
					               + (p2.z-z)*(p2.z-z) *this->spz*this->spz
					               ));
								   
					//double dist = (p2.x-x)*(p2.x-x) + (p2.y-y)*(p2.y-y) + (p2.z-z)*(p2.z-z);
					if(dist<min){
						closest = p2;
						min = dist;
					}
				}
			}

			if(ind%1000==0){
				//if(exhaust_search)
				//	std::cout << ind<< " of "<< numberfalseNegatives<< " rcount:" << rcount << std::endl;
				rcount=0;
			}

			min =std::sqrt((double)(
					               (closest.x-x)*(closest.x-x) *this->spx*this->spx
					               + (closest.y-y)*(closest.y-y) *this->spy*this->spy
					               + (closest.z-z)*(closest.z-z) *this->spz*this->spz
					               ));
								   
			//min = std::sqrt( (closest.x-x)*(closest.x-x) + (closest.y-y)*(closest.y-y) + (closest.z-z)*(closest.z-z) );
			AVD_SUM += min;
			AVD_NUM ++;

		}


#ifdef _DEBUG
		//saveImage(falseNegatives, numberfalseNegatives, "falseNegatives.mha", max_x, max_y, max_z);
		//saveImage(falsePositives, numberFalsePositives, "falsePositives.mha", max_x, max_y, max_z);
		//saveImage(truePositives, numberTruePositives, "truePositives.mha", max_x, max_y, max_z);
		//VoxelInfo* surface = new VoxelInfo[FP_index];
		//for(int i=0; i< empSeg.size() ; i++){
		//	VoxelInfo p= empSeg[i];
		//	surface[i]=p;
		//}
		//saveImage(surface, FP_index, "surface.mha", max_x, max_y, max_z);
#endif

		return AVD_SUM/(AVD_NUM+numberTruePositives);

	}

	double calcSph(VoxelInfo p, ImageType *image, V2 mean){
		
		double x1 = p.x;
		double y1 = p.y;
		double z1 = p.z;
		double x2 = mean[0];
		double y2 = mean[1];
		double z2 = mean[2];
		//std::cout << " MEAN: "<< x2 << ", " << y2 << ", " << z2 << std::endl;
		
		double distance =std::sqrt((double)(
					               (x2-x1)*(x2-x1) *this->spx*this->spx
					               + (y2-y1)*(y2-y1) *this->spy*this->spy
					               + (z2-z1)*(z2-z1) *this->spz*this->spz
					               ));
								   
		//double distance = std::sqrt((double)((x2-x1)*(x2-x1)+ (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1)));
		double x_factor = (x2-x1)/distance;
		double y_factor = (y2-y1)/distance;
		double z_factor = (z2-z1)/distance;

		
		int step = 1;
		ImageType::IndexType point;
		for( int i =1;; i=i+step){
				double x = x1 + i * x_factor;
				double y = y1 + i * y_factor;
				double z = z1 + i * z_factor;
				if(x>max_x || y>max_y || z>max_z || x<0 || y<0 || z<0){
					return -1;
				}
				point[0]=x;
				point[1]=y;
				point[2]=z;

				ImageType::PixelType pix = image->GetPixel(point);
				if(pix > thd){
					return (double)i;
				}

		}
		
		return -1;
	}



	bool isBoundary(ImageType::IndexType index, ImageType *image){
		ImageType::IndexType i;
		ImageType::PixelType pix;

		i[0] = index[0]+1; 
		if(i[0]>max_x)
			return true;
		i[1] = index[1]; 
		i[2] = index[2]; 
		pix = image->GetPixel(i);
		if(pix <= thd){
			return true;
		}

		i[0] = index[0]-1; 
		if(i[0]<0)
			return true;
		i[1] = index[1]; 
		i[2] = index[2]; 
		pix = image->GetPixel(i);
		if(pix <= thd){
			return true;
		}

		i[0] = index[0]; 
		i[1] = index[1]+1; 
		if(i[1]>max_y)
			return true;
		i[2] = index[2]; 
		pix = image->GetPixel(i);
		if(pix <= thd){
			return true;
		}

		i[0] = index[0]; 
		i[1] = index[1]-1; 
		if(i[1]<0)
			return true;
		i[2] = index[2]; 
		pix = image->GetPixel(i);
		if(pix <= thd){
			return true;
		}

		i[0] = index[0]; 
		i[1] = index[1]; 
		i[2] = index[2]+1; 
		if(i[2]>max_z)
			return true;
		pix = image->GetPixel(i);
		if(pix <= thd){
			return true;
		}

		i[0] = index[0]; 
		i[1] = index[1]; 
		i[2] = index[2]-1; 
		if(i[2]<0)
			return true;
		pix = image->GetPixel(i);
		if(pix <= thd){
			return true;
		}

		return false;
	}

	V2 calcMean(ImageType *image){
		IteratorType it(image, image->GetRequestedRegion());
		it.GoToBegin();
		V2 mat;
		mat.Fill(0); 
		double count=0;
		it.GoToBegin();
		while (!it.IsAtEnd()){
			double val = it.Value();
			if(val>thd){
				ImageType::IndexType index = it.GetIndex();
				mat[0] += index[0];
				mat[1] += index[1];
				mat[2] += index[2];
				count++;
			}
			++it;
		}
		mat = mat/count;
		return mat;

	}

	bool IsFixedImageEmpty(){
		return emp_f;
	}
	bool IsMovingImageEmpty(){
		return emp_m;
	}

#ifdef _DEBUG
	void saveImage(VoxelInfo* points, int num_points, char* path, int width, int length, int height){
		ImageType::Pointer image1 = ImageType::New();
		ImageType::IndexType start;
		start[0] = 0;
		start[1] = 0;
		start[2] = 0;
		ImageType::SizeType size;
		size[0] = width+1;
		size[1] = length+1;
		size[2] = height+1;
		ImageType::RegionType region(start, size);
		image1->SetRegions(region);
		image1->Allocate();
		itk::ImageRegionIteratorWithIndex<ImageType>  initIt(image1, region);
		initIt.GoToBegin();
		while (!initIt.IsAtEnd()){
			initIt.Set(0);
			++initIt;
		}

		for(int i=0; i< num_points ;i++){
			VoxelInfo vi= points[i];
			ImageType::IndexType ind;
			ind[0]=vi.x;
			ind[1]=vi.y;
			ind[2]=vi.z;
			image1->SetPixel(ind,1);

		}

		typedef  itk::ImageFileWriter< ImageType  > WriterType;
		WriterType::Pointer writer = WriterType::New();
		writer->SetFileName(path);
		writer->SetInput(image1);
		//writer->SetUseCompression (true);
		writer->Update();
	}
#endif

};

