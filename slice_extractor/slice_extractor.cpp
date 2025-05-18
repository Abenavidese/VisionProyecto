#include "slice_extractor.h"

cv::Mat getSlice(ImageType::Pointer image, int z) {
    auto region = image->GetLargestPossibleRegion();
    int width = region.GetSize()[0];
    int height = region.GetSize()[1];

    cv::Mat slice(height, width, CV_32FC1);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            ImageType::IndexType idx = {x, y, z};
            slice.at<float>(y, x) = image->GetPixel(idx);
        }

    cv::normalize(slice, slice, 0, 255, cv::NORM_MINMAX);
    slice.convertTo(slice, CV_8UC1);
    return slice;
}

cv::Mat getMaskSlice(MaskType::Pointer mask, int z) {
    auto region = mask->GetLargestPossibleRegion();
    int width = region.GetSize()[0];
    int height = region.GetSize()[1];

    cv::Mat slice(height, width, CV_8UC1);
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            MaskType::IndexType idx = {x, y, z};
            slice.at<uchar>(y, x) = mask->GetPixel(idx) > 0 ? 255 : 0;
        }
    return slice;
}

