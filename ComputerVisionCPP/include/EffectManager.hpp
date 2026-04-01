#ifndef EFFECT_MANAGER
#define EFFECT_MANAGER
#include <opencv2/opencv.hpp>
#include "ConfigManager.hpp"
#include "ImageManager.hpp"
#include <math.h>

#define KEY_DEFAULT (char)'0'
#define KEY_QUESTION1 (char)'1'
#define KEY_QUESTION2 (char)'2'
#define KEY_QUESTION3 (char)'3'
#define KEY_QUESTION4 (char)'4'
#define KEY_QUESTION5 (char)'5'

class EffectManager
{
public:
    enum class Effect
    {
        DEFAULT_IMAGE = KEY_DEFAULT,
        Question1 = KEY_QUESTION1,
        Question2 = KEY_QUESTION2,
        Question3 = KEY_QUESTION3,
        Question4 = KEY_QUESTION4,
        Question5 = KEY_QUESTION5
    };

    static Effect currentEffect;

    static ConfigManager *config;
    static ConfigManager InitConfig(std::string path);
    static void ReloadConfig();

    static void RegisterEffectEvents();
    
    static cv::Mat Question1(const cv::Mat &image);
    static cv::Mat Question2(const cv::Mat &image);
    static cv::Mat Question3(const cv::Mat &image);
    static cv::Mat Question4(const cv::Mat &image);
    static cv::Mat Question5(const cv::Mat &image);


private:
    static cv::Mat Helper_Question1(const cv::Mat &image);
    static cv::Mat Helper_Question2(const cv::Mat &image);
    static cv::Mat Helper_Question3(const cv::Mat &image);
    static cv::Mat Helper_Question4(const cv::Mat &image);
    static cv::Mat Helper_Question5(const cv::Mat &image);

};

#endif