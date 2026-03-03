#ifndef EVO_BEVIRTUALRECTIFYPIPELINE_HITNET_TRT_H
#define EVO_BEVIRTUALRECTIFYPIPELINE_HITNET_TRT_H

#include "evo_tinycgl_global_define.h"

#if defined(EVO_ENV_WINDOWS) && !defined(EVO_BEVIRTUALRECTIFYPIPELINE_HITNET_TRT_EXPORTS)
#define EVO_BEVIRTUALRECTIFYPIPELINE_HITNET_TRT_API EVO_WINDOWS_DLLIMPORT
#else
#define EVO_BEVIRTUALRECTIFYPIPELINE_HITNET_TRT_API EVO_API
#endif

namespace evo
{
    namespace depth
    {
        class BEVirtualRectifyPipeline_Hitnet_TrtVars;
        class EVO_BEVIRTUALRECTIFYPIPELINE_HITNET_TRT_API BEVirtualRectifyPipeline_Hitnet_Trt
        {
        public:
            BEVirtualRectifyPipeline_Hitnet_Trt();
            ~BEVirtualRectifyPipeline_Hitnet_Trt();

            //@{
            /**  @name 初始化&参数	 */
            //max_shift_px表示实际计算时图片左或右增加的最大像素数（即计算的最大宽为input_image_width + max_shift_px）
            void init(std::string model_path, int input_image_width, int input_image_height, INPUT_IMAGE_FORMAT input_image_format,
                int max_shift_px = 300);

            //min_distance、max_distance表示计算的距离范围（不在此范围内的会被过滤掉）
            void setDistanceRange(float min_distance = 200.0f, float max_distance = 5000.0f);

            //初始化之后，提前设置相机内参，避免每次重新去生成去畸变map
            void setCameraMatrix(std::vector<float> K1, std::vector<float> D1,
                std::vector<float> K2, std::vector<float> D2);
            void setCameraMatrix(std::vector<double> K1, std::vector<double> D1,
                std::vector<double> K2, std::vector<double> D2);

            //设置计算中SV开关
            void setSVValid(bool valid = false);

            //设置输入图像中值滤波的窗大小（奇数，1表示不做）
            //仅在CUDA>=12时生效
            void setMedianFilterSize(int size = 5);
            //@}


            //@{
            /**  @name 处理	 */
            //p_left、p_right为左右原图
            //lr_r、lr_t为左眼到右眼的旋转向量/矩阵、平移向量
            //l0_r、l0_t为左眼0位置到新位置的旋转向量/矩阵、平移向量（为空时得不到转回0位置的点云）
            //图片和参数都在内存里
            void process(unsigned char *p_left, unsigned char *p_right,
                std::vector<float> lr_r, std::vector<float> lr_t,
                std::vector<float> l0_r, std::vector<float> l0_t);
            //@}


            //@{
            /**  @name 获取结果	 */
            //转回0点的（始终固定0点初始方向），推荐直接使用（可以保持看到的点云不动）
            //因为有平移，所以转回的结果只有点云，没有图片
            //获取左图转回0点的xyz距离
            float* retrieveXYZZeroPoint(bool is_cuda_buffer);//3通道float
            //获取左图转回0点的z距离
            float* retrieveZZeroPoint(bool is_cuda_buffer);//1通道float
            //获取左图转回0点的点云
            float* retrievePointCloudZeroPoint(POINT_CLOUD_TYPE type, int& point_count, bool is_cuda_buffer);//4通道float
            //保存左图转回0点的PLY点云
            bool savePLYZeroPoint(const char *file_name, bool using_binary);

            //转回原角度的（始终固定左图原始方向），推荐直接使用
            //获取左图转回原角度的矫正后的图像
            unsigned char* retrieveRectifiedOriBGR(bool is_cuda_buffer);//3通道BGR，用于显示绘制等
            unsigned char* retrieveRectifiedOriRGBA(bool is_cuda_buffer); //4通道RGBA，用于点云绘制等
            //获取左图转回原角度的xyz距离
            float* retrieveXYZOri(bool is_cuda_buffer);//3通道float
            //获取左图转回原角度的z距离
            float* retrieveZOri(bool is_cuda_buffer);//1通道float
            //获取左图转回原角度的可视化的距离z彩图（始终固定，推荐使用）
            unsigned char* retrieveZOriColor(bool is_cuda_buffer);//4通道BGRA距离z彩图（颜色范围与min_distance、max_distance相关）
            //取左图转回原角度的点云
            float* retrievePointCloudOri(POINT_CLOUD_TYPE type, int& point_count, bool is_cuda_buffer);//4通道float
            //保存左图转回原角度的PLY点云
            bool savePLYOri(const char* file_name, bool using_binary);

            //矫正后的方向（与原始方向不同，会因为另一个相机的移动导致移动)，不推荐直接使用
            //获取矫正后的图像
            unsigned char* retrieveRectifiedBGR(bool is_left, bool is_cuda_buffer);//3通道BGR，用于显示绘制等
            unsigned char* retrieveRectifiedRGBA(bool is_left, bool is_cuda_buffer); //4通道RGBA，用于点云绘制等
            //获取左图视差图（在虚平行的情况下，视差图有动态变化负值，没有直接使用的价值，或者说很难用）
            float* retrieveDisparity(bool is_cuda_buffer);//1通道float
            //获取左图xyz距离
            float* retrieveXYZ(bool is_cuda_buffer);//3通道float
            //获取左图z距离
            float* retrieveZ(bool is_cuda_buffer);//1通道float
            //获取可视化的左图距离z彩图
            unsigned char* retrieveZColor(bool is_cuda_buffer);//4通道BGRA距离z彩图（颜色范围与min_distance、max_distance相关）
            //获取左图点云
            float* retrievePointCloud(POINT_CLOUD_TYPE type, int& point_count, bool is_cuda_buffer);//4通道float
            //保存左图PLY点云
            bool savePLY(const char* file_name, bool using_binary);
            //@}

        private:
            BEVirtualRectifyPipeline_Hitnet_TrtVars* vars;
        };
    }
}

#endif
