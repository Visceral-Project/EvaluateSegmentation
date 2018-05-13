cmake_minimum_required(VERSION 2.8)
project(EvaluateSegmentation)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
endif()

# Placing header files in the executable helps with IDE project managment
set(HDRS
        AverageDistanceMetric.h
        ClassicMeasures.h
        CohinKappaMetric.h
        ContingencyTable.h
        DiceCoefficientMetric.h
        Global.h
        GlobalConsistencyError.h
        HausdorffDistanceMetric.h
        ImageStatistics.h
        Imagedownloader.h
        InterclassCorrelationMetric.h
        JaccardCoefficientMetric.h
        LesionDetection.h
        LesionDetectionConst.h
        LesionDetectionMask.h
        Localization.h
        MahalanobisDistanceMetric.h
        Metric_constants.h
        MutualInformationMetric.h
        Outputter.h
        ProbabilisticDistanceMetric.h
        RandIndexMetric.h
        Segmentation.h
        VariationOfInformationMetric.h
        VolumeSimilarityCoefficient.h
        VoxelPreprocessor.h)

add_executable(EvaluateSegmentation MACOSX_BUNDLE EvaluateSegmentation.cxx ${HDRS})
if( "${ITK_VERSION_MAJOR}" LESS 4 )
  target_link_libraries(EvaluateSegmentation ITKReview ${ITK_LIBRARIES})
else( "${ITK_VERSION_MAJOR}" LESS 4 )
  target_link_libraries(EvaluateSegmentation ${ITK_LIBRARIES})
endif( "${ITK_VERSION_MAJOR}" LESS 4 )




