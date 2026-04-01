#include "ImageManager.hpp"
#include "InputManager.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    try
    {
        std::string imageIndex = "1"; 
        
        if (argc >= 2) 
        {
            imageIndex = argv[1]; 
        }

        std::string filename = "assets/soru" + imageIndex + ".png";
        
        (void)EffectManager::InitConfig("config.txt");

        ImageManager imgMgr(filename, "Ahmet_Cevik", cv::Size(1280, 720));

        InputManager inputMgr(imgMgr);
        EffectManager::RegisterEffectEvents();

        ImageManager::instance->ShowImage(ImageManager::instance->GetOriginalImage());

        inputMgr.Listen();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return -1;
    }

    cv::destroyAllWindows();

    return 0;
}