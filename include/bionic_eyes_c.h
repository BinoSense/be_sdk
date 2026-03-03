#ifndef BIONIC_EYES_C_H
#define BIONIC_EYES_C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bionic_eyes_c_type.h"
#include "bionic_eyes_export.h"



// 基本类型定义
typedef void* BE_Handle;
typedef void* BE_LoggerPtr;





// 创建和销毁函数
//本机调用，不能用于跨机器远程调用（仿生眼安装在调用机器上）
BE_API  BE_Handle BE_CALL BE_create_local(bool openNetworkService, int dataDefaultFrameRate, BE_LoggerPtr logger_ptr);

BE_API BE_Handle BE_CALL BE_create(BE_Connect_Type type);
//logger_ptr可填NULL
BE_API BE_Handle BE_CALL BE_create1(BE_Connect_Type type,
                         BE_Connect_DataServerType dataServerType,
                         BE_Data_TransmissionType dataTransmissionType,
                         BE_LoggerPtr logger_ptr);

//
BE_API BE_Handle BE_CALL BE_create_ip(const char* ipAddr,  BE_Connect_Type type);

//logger_ptr可填NULL
BE_API BE_Handle BE_CALL BE_create_ip1(const char* ipAddr,
                       BE_Connect_Type type,
                       BE_Connect_DataServerType dataServerType,
                       BE_Data_TransmissionType dataTransmissionType,
                       BE_LoggerPtr logger_ptr);
#ifdef EAGLE
//鹰眼专用
BE_API BE_Handle BE_CALL BE_create_server( const char* SrvipAddr,const char* devName);

// SrvipAddr 指定服务器的ip地址，
// devName 设备序列号， 
// mediaPort 媒体端口号 
// controlPort 控制端口号，  
// logger_ptr可填NULL
BE_API BE_Handle BE_CALL BE_create_server1(const char* SrvipAddr,
                          const char* devName,
                          int mediaPort,
                          int controlPort,
                          BE_LoggerPtr logger_ptr
                          );
#endif

BE_API void BE_CALL BE_destroy(BE_Handle handle);

// 获取设备信息函数
BE_API uint32_t BE_CALL       BE_getBeDevice_Ip(BE_Handle handle);
BE_API BE_Device_Type BE_CALL BE_getBeDeviceType(BE_Handle handle);

BE_API const char* BE_CALL    BE_getBeDevice_Ip_str(BE_Handle handle);

// 图像设置函数
BE_API void  BE_CALL   BE_setImageResolution(BE_Handle handle, BE_Size size);
BE_API void  BE_CALL   BE_setImageResolution_Transfer(BE_Handle handle, BE_Size size);
BE_API void  BE_CALL   BE_setImageColor(BE_Handle handle, BE_ImageColorType type);
BE_API void  BE_CALL   BE_setImageColor_Transfer(BE_Handle handle, BE_ImageColorType type);
BE_API void  BE_CALL   BE_setDataRate_Transfer(BE_Handle handle, float rate);
BE_API BE_Size BE_CALL BE_getOriginImageResolution(BE_Handle handle);
BE_API float BE_CALL   BE_getMaxImageFrameRate(BE_Handle handle);
BE_API void  BE_CALL   BE_setImageCompressionQuality(BE_Handle handle, int imgQual);

// 数据获取函数
BE_API bool  BE_CALL    BE_isBeDataReady(BE_Handle handle);
BE_API void  BE_CALL    BE_getBeData(BE_Handle handle, BE_GeneralData* data);
BE_API bool  BE_CALL    BE_getBeDataById(BE_Handle handle, BE_GeneralData* data, uint32_t id);
BE_API uint8_t* BE_CALL BE_getCalibrationInfo(BE_Handle handle, size_t * tOutSize);

// 电机限制的阈值获取
BE_API bool BE_getUpDownLimit(BE_Handle handle, BE_MotorType type, float* up, float* down);
BE_API bool BE_getUpDownLimit_Neck(BE_Handle handle, BE_MotorType_Neck type, float* up, float* down);

// 电机控制函数
BE_API void BE_CALL BE_setAbsoluteSpeed(BE_Handle handle, bool activeFlag[6], float speed[6]);
BE_API void BE_CALL BE_setAbsolutePosition(BE_Handle handle, BE_MotorType type, float angle[6],
                           BE_MovePatternType moveType, BE_MoveBaseType movebaseType);
BE_API void BE_CALL BE_setRelativePosition(BE_Handle handle, BE_MotorType type, float angle[6],
                           BE_MovePatternType moveType, BE_MoveBaseType movebaseType);
BE_API void BE_CALL BE_setAbsolutePositionEx(BE_Handle handle, bool activeFlag[6], float angle[6],
                             BE_MovePatternType moveType, BE_MoveBaseType movebaseType);
BE_API void BE_CALL BE_setRelativePositionEx(BE_Handle handle, bool activeFlag[6], float angle[6],
                             BE_MovePatternType moveType, BE_MoveBaseType movebaseType);

// 空间位置函数
BE_API void BE_CALL BE_setSpaceAbsPosition(BE_Handle handle, float motorAngle[6], float motorMoveAngle[6],
                           float imuAngle[3], BE_MovePatternType moveType,
                           BE_MoveBaseType movebaseType);

