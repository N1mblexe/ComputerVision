#ifndef IMAGE_MANAGER
#define IMAGE_MANAGER
#include <opencv2/opencv.hpp>
#include "EffectManager.hpp"
#include "ConfigManager.hpp"

class ImageManager
{
public:
    cv::String GetWindowName() const { return windowName; }

private:
    cv::Mat originalImage;
    cv::String windowName;

    void SetImage(cv::Mat img, cv::String title);

public:
    static ImageManager *instance;

    ImageManager(cv::String path, cv::String windowName, cv::Size windowSize);
    ~ImageManager();

    void ShowImage(cv::Mat image);

    const cv::Mat GetOriginalImage() { return originalImage; }
};

#endif