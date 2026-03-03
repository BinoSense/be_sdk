#ifndef BIONIC_EYES_CPP_WRAPPER_H
#define BIONIC_EYES_CPP_WRAPPER_H



#include <opencv2/opencv.hpp>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cstring>

#include "bionic_eyes_c.h"
#include "mat_converter.hpp"

namespace bionic_eyes {

	struct BE_CalibData_w
	{
		std::vector<double> K[2];//注意0是右眼，1是左眼，和常识是反的
		std::vector<double> D[2];//注意0是右眼，1是左眼，和常识是反的
		std::vector<double> R[7];//R[6]是0位旋转矩阵
		std::vector<double> t[7];//t[6]是0位平移矩阵
		std::string motors[3];
		std::string eyes[2];
    };

// 从C数组（TLV格式）反序列化为结构体
BE_CalibData_w deserialize_from_c_array(const uint8_t* data, size_t data_size) {
    BE_CalibData_w obj = {};
    size_t offset = 0;
    
    while(offset < data_size) {
        if(offset + sizeof(uint32_t) * 2 > data_size) break;
        
        uint32_t tag, length;
        memcpy(&tag, data + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(&length, data + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        
        if(offset + length > data_size) break;
        
        switch(tag) {
            case TAG_K_0:
                obj.K[0].resize(length / sizeof(double));
                memcpy(obj.K[0].data(), data + offset, length);
                break;
            case TAG_K_1:
                obj.K[1].resize(length / sizeof(double));
                memcpy(obj.K[1].data(), data + offset, length);
                break;
            case TAG_D_0:
                obj.D[0].resize(length / sizeof(double));
                memcpy(obj.D[0].data(), data + offset, length);
                break;
            case TAG_D_1:
                obj.D[1].resize(length / sizeof(double));
                memcpy(obj.D[1].data(), data + offset, length);
                break;
            case TAG_R_0:
                obj.R[0].resize(length / sizeof(double));
                memcpy(obj.R[0].data(), data + offset, length);
                break;
            case TAG_R_1:
                obj.R[1].resize(length / sizeof(double));
                memcpy(obj.R[1].data(), data + offset, length);
                break;
            case TAG_R_2:
                obj.R[2].resize(length / sizeof(double));
                memcpy(obj.R[2].data(), data + offset, length);
                break;
            case TAG_R_3:
                obj.R[3].resize(length / sizeof(double));
                memcpy(obj.R[3].data(), data + offset, length);
                break;
            case TAG_R_4:
                obj.R[4].resize(length / sizeof(double));
                memcpy(obj.R[4].data(), data + offset, length);
                break;
            case TAG_R_5:
                obj.R[5].resize(length / sizeof(double));
                memcpy(obj.R[5].data(), data + offset, length);
                break;
            case TAG_R_6:
                obj.R[6].resize(length / sizeof(double));
                memcpy(obj.R[6].data(), data + offset, length);
                break;
            case TAG_T_0:
                obj.t[0].resize(length / sizeof(double));
                memcpy(obj.t[0].data(), data + offset, length);
                break;
            case TAG_T_1:
                obj.t[1].resize(length / sizeof(double));
                memcpy(obj.t[1].data(), data + offset, length);
                break;
            case TAG_T_2:
                obj.t[2].resize(length / sizeof(double));
                memcpy(obj.t[2].data(), data + offset, length);
                break;
            case TAG_T_3:
                obj.t[3].resize(length / sizeof(double));
                memcpy(obj.t[3].data(), data + offset, length);
                break;
            case TAG_T_4:
                obj.t[4].resize(length / sizeof(double));
                memcpy(obj.t[4].data(), data + offset, length);
                break;
            case TAG_T_5:
                obj.t[5].resize(length / sizeof(double));
                memcpy(obj.t[5].data(), data + offset, length);
                break;
            case TAG_T_6:
                obj.t[6].resize(length / sizeof(double));
                memcpy(obj.t[6].data(), data + offset, length);
                break;
            case TAG_MOTORS_0:
                obj.motors[0] = std::string(reinterpret_cast<const char*>(data + offset), length);
                break;
            case TAG_MOTORS_1:
                obj.motors[1] = std::string(reinterpret_cast<const char*>(data + offset), length);
                break;
            case TAG_MOTORS_2:
                obj.motors[2] = std::string(reinterpret_cast<const char*>(data + offset), length);
                break;
            case TAG_EYES_0:
                obj.eyes[0] = std::string(reinterpret_cast<const char*>(data + offset), length);
                break;
            case TAG_EYES_1:
                obj.eyes[1] = std::string(reinterpret_cast<const char*>(data + offset), length);
                break;
        }
        
        offset += length;
    }
    
    return obj;
}

namespace detail {
    // 将std::vector<bool>转换为bool数组
    template<size_t N>
    static void convertVectorToArray(const std::vector<bool>& vec, bool (&arr)[N]) 
    {
        const size_t count = std::min(vec.size(), N);
        for (size_t i = 0; i < count; ++i) 
        {
            arr[i] = vec[i];
        }
        // 填充剩余部分
        for (size_t i = count; i < N; ++i) 
        {
            arr[i] = false;
        }
    }
}

class BionicEyesWrapper 
{
private:
    BE_Handle handle_;
    
public:
    BionicEyesWrapper(bool openNetworkService = true, int dataDefaultFrameRate = 25, void* logger_ptr = nullptr)
        : handle_(nullptr) 
    {
        handle_ = BE_create_local(openNetworkService, dataDefaultFrameRate, logger_ptr);
    }
    
    BionicEyesWrapper(BE_Connect_Type type,
                     BE_Connect_DataServerType dataServerType = enumDeviceServer_Only,
                     BE_Data_TransmissionType dataTransmissionType = enumDataTransmission_ASPAP,
                     void* logger_ptr = nullptr)
        : handle_(nullptr) 
    {
        handle_ = BE_create1(type, dataServerType, dataTransmissionType, logger_ptr);
    }
    
    BionicEyesWrapper(const std::string& ipAddr,
                     BE_Connect_Type type = enumdisConnect,
                     BE_Connect_DataServerType dataServerType = enumDeviceServer_Only,
                     BE_Data_TransmissionType dataTransmissionType = enumDataTransmission_ASPAP,
                     void* logger_ptr = nullptr)
        : handle_(nullptr) 
    {
        handle_ = BE_create_ip1(ipAddr.c_str(), type, dataServerType, dataTransmissionType, logger_ptr);
    }
#ifdef EAGLE
    BionicEyesWrapper(const std::string& SrvipAddr,
                     const std::string& devName,
                     void* logger_ptr = nullptr,
                     int mediaPort = 6674,
                     int controlPort = 6676)
        : handle_(nullptr) 
    {
        handle_ = BE_create_server1(SrvipAddr.c_str(), devName.c_str(), mediaPort, controlPort, logger_ptr);
    }
#endif
    ~BionicEyesWrapper() 
     {
        if (handle_) 
        {
            BE_destroy(handle_);
        }
    }
    
    // 禁止拷贝
    BionicEyesWrapper(const BionicEyesWrapper&) = delete;
    BionicEyesWrapper& operator=(const BionicEyesWrapper&) = delete;
    
    // 允许移动
    BionicEyesWrapper(BionicEyesWrapper&& other) noexcept : handle_(other.handle_) 
    {
        other.handle_ = nullptr;
    }
    
    BionicEyesWrapper& operator=(BionicEyesWrapper&& other) noexcept 
    {
        if (this != &other) 
        {
            if (handle_) 
            {
                BE_destroy(handle_);
            }
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }
    
    // 设备信息
    uint32_t getBeDevice_Ip() const 
    {
        return BE_getBeDevice_Ip(handle_);
    }
    
    BE_Device_Type getBeDeviceType() const 
    {
        return BE_getBeDeviceType(handle_);
    }
    
    std::string getBeDevice_Ip_str() const 
    {
        const char* ip = BE_getBeDevice_Ip_str(handle_);
        return ip ? std::string(ip) : std::string();
    }
    
    // 图像设置
    void setImageResolution(int width = 1920, int height = 1080) 
    {
        BE_Size size = {width, height};
        BE_setImageResolution(handle_, size);
    }
    
    void setImageResolution_Transfer(int width = 960, int height = 540) 
    {
        BE_Size size = {width, height};
        BE_setImageResolution_Transfer(handle_, size);
    }

    void setImageColor(BE_ImageColorType type = enumColor) 
    {
        BE_setImageColor(handle_, type);
    }
    
    void setImageColor_Transfer(BE_ImageColorType type = enumColor) 
    {
        BE_setImageColor_Transfer(handle_, type);
    }
    
    void setDataRate_Transfer(float rate = 25.0f) 
    {
        BE_setDataRate_Transfer(handle_, rate);
    }
    
    std::pair<int, int> getOriginImageResolution() const 
    {
        BE_Size size = BE_getOriginImageResolution(handle_);
        return {size.width, size.height};
    }
    
    float getMaxImageFrameRate() const 
    {
        return BE_getMaxImageFrameRate(handle_);
    }
    
    void setImageCompressionQuality(int imgQual = 95) 
    {
        BE_setImageCompressionQuality(handle_, imgQual);
    }
    
    // 数据获取
    bool isBeDataReady() const 
    {
        return BE_isBeDataReady(handle_);
    }
    
    bool getBeData(BE_GeneralData &data) const 
    {
        BE_getBeData(handle_, &data);
        return true;
    }
    
    bool getBeData(BE_GeneralData& data, uint32_t id) const 
    {
        return BE_getBeDataById(handle_, &data, id);
    }
    
    BE_CalibData_w  getCalibrationInfo() const 
    {
        size_t nOutSize = 0;
        uint8_t* info = BE_getCalibrationInfo(handle_, &nOutSize);
        if (!info) return BE_CalibData_w();
 
        BE_CalibData_w tmp = deserialize_from_c_array(info, nOutSize);
        delete [] info;
        return tmp;
    }
    
   
    // 电机限制
    std::pair<float, float> getUpDownLimit(BE_MotorType type) const 
    {
        float up = 0, down = 0;
        BE_getUpDownLimit(handle_, type, &up, &down);
        return {up, down};
    }
    
    std::pair<float, float> getUpDownLimit_Neck(BE_MotorType_Neck type) const 
    {
        float up = 0, down = 0;
        BE_getUpDownLimit_Neck(handle_, type, &up, &down);
        return {up, down};
    }
    
    // 电机控制
    void setAbsoluteSpeed(bool* activeFlag, float* speed) 
    {
        BE_setAbsoluteSpeed(handle_, activeFlag, speed);
    }
	
    void setAbsoluteSpeed(const std::vector<bool>& activeFlag, const std::vector<float>& speed) 
    {
        if (activeFlag.size()  >=  6 && speed.size()  >=  6) 
        {
            bool active_flags[6];
            detail::convertVectorToArray(activeFlag, active_flags);
            BE_setAbsoluteSpeed(handle_, active_flags, const_cast<float*>(speed.data()));
        }
    }
	
	void setAbsolutePosition(BE_MotorType type, float* angle,
						BE_MovePatternType moveType = enumMovePattern_Saccade,
						BE_MoveBaseType movebaseType = enumMoveBase_Independent) 
    {
		BE_setAbsolutePosition(handle_, type, angle, moveType, movebaseType);
    }
    
    void setAbsolutePosition(BE_MotorType type, const std::vector<float>& angle,
                            BE_MovePatternType moveType = enumMovePattern_Saccade,
                            BE_MoveBaseType movebaseType = enumMoveBase_Independent) 
    {
        if (angle.size() >= 6) 
        {
            BE_setAbsolutePosition(handle_, type, const_cast<float*>(angle.data()), moveType, movebaseType);
        }
    }
    
	void setRelativePosition(BE_MotorType type, float* angle,  BE_MovePatternType moveType = enumMovePattern_Saccade,  BE_MoveBaseType movebaseType = enumMoveBase_Independent) 
    {
         BE_setRelativePosition(handle_, type, angle, moveType, movebaseType);
    }
	
    void setRelativePosition(BE_MotorType type, const std::vector<float>& angle,  BE_MovePatternType moveType = enumMovePattern_Saccade,  BE_MoveBaseType movebaseType = enumMoveBase_Independent) 
    {
        if (angle.size() >= 6) 
        {
            BE_setRelativePosition(handle_, type, const_cast<float*>(angle.data()), moveType, movebaseType);
        }
    }
    
    void setAbsolutePosition(bool* activeFlag, float* angle,
                              BE_MovePatternType moveType = enumMovePattern_Saccade,
                              BE_MoveBaseType movebaseType = enumMoveBase_Independent) 
    {
		BE_setAbsolutePositionEx(handle_, 
								activeFlag,
								angle,
								moveType, movebaseType);
    }
	
    void setAbsolutePosition(const std::vector<bool>& activeFlag, const std::vector<float>& angle,
                              BE_MovePatternType moveType = enumMovePattern_Saccade,
                              BE_MoveBaseType movebaseType = enumMoveBase_Independent) 
    {
        if (activeFlag.size() >= 6 && angle.size() >= 6) 
        {
            bool active_flags[6];
            detail::convertVectorToArray(activeFlag, active_flags);
            BE_setAbsolutePositionEx(handle_, 
                                    active_flags,
                                    const_cast<float*>(angle.data()),
                                    moveType, movebaseType);
        }
    }
    
	void setRelativePosition(bool* activeFlag, float* angle,
                              BE_MovePatternType moveType = enumMovePattern_Saccade,
                              BE_MoveBaseType movebaseType = enumMoveBase_Independent) 
    {
		BE_setRelativePositionEx(handle_, 
								activeFlag,
								angle,
								moveType, movebaseType);
    }
	
    void setRelativePosition(const std::vector<bool>& activeFlag, const std::vector<float>& angle,
                              BE_MovePatternType moveType = enumMovePattern_Saccade,
                              BE_MoveBaseType movebaseType = enumMoveBase_Independent) 
    {
        if (activeFlag.size() >= 6 && angle.size() >= 6) 
        {
            bool active_flags[6];
            detail::convertVectorToArray(activeFlag, active_flags);
            BE_setRelativePositionEx(handle_, 
                                    active_flags,
                                    const_cast<float*>(angle.data()),
                                    moveType, movebaseType);
        }
    }
    
    // 空间位置
    void setSpaceAbsPosition(float* motorAngle,
                            float* motorMoveAngle,
                            float* imuAngle,
                            BE_MovePatternType moveType = enumMovePattern_Saccade,
                            BE_MoveBaseType movebaseType = enumMoveBase_Independent) 
    {
		BE_setSpaceAbsPosition(handle_, 
							  motorAngle,
							  motorMoveAngle,
							  imuAngle,
							  moveType, movebaseType);
    }
    void setSpaceAbsPosition(const std::vector<float>& motorAngle,
                            const std::vector<float>& motorMoveAngle,
                            const std::vector<float>& imuAngle,
                            BE_MovePatternType moveType = enumMovePattern_Saccade,
                            BE_MoveBaseType movebaseType = enumMoveBase_Independent) 
    {
        if (motorAngle.size() >= 6 && motorMoveAngle.size() >= 6 && imuAngle.size() >= 3) {
            BE_setSpaceAbsPosition(handle_, 
                                  const_cast<float*>(motorAngle.data()),
                                  const_cast<float*>(motorMoveAngle.data()),
                                  const_cast<float*>(imuAngle.data()),
                                  moveType, movebaseType);
        }
    }
    
    // 颈部控制
    void setAbsolutePosition_Neck(bool* activeFlag, float* angle,
                                 BE_MovePatternType moveType = enumMovePattern_Saccade) 
    {
		BE_setAbsolutePosition_Neck(handle_, 
								   activeFlag,
								   angle, 
								   moveType);
    }
    void setAbsolutePosition_Neck(const std::vector<bool>& activeFlag, const std::vector<float>& angle,
                                 BE_MovePatternType moveType = enumMovePattern_Saccade) 
    {
        if (activeFlag.size() >= 3 && angle.size() >= 3) {
            bool active_flags[3];
            detail::convertVectorToArray(activeFlag, active_flags);
            BE_setAbsolutePosition_Neck(handle_, 
                                       active_flags,
                                       const_cast<float*>(angle.data()), 
                                       moveType);
        }
    }
    
    void setRelativePosition_Neck(bool* activeFlag, float* angle,
                                 BE_MovePatternType moveType = enumMovePattern_Saccade) 
    {
		BE_setRelativePosition_Neck(handle_, 
								   activeFlag,
								   angle, 
								   moveType);
    }
    void setRelativePosition_Neck(const std::vector<bool>& activeFlag, const std::vector<float>& angle,
                                 BE_MovePatternType moveType = enumMovePattern_Saccade) 
    {
        if (activeFlag.size() >= 3 && angle.size() >= 3) 
        {
            bool active_flags[3];
            detail::convertVectorToArray(activeFlag, active_flags);
            BE_setRelativePosition_Neck(handle_, 
                                       active_flags,
                                       const_cast<float*>(angle.data()), 
                                       moveType);
        }
    }
    
    // 联合控制
    void setUnionAbsolutePosition_NeckEye(bool* activeFlag,
                                         float* eyeAngle,
                                         float* neckAngle,
                                         BE_MovePatternType moveType = enumMovePattern_Saccade) 
    {
		BE_setUnionAbsolutePosition_NeckEye(handle_, 
										   activeFlag,
										   eyeAngle,
										   neckAngle,
										   moveType);
    }
    void setUnionAbsolutePosition_NeckEye(const std::vector<bool>& activeFlag,
                                         const std::vector<float>& eyeAngle,
                                         const std::vector<float>& neckAngle,
                                         BE_MovePatternType moveType = enumMovePattern_Saccade) 
    {
        if (activeFlag.size() >= 3 && eyeAngle.size() >= 6 && neckAngle.size() >= 3) 
        {
            bool active_flags[3];
            detail::convertVectorToArray(activeFlag, active_flags);
            BE_setUnionAbsolutePosition_NeckEye(handle_, 
                                               active_flags,
                                               const_cast<float*>(eyeAngle.data()),
                                               const_cast<float*>(neckAngle.data()),
                                               moveType);
        }
    }
	
    void setUnionRelativePosition_NeckEye(bool* activeFlag,
                                         float* eyeAngle,
                                         float* neckAngle,
                                         BE_MovePatternType moveType = enumMovePattern_Saccade) 
    {
		BE_setUnionRelativePosition_NeckEye(handle_, 
										   activeFlag,
										   eyeAngle,
										   neckAngle,
										   moveType);
    }
    void setUnionRelativePosition_NeckEye(const std::vector<bool>& activeFlag,
                                         const std::vector<float>& eyeAngle,
                                         const std::vector<float>& neckAngle,
                                         BE_MovePatternType moveType = enumMovePattern_Saccade) 
    {
        if (activeFlag.size() >= 3 && eyeAngle.size() >= 6 && neckAngle.size() >= 3) 
        {
            bool active_flags[3];
            detail::convertVectorToArray(activeFlag, active_flags);
            BE_setUnionRelativePosition_NeckEye(handle_, 
                                               active_flags,
                                               const_cast<float*>(eyeAngle.data()),
                                               const_cast<float*>(neckAngle.data()),
                                               moveType);
        }
    }
    
    // 初始位置
    void setNowPositionAsInitPos() 
    {
        BE_setNowPositionAsInitPos(handle_);
    }
    
    void goInitPosition(BE_MotorType type) 
    {
        BE_goInitPosition(handle_, type);
    }
	
    std::vector<float> getInitPosition(BE_MotorType type) const 
    {
        std::vector<float> angle(6, 0.0f);
        BE_getInitPosition(handle_, type, angle.data());
        return angle;
    }
    
    void goInitPosition_Neck(BE_MotorType_Neck type) {
        BE_goInitPosition_Neck(handle_, type);
    }
    
    std::vector<float> getInitPosition_Neck(BE_MotorType_Neck type) const {
        std::vector<float> angle(3, 0.0f);
        BE_getInitPosition_Neck(handle_, type, angle.data());
        return angle;
    }
    
    // 设备状态
    bool haveNeckLinked() const 
    {
        return BE_haveNeckLinked(handle_);
    }
    
    void onoff_VOR(bool vor_eye = true, bool vor_neck = false) 
    {
        BE_onoff_VOR(handle_, vor_eye, vor_neck);
    }
    
    void onoff_SV(bool onoff) 
    {
        BE_onoff_SV(handle_, onoff);
    }
    
    void setSvModel(BE_SVType type = enumGlobalSV,
                   const cv::Rect& rect = cv::Rect(0, 0, 1920, 1080),
                   const cv::Size& size = cv::Size(1920, 1080)) 
    {
        BE_Rect be_rect = {rect.x, rect.y, rect.width, rect.height};
        BE_Size be_size = {size.width, size.height};
        BE_setSvModel(handle_, type, be_rect, be_size);
    }
    
    void setSyncSignalSource(bool flag = true) {
        BE_setSyncSignalSource(handle_, flag);
    }
    
    // 相机控制
    std::pair<float, float> getCameraExposureTimeRange() const 
    {
        float up = 0, down = 0;
        BE_getCameraExposureTimeRange(handle_, &up, &down);
        return {up, down};
    }
    
    float getCameraExposureTime(int type) const 
    {
        return BE_getCameraExposureTime(handle_, type);
    }
    
    void setCameraExposure(int type, bool autoExposure, float exposureTime = 0) 
    {
        BE_setCameraExposure(handle_, type, autoExposure, exposureTime);
    }
    
    std::pair<int, int> getCameraWhiteBalanceTemperatureRange() const {
        int up = 0, down = 0;
        BE_getCameraWhiteBalanceTemperatureRange(handle_, &up, &down);
        return {up, down};
    }
    
    float getCameraWhiteBalanceTemperature(int type) const {
        return BE_getCameraWhiteBalanceTemperature(handle_, type);
    }
    
    void setCameraWhiteBalanceTemperature(int type, bool autoWhitebalance, 
                                         float WhiteBalanceTemperature = 6600) 
    {
        BE_setCameraWhiteBalanceTemperature(handle_, type, autoWhitebalance, 
                                           WhiteBalanceTemperature);
    }
    
    std::pair<float, float> getCameraExposureAutoReferenceRange() const 
    {
        float up = 0, down = 0;
        BE_getCameraExposureAutoReferenceRange(handle_, &up, &down);
        return {up, down};
    }
    
    void setCameraExposureAutoReference(int type, int value = 70) 
    {
        BE_setCameraExposureAutoReference(handle_, type, value);
    }
    
    std::pair<float, float> getCameraGainRange() const 
    {
        float up = 0, down = 0;
        BE_getCameraGainRange(handle_, &up, &down);
        return {up, down};
    }
    
    void setCameraGain(int type, bool autoGain = true, int value = 370) 
    {
        BE_setCameraGain(handle_, type, autoGain, value);
    }
    
    // 数据保存
    void saveBeData(bool startStop = true, const std::string& folderPath = "./Be_Io_Path/fastSave") 
    {
        BE_saveBeData(handle_, startStop, folderPath.c_str());
    }
    
    bool snapBeData(const std::string& description = "", const std::string& folderPath = "./Be_Io_Path/snapSave") 
    {
        return BE_snapBeData(handle_, description.c_str(), folderPath.c_str());
    }
    
    void stopSnapBeData() 
    {
        BE_stopSnapBeData(handle_);
    }
    
    bool recordBeData(BE_GeneralData& data, const std::string& description = "",
                     const std::string& folderPath = "Be_Io_Path/recordSave") 
    {
        return BE_recordBeData(handle_, &data, description.c_str(), folderPath.c_str());
    }
    
    void stopRecordBeData() 
    {
        BE_stopRecordBeData(handle_);
    }
    
    // 其他功能
    void triggerDataTransmission() 
    {
        BE_triggerDataTransmission(handle_);
    }
    
    void setBeDataRate(int rate = 25) 
    {
        BE_setBeDataRate(handle_, rate);
    }
    
    void setSyncCameraDelayTime(int timeDelay = 0) 
    {
        BE_setSyncCameraDelayTime(handle_, timeDelay);
    }
    
    std::pair<BE_Image, BE_Image> infraImageTrans(BE_GeneralData& data) 
    {
        BE_Image infraImage = {0};
        BE_Image tempImage = {0};
        BE_infraImageTrans(handle_, &data, &infraImage, &tempImage);
        return {infraImage, tempImage};
    }

    // 红外图像处理（返回cv::Mat版本）
    std::pair<cv::Mat, cv::Mat> infraImageTrans2(BE_GeneralData& data) 
    {
        BE_Image infraImageStruct = {0};
        BE_Image tempImageStruct = {0};
        
        bool success = BE_infraImageTrans(handle_, &data, &infraImageStruct, &tempImageStruct);
        
        if (!success) 
        {
            return {cv::Mat(), cv::Mat()};
        }
        
        // 转换为cv::Mat
        cv::Mat infraMat = ImageConverter::toMat(&infraImageStruct, true);
        cv::Mat tempMat  = ImageConverter::toMat(&tempImageStruct, true);
        
        // 释放BE_Image内存
        BE_freeImage(&infraImageStruct);
        BE_freeImage(&tempImageStruct);
        
        return {infraMat, tempMat};
    }

    //单个像素16位 转温度值
    float infraTempCalc(uint32_t value) 
    {
        return BE_infraTempCalc(handle_, value);
    }
    
    void onOffCameraImage(bool* flag, bool onlyNetworkType = true) 
    {
        BE_onOffCameraImage(handle_, flag, onlyNetworkType);
    }
    void onOffCameraImage(const std::vector<bool>& flag, bool onlyNetworkType = true) 
    {
        if (flag.size() > 0) 
        {
            bool active_flags[4];
            detail::convertVectorToArray(flag, active_flags);
            BE_onOffCameraImage(handle_, active_flags, onlyNetworkType);
        }
    }
    
 
    
    void setZoomFocalLength(uint8_t focalLength = 1) 
    {
        BE_setZoomFocalLength(handle_, focalLength);
    }
    
    void setWipers(uint8_t value = 0) 
    {
        BE_setWipers(handle_, value);
    }
    
    void setExtraModuleFunction(BE_ExtraModuleType type, int para0 = 0,  int para1 = 0, int para2 = 0) 
    {
        BE_setExtraModuleFunction(handle_, type, para0, para1, para2);
    }
    
    void setMotorPositionSpeed(int saccadeSpeed, int pursuitSpeed) 
    {
        BE_setMotorPositionSpeed(handle_, saccadeSpeed, pursuitSpeed);
    }
    
    // 操作符重载，检查有效性
    explicit operator bool() const 
    {
        return handle_ != nullptr;
    }
    
    // 获取原始句柄
    BE_Handle getHandle() const 
    {
        return handle_;
    }
    
private:
    // 辅助函数：从BE_GeneralData中提取图像（需要根据实际数据结构实现）
    cv::Mat getImageFromData(const BE_GeneralData& data, bool copy_data = true) {
        // 这里需要根据实际的BE_GeneralData结构实现
        // 假设data中包含BE_Image指针
        // 示例代码：
        // if (data.image) {
        //     return ImageConverter::toMat(data.image, copy_data);
        // }
        return cv::Mat();
    }
};

// 方便的别名
using BE = BionicEyesWrapper;
using BEConverter = ImageConverter;

} // namespace bionic_eyes

#endif // BIONIC_EYES_CPP_WRAPPER_H
