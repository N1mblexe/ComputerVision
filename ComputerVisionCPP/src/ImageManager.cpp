#include "ImageManager.hpp"

ImageManager* ImageManager::instance = nullptr;

void ImageManager::SetImage(cv::Mat img, cv::String title)
{
    cv::setWindowTitle(windowName, title);
    cv::imshow(windowName, img);
}

ImageManager::ImageManager(cv::String path, cv::String windowName, cv::Size windowSize)
    : windowName(windowName)
{
    if(instance == nullptr)
        instance = this;

    originalImage = cv::imread(path);
    if (originalImage.empty())
        throw std::runtime_error("Image not found!");

    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, windowSize);
}

ImageManager::~ImageManager()
{
}

void ImageManager::ShowImage(cv::Mat image)
{
    cv::imshow(windowName , image);
}