// 颈部控制函数
BE_API void BE_CALL BE_setAbsolutePosition_Neck(BE_Handle handle, bool activeFlag[3], float angle[3],  BE_MovePatternType moveType);
BE_API void BE_CALL BE_setRelativePosition_Neck(BE_Handle handle, bool activeFlag[3], float angle[3],  BE_MovePatternType moveType);
BE_API void BE_CALL BE_setUnionAbsolutePosition_NeckEye(BE_Handle handle, bool activeFlag[3],
                                        float eyeAngle[6], float neckAngle[3],
                                        BE_MovePatternType moveType);
BE_API void BE_CALL BE_setUnionRelativePosition_NeckEye(BE_Handle handle, bool activeFlag[3],
                                        float eyeAngle[6], float neckAngle[3],
                                        BE_MovePatternType moveType);

// 初始位置函数
BE_API void BE_CALL BE_setNowPositionAsInitPos(BE_Handle handle);
BE_API void BE_CALL BE_goInitPosition(BE_Handle handle, BE_MotorType type);
BE_API void BE_CALL BE_getInitPosition(BE_Handle handle, BE_MotorType type, float angle[6]);
BE_API void BE_CALL BE_goInitPosition_Neck(BE_Handle handle, BE_MotorType_Neck type);
BE_API void BE_CALL BE_getInitPosition_Neck(BE_Handle handle, BE_MotorType_Neck type, float angle[3]);

// 设备状态函数
BE_API bool BE_CALL BE_haveNeckLinked(BE_Handle handle);


BE_API void BE_CALL BE_onoff_VOR(BE_Handle handle, bool vor_eye, bool vor_neck);
BE_API void BE_CALL BE_onoff_SV(BE_Handle handle, bool onoff);
BE_API void BE_CALL BE_setSvModel(BE_Handle handle, BE_SVType type, BE_Rect rect, BE_Size size);
BE_API void BE_CALL BE_setSyncSignalSource(BE_Handle handle, bool flag);

// 相机控制函数
BE_API void BE_CALL  BE_getCameraExposureTimeRange(BE_Handle handle, float* up, float* down);
BE_API float BE_CALL BE_getCameraExposureTime(BE_Handle handle, int type);
BE_API void BE_CALL  BE_setCameraExposure(BE_Handle handle, int type, bool autoExposure, float exposureTime);

BE_API void BE_CALL  BE_getCameraWhiteBalanceTemperatureRange(BE_Handle handle, int* up, int* down);
BE_API float BE_CALL BE_getCameraWhiteBalanceTemperature(BE_Handle handle, int type);
BE_API void BE_CALL  BE_setCameraWhiteBalanceTemperature(BE_Handle handle, int type, bool autoWhitebalance,
                                         float WhiteBalanceTemperature);

BE_API void BE_CALL BE_getCameraExposureAutoReferenceRange(BE_Handle handle, float* up, float* down);
BE_API void BE_CALL BE_setCameraExposureAutoReference(BE_Handle handle, int type, int value);

BE_API void BE_CALL BE_getCameraGainRange(BE_Handle handle, float* up, float* down);
BE_API void BE_CALL BE_setCameraGain(BE_Handle handle, int type, bool autoGain, int value);

// 数据保存函数
BE_API void BE_CALL BE_saveBeData(BE_Handle handle, bool startStop, const char* folderPath);
BE_API bool BE_CALL BE_snapBeData(BE_Handle handle, const char* description, const char* folderPath);
BE_API void BE_CALL BE_stopSnapBeData(BE_Handle handle);
BE_API bool BE_CALL BE_recordBeData(BE_Handle handle, BE_GeneralData* data, const char* description,  const char* folderPath);
BE_API void BE_CALL BE_stopRecordBeData(BE_Handle handle);

// 其他功能函数
BE_API void BE_CALL BE_triggerDataTransmission(BE_Handle handle);
BE_API void BE_CALL BE_setBeDataRate(BE_Handle handle, int rate);
BE_API void BE_CALL BE_setSyncCameraDelayTime(BE_Handle handle, int timeDelay);

//红外相机
BE_API bool BE_CALL  BE_infraImageTrans(BE_Handle handle, BE_GeneralData* data, BE_Image* infraImage,  BE_Image* tempImage);
BE_API float BE_CALL BE_infraTempCalc(BE_Handle handle, uint32_t value);

BE_API void BE_CALL BE_onOffCameraImage(BE_Handle handle, bool flag[], bool onlyNetworkType);


//变焦相机控制变焦倍数
BE_API void BE_CALL BE_setZoomFocalLength(BE_Handle handle, uint8_t focalLength);
//鹰眼 雨刮器
BE_API void BE_CALL BE_setWipers(BE_Handle handle, uint8_t value);


BE_API void BE_CALL BE_setExtraModuleFunction(BE_Handle handle, BE_ExtraModuleType type, int para0, int para1, int para2);
BE_API void BE_CALL BE_setMotorPositionSpeed(BE_Handle handle, int saccadeSpeed, int pursuitSpeed);

// 内存释放函数
BE_API void BE_CALL BE_freeImage(BE_Image* image);
BE_API void BE_CALL BE_freeCalibrationInfo(uint8_t* info);

BE_API const char * BE_CALL BE_getDeviceName();  //获取名字
BE_API const char * BE_CALL BE_getDeviceIP();    //获取IP  
#ifdef __cplusplus
}
#endif

#endif // BIONIC_EYES_C_H
