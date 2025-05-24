#include "nifti_utils.h"
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkExtractImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

using ImageType3D = itk::Image<unsigned char, 3>;
using ImageType2D = itk::Image<unsigned char, 2>;

std::vector<cv::Mat> loadNiftiToCvSlices(const QString& path) {
    using ReaderType = itk::ImageFileReader<ImageType3D>;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(path.toStdString());
    reader->Update();
    ImageType3D::Pointer image = reader->GetOutput();

    std::vector<cv::Mat> output;
    ImageType3D::RegionType region = image->GetLargestPossibleRegion();
    int depth = region.GetSize()[2];

    for (int z = 0; z < depth; ++z) {
        ImageType3D::IndexType start = {0, 0, z};
        ImageType3D::SizeType size = {region.GetSize()[0], region.GetSize()[1], 0};
        ImageType3D::RegionType sliceRegion(start, size);

        using ExtractFilterType = itk::ExtractImageFilter<ImageType3D, ImageType2D>;
        ExtractFilterType::Pointer extractor = ExtractFilterType::New();
        extractor->SetExtractionRegion(sliceRegion);
        extractor->SetInput(image);
        extractor->SetDirectionCollapseToIdentity();
        extractor->Update();

        using RescaleType = itk::RescaleIntensityImageFilter<ImageType2D, ImageType2D>;
        RescaleType::Pointer rescaler = RescaleType::New();
        rescaler->SetInput(extractor->GetOutput());
        rescaler->SetOutputMinimum(0);
        rescaler->SetOutputMaximum(255);
        rescaler->Update();

        ImageType2D::Pointer slice = rescaler->GetOutput();
        cv::Mat cvSlice(slice->GetLargestPossibleRegion().GetSize()[1],
                        slice->GetLargestPossibleRegion().GetSize()[0],
                        CV_8UC1,
                        (void*)slice->GetBufferPointer());
        output.push_back(cv::Mat(cvSlice).clone());
    }
    return output;
}
