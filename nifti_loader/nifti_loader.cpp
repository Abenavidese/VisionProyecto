#include "nifti_loader.h"
#include "itkImageFileReader.h"
#include "itkNiftiImageIOFactory.h"

ImageType::Pointer loadImage(const std::string& filename) {
    itk::NiftiImageIOFactory::RegisterOneFactory();
    using ReaderType = itk::ImageFileReader<ImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(filename);
    reader->Update();
    return reader->GetOutput();
}

MaskType::Pointer loadMask(const std::string& filename) {
    itk::NiftiImageIOFactory::RegisterOneFactory();
    using ReaderType = itk::ImageFileReader<MaskType>;
    auto reader = ReaderType::New();
    reader->SetFileName(filename);
    reader->Update();
    return reader->GetOutput();
}
