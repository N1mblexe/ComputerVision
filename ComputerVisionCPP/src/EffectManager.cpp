#include "EffectManager.hpp"
#include "ImageManager.hpp"
#include "InputManager.hpp"

#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>

ConfigManager *EffectManager::config = nullptr;
EffectManager::Effect EffectManager::currentEffect = EffectManager::Effect::DEFAULT_IMAGE;

void EffectManager::ReloadConfig()
{
    config->Load();
}

ConfigManager EffectManager::InitConfig(std::string path)
{
    config = new ConfigManager(path);
    return *config;
}

void EffectManager::RegisterEffectEvents()
{
    InputManager::instance->RegisterEvent(KEY_QUESTION1, Question1);
    InputManager::instance->RegisterEvent(KEY_QUESTION2, Question2);
    InputManager::instance->RegisterEvent(KEY_QUESTION3, Question3);
    InputManager::instance->RegisterEvent(KEY_QUESTION4, Question4);
    InputManager::instance->RegisterEvent(KEY_QUESTION5, Question5);

}

cv::Mat EffectManager::Question1(const cv::Mat &image)
{
    return Helper_Question1(image);
}

cv::Mat EffectManager::Question2(const cv::Mat &image)
{
    return Helper_Question2(image);
}

cv::Mat EffectManager::Question3(const cv::Mat &image)
{
    return Helper_Question3(image);
}

cv::Mat EffectManager::Question4(const cv::Mat &image)
{
    return Helper_Question4(image);
}

cv::Mat EffectManager::Question5(const cv::Mat &image)
{
    return Helper_Question5(image);
}

cv::Mat DetectEdges(const cv::Mat &inputImage)
{
    cv::Mat gray;
    if (inputImage.channels() == 3)
    {
        cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
    }
    else
    {
        gray = inputImage.clone();
    }

    cv::Mat edges = cv::Mat::zeros(gray.size(), CV_8UC1);

    int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}};

    int Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}};

    for (int y = 1; y < gray.rows - 1; ++y)
    {
        for (int x = 1; x < gray.cols - 1; ++x)
        {
            int sumX = 0;
            int sumY = 0;

            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    int pixelValue = gray.at<uchar>(y + i, x + j);
                    sumX += pixelValue * Gx[i + 1][j + 1];
                    sumY += pixelValue * Gy[i + 1][j + 1];
                }
            }

            int magnitude = std::sqrt(sumX * sumX + sumY * sumY);

            edges.at<uchar>(y, x) = static_cast<uchar>((magnitude > 70) ? 255 : 0);
        }
    }

    return edges;
}

