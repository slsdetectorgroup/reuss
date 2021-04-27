#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


int main(){

    cv::Mat raw_data(512,1024, CV_16U);

    std::string fname("/home/l_frojdh/data/emtest/file_0.bin");
    std::ifstream ifs(fname, std::ios::binary);
    ifs.read((char*)raw_data.ptr(), 512*1024*2);


    for (int i = 0; i!=raw_data.total(); ++i){
        if (auto& val = raw_data.at<uint16_t>(i); val>4000){
            val = 4000;
        }else if(val <1000){
            val = 1000;
        }
    }

    cv::Mat norm;
    // cv::normalize(raw_data, norm, 0., 255., cv::NORM_MINMAX, CV_8U);
    cv::normalize(raw_data, norm, 255., 0., cv::NORM_MINMAX, CV_8U);
    cv::Mat color;
    cv::applyColorMap(norm, color, cv::COLORMAP_JET);
    // cv::cvtColor(color, color, cv::COLOR_RGB2RGBA);
    cv::imshow("Display window", color);
    // cv::imshow("Display window", norm);


    int k = cv::waitKey(0); // Wait for a keystroke in the window

}