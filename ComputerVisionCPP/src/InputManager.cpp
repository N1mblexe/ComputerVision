#include "InputManager.hpp"

InputManager *InputManager::instance = nullptr;

InputManager::InputManager(ImageManager &manager)
    : imgManager(manager), isRunning(true)
{
    if (instance == nullptr)
        instance = this;
}

void InputManager::RegisterEvent(char key, std::function<cv::Mat(cv::Mat)> function)
{
    events[key] = function;
}

void InputManager::Listen()
{
    while (isRunning)
    {
        int key = cv::waitKey(0);
        ProcessKey(key);
    }
}

void InputManager::ProcessKey(int key)
{
    char c = (char)key;

    if (c == 'q')
        isRunning = false;
    else if (c == 'r')
    {
        EffectManager::ReloadConfig();
        ProcessKey(static_cast<int>(EffectManager::currentEffect));
        return;
    }
    else if (c == KEY_DEFAULT)
    {
        ImageManager::instance->ShowImage(ImageManager::instance->GetOriginalImage());
        EffectManager::currentEffect = static_cast<EffectManager::Effect>(key);
        return;
    }

    auto it = events.find(c);
    if (it != events.end())
    {
        std::cout << it->first;
        cv::Mat resultImage = it->second(imgManager.GetOriginalImage());
        ImageManager::instance->ShowImage(resultImage);
        EffectManager::currentEffect = static_cast<EffectManager::Effect>(key);
    }
}