cv::Mat VerticalAdaptiveThreshold(const cv::Mat &inputImg, const cv::Mat &edgeImg, int numDivisions = 10)
{
    if (inputImg.empty() || edgeImg.empty())
    {
        throw std::invalid_argument("Input image or edge image is empty.");
    }

    cv::Mat grayImg;

    if (inputImg.channels() == 3)
    {
        cv::cvtColor(inputImg, grayImg, cv::COLOR_BGR2GRAY);
    }
    else
    {
        grayImg = inputImg;
    }

    cv::Mat outputImg = cv::Mat::zeros(grayImg.size(), CV_8UC1);

    int width = grayImg.cols;
    int stepSize = width / numDivisions;

    for (int i = 0; i < numDivisions; ++i)
    {
        int startX = i * stepSize;
        int endX = (i == numDivisions - 1) ? width : (i + 1) * stepSize;

        cv::Rect roi(startX, 0, endX - startX, grayImg.rows);

        cv::Mat edgeStrip = edgeImg(roi);

        if (cv::countNonZero(edgeStrip) > 0)
        {
            cv::Mat strip = grayImg(roi);
            cv::Mat binaryStrip;

            cv::threshold(strip, binaryStrip, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

            binaryStrip.copyTo(outputImg(roi));
        }
    }

    return outputImg;
}

cv::Mat EffectManager::Helper_Question1(const cv::Mat &image)
{
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat edges = DetectEdges(image);
    cv::Mat out = VerticalAdaptiveThreshold(image, edges);
    return out;
}

#define CSIZE 256
typedef std::array<size_t, CSIZE> histogram;

histogram GetHist(const cv::Mat &image)
{
    std::array<size_t, CSIZE> out;
    out.fill(0);

    if (image.empty() || image.channels() != 1)
        return out;

    for (size_t x = 0; x < (size_t)image.rows; x++)
    {
        for (size_t y = 0; y < (size_t)image.cols; y++)
        {
            uchar index = image.at<uchar>(x, y);

            out[index]++;
        }
    }

    return out;
}

std::array<double, CSIZE> GetPDF(const histogram hist, size_t totalPixels = 0)
{
    if (totalPixels == 0)
    {
        for (size_t i = 0; i < hist.size(); i++)
            totalPixels += hist[i];
    }

    std::array<double, CSIZE> pdf;

    for (size_t i = 0; i < hist.size(); i++)
    {
        pdf[i] = static_cast<double>(hist[i]) / totalPixels;
    }

    return pdf;
}

std::array<uchar, CSIZE> GetEqualizationLUT(const std::array<double, CSIZE> &pdf)
{
    double sumProb = 0.0;
    std::array<uchar, CSIZE> lut;

    for (size_t i = 0; i < CSIZE; i++)
    {
        sumProb += pdf[i];

        lut[i] = static_cast<uchar>(std::round(sumProb * (CSIZE - 1)));
    }
    return lut;
}

cv::Mat EqualizeImage(const cv::Mat &input, const std::array<uchar, CSIZE> &lut)
{
    cv::Mat output = input.clone();

    for (int x = 0; x < output.rows; x++)
        for (int y = 0; y < output.cols; y++)
            output.at<uchar>(x, y) = lut[output.at<uchar>(x, y)];

    return output;
}

cv::Mat ContrastStretching(const cv::Mat &input, const histogram &hist)
{
    cv::Mat out = cv::Mat::zeros(input.size(), input.type());

    uchar minVal = 0;
    uchar maxVal = 255;

    for (int i = 0; i < CSIZE; i++)
    {
        if (hist[i] > 0)
        {
            minVal = (uchar)i;
            break;
        }
    }

    for (int i = CSIZE - 1; i >= 0; i--)
    {
        if (hist[i] > 0)
        {
            maxVal = (uchar)i;
            break;
        }
    }

    std::array<uchar, CSIZE> lut;

    double scale = 255.0 / (maxVal - minVal);

    for (int i = 0; i < CSIZE; i++)
    {
        if (i < minVal)
            lut[i] = 0;
        else if (i > maxVal)
            lut[i] = 255;
        else
        {
            lut[i] = (uchar)std::round((i - minVal) * scale);
        }
    }

    for (int x = 0; x < input.rows; x++)
    {
        for (int y = 0; y < input.cols; y++)
        {
            uchar pixel = input.at<uchar>(x, y);
            out.at<uchar>(x, y) = lut[pixel];
        }
    }

    return out;
}

std::array<uchar, CSIZE> GetGammaLUT(double gamma)
{
    std::array<uchar, CSIZE> lut;
    for (int i = 0; i < CSIZE; i++)
    {
        lut[i] = cv::saturate_cast<uchar>(std::pow(i / 255.0, gamma) * 255.0);
    }
    return lut;
}

cv::Mat EffectManager::Helper_Question2(const cv::Mat &image)
{
    cv::Mat gray;
    if (image.channels() == 3)
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else
        gray = image.clone();

    // Hist eq
    histogram histOrig = GetHist(gray);
    std::array<double, CSIZE> pdf = GetPDF(histOrig);
    std::array<uchar, CSIZE> eqLut = GetEqualizationLUT(pdf);
    cv::Mat stage1 = EqualizeImage(gray, eqLut);

    // Contrast
    histogram histStage1 = GetHist(stage1);
    cv::Mat stage2 = ContrastStretching(stage1, histStage1);

    // Gamma
    double gammaVal = config->Get("Gamma_Value");
    std::array<uchar, CSIZE> gammaLut = GetGammaLUT(gammaVal);

    cv::Mat finalResult = EqualizeImage(stage2, gammaLut);

    return finalResult;
}

cv::Mat EffectManager::Helper_Question3(const cv::Mat &image)
{
    if (image.empty())
        return image;

    int k = 4;
    cv::Mat processed_image;

    cv::bilateralFilter(image, processed_image, 15, 80, 80);

    cv::Mat reshaped_image = processed_image.reshape(1, processed_image.total());

    cv::Mat float_image;
    reshaped_image.convertTo(float_image, CV_32F);

    cv::Mat labels;
    cv::Mat centers;

    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 0.2);

    cv::kmeans(float_image, k, labels, criteria, 3, cv::KMEANS_PP_CENTERS, centers);

    cv::Mat centers_u8;
    centers.convertTo(centers_u8, CV_8U);

    cv::Mat quantized_image = cv::Mat::zeros(image.size(), image.type());

    int channels = image.channels();

    for (int i = 0; i < image.total(); ++i)
    {
        int cluster_idx = labels.at<int>(i);

        if (channels == 3)
        {
            quantized_image.at<cv::Vec3b>(i)[0] = centers_u8.at<uchar>(cluster_idx, 0);
            quantized_image.at<cv::Vec3b>(i)[1] = centers_u8.at<uchar>(cluster_idx, 1);
            quantized_image.at<cv::Vec3b>(i)[2] = centers_u8.at<uchar>(cluster_idx, 2);
        }
        else if (channels == 1)
        {
            quantized_image.at<uchar>(i) = centers_u8.at<uchar>(cluster_idx, 0);
        }
    }

    return quantized_image;
}

