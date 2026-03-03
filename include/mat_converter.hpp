#include "bionic_eyes_c_type.h"
#include <opencv2/opencv.hpp>

// 图像转换工具类
class ImageConverter {
public:
    /**
     * @brief 将BE_Image转换为cv::Mat
     * @param image BE_Image结构体指针
     * @param copy_data 是否复制数据（true:深拷贝，false:浅拷贝）
     * @return 转换后的cv::Mat
     */
    static cv::Mat toMat(const BE_Image* image, bool copy_data = true) 
    {
        if (!image || !image->data || image->width <= 0 || image->height <= 0) 
        {
            return cv::Mat();
        }
        
        // 确定OpenCV数据类型
        int cv_type = CV_8UC1; // 默认为8位单通道
        switch (image->channels) 
        {
            case 1:
                cv_type = CV_8UC1;
                break;
            case 3:
                cv_type = CV_8UC3;
                break;
            case 4:
                cv_type = CV_8UC4;
                break;
            default:
                // 未知通道数，创建单通道
                cv_type = CV_8UC1;
                break;
        }
        
        if (copy_data) 
        {
            // 深拷贝
            cv::Mat mat(image->height, image->width, cv_type);
            size_t data_size = image->height * image->width * image->channels;
            if (data_size <= image->data_size) 
            {
                std::memcpy(mat.data, image->data, data_size);
            } 
            else 
            {
                std::memcpy(mat.data, image->data, image->data_size);
            }
            return mat;
        } 
        else 
        {
            // 浅拷贝（共享数据）
            // 注意：调用者需要确保BE_Image的生命周期长于cv::Mat
            return cv::Mat(image->height, image->width, cv_type, image->data);
        }
    }
    
    /**
     * @brief 将cv::Mat转换为BE_Image（深拷贝）
     * @param mat 输入cv::Mat
     * @return 新分配的BE_Image，需要调用者释放
     */
    static bool toBEImage(const cv::Mat& mat, BE_Image* image) 
    {
        if (mat.empty()) 
        {
            return false;
        }
        
        image->width     = mat.cols;
        image->height    = mat.rows;
        image->channels  = mat.channels();
        image->data_size = mat.total() * mat.elemSize();
        
        // 分配内存并复制数据
        if(!image->data)
            image->data = new uint8_t[image->data_size];
        std::memcpy(image->data, mat.data, image->data_size);
        
        return true;
    }
    /**
     * @brief 释放BE_Image
     * @param image 要释放的BE_Image指针
     */
    static void release(BE_Image* image) 
    {
        if (image) 
        {
            delete[] image->data;
        }
    }
    
    /**
     * @brief BE_Image智能指针类型
     */
    struct BEImageDeleter 
    {
        void operator()(BE_Image* img) const 
        {
            if (img) 
            {
                delete[] img->data;
                delete img;
            }
        }
    };
    
    using BEImagePtr = std::unique_ptr<BE_Image, BEImageDeleter>;
    
    /**
     * @brief 使用智能指针包装BE_Image
     * @param image BE_Image指针
     * @return 智能指针
     */
    static BEImagePtr wrap(BE_Image* image) {
        return BEImagePtr(image);
    }
};
