#ifndef INPUT_MANAGER
#define INPUT_MANAGER

#include <opencv2/opencv.hpp>
#include "ImageManager.hpp"

class InputManager
{
private:
    ImageManager& imgManager; 
    bool isRunning;
    std::map<char , std::function<cv::Mat(cv::Mat)>> events;

public:
    static InputManager* instance;

    InputManager(ImageManager& manager);
    
    void RegisterEvent(char key , std::function<cv::Mat(cv::Mat)> function);
    void Listen(); 
    void ProcessKey(int key);
};

#endif