size_t currSharpening = 0;

cv::Mat EffectManager::Helper_Question4(const cv::Mat &image)
{
    if (image.empty())
        return image;

    cv::Mat blurred;
    cv::blur(image, blurred, cv::Size(7, 7));

    // Laplace Sharpening
    cv::Mat laplace_sharpened;
    cv::Mat laplace_kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0,
                              -1, 5, -1,
                              0, -1, 0);
    cv::filter2D(blurred, laplace_sharpened, -1, laplace_kernel);

    // Laplacian of Gaussian (LoG) Sharpening
    cv::Mat log_sharpened;
    cv::Mat log_kernel = (cv::Mat_<float>(5, 5) << 0, 0, -1, 0, 0,
                          0, -1, -2, -1, 0,
                          -1, -2, 17, -2, -1,
                          0, -1, -2, -1, 0,
                          0, 0, -1, 0, 0);
    cv::filter2D(blurred, log_sharpened, -1, log_kernel);

    // High Boost Sharpening
    cv::Mat highboost_sharpened;
    float A = 1.5f;
    cv::Mat highboost_kernel = (cv::Mat_<float>(3, 3) << -1, -1, -1,
                                -1, A + 8, -1,
                                -1, -1, -1);
    cv::filter2D(blurred, highboost_sharpened, -1, highboost_kernel);

    double psnr_laplace = cv::PSNR(image, laplace_sharpened);
    double psnr_log = cv::PSNR(image, log_sharpened);
    double psnr_highboost = cv::PSNR(image, highboost_sharpened);

    std::cout << "=== Question 4: PSNR Degerleri ===" << std::endl;
    std::cout << "Orijinal vs Laplace Keskinlestirme : " << psnr_laplace << " dB" << std::endl;
    std::cout << "Orijinal vs LoG Keskinlestirme     : " << psnr_log << " dB" << std::endl;
    std::cout << "Orijinal vs High Boost Filtresi    : " << psnr_highboost << " dB" << std::endl;
    std::cout << "==================================" << std::endl;

    currSharpening = (currSharpening > 3) ? 0 : currSharpening + 1;

    switch (currSharpening)
    {
    case 0:
        std::cout << "Blurred" << std::endl;
        return blurred;
    case 1:
        std::cout << "Laplace Sharpened" << std::endl;
        return laplace_sharpened;
    case 2:
        std::cout << "Log Sharpened" << std::endl;
        return log_sharpened;
    case 3:
        std::cout << "HighBoost Sharpened" << std::endl;
        return highboost_sharpened;

    default:
        break;
    }

    return highboost_sharpened;
}

cv::Mat EffectManager::Helper_Question5(const cv::Mat &image)
{
    if (image.empty())
        return image;

    cv::Mat gray;
    if (image.channels() == 3)
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    else
        gray = image.clone();

    cv::Moments moments = cv::moments(gray, false);

    double huMoments[7];
    cv::HuMoments(moments, huMoments);

    std::cout << "=== HU MOMENTLERI ===" << std::endl;
    for (int i = 0; i < 7; i++)
    {
        double huLog = -1 * std::copysign(1.0, huMoments[i]) * std::log10(std::abs(huMoments[i]));
        
        std::cout << "Hu[" << i + 1 << "] : " 
                  << huLog << " (Log Ölçekli)  |  " 
                  << huMoments[i] << " (Ham Deger)" << std::endl;
    }
    std::cout << "=====================" << std::endl;

    return gray; 
}

// Algo analizi
// 15 ödev 85 sınav - 4 soru
// en iyi en kötü (rekürsif olmayan)
// sayıları algoritmaya göre sıralıyın
// rekürsif bir kodu analiz et T(n)
// verdiğimiz kodu istediğin yöntemi kullanarak çöz
// en son counting sort kısmından bir soru çıkabilir