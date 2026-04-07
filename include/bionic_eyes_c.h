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


// pDevInfoList指向一个 BE_IpInfo的数组，大小可以自己定义， nSize 传入自己定义的数组大小
BE_API  int BE_CALL BE_GetAllRemoteDeviceList( BE_IpInfo *pDevInfoList, int nSize);

// 创建和销毁函数
//本机调用，不能用于跨机器远程调用（仿生眼安装在调用机器上）
/**************************************************
 * Creating BinoSense device instances for direct
 * connection use.
 * @brief Creating BinoSense device instances
 * @param openNetworkService:           Set #True# to use network service and support remote connect, or set false to
 * decrease CPU usage.
 * @param dataDefaultFrameRate:          Set default frame(including image， motor coder， IMU data, and so on..)
 * acquisition rate. You can still change it by calling function setDataRate. Max accepted value is 200. In some device
 *  and some image resolution setting, this value may be less than 200.\n
 * For example CBionicEyes *device = device->create(false, 50);
 * @note When setting openNetworkService true, you still need to start evo_be_Device_Service_**** program to support
 * remote user.
 **************************************************/
BE_API  BE_Handle BE_CALL BE_create_local(bool openNetworkService, int dataDefaultFrameRate, BE_LoggerPtr logger_ptr);


/**************************************************
 * Creating BinoSense device instances for remote connection use. When use this to create instance, all
 * devices in local network will be detected and listed, you can choose which device you want to use.
 * @brief Creating BinoSense device instances.
 * @note One instance can only connect one BinoSense device. If you have more BinoSense to connect, more
 * create functions should be called.
 * @param type:             Device connect type, for example, enumConnect_ImageControl means you can
 * control device and get its data including image info.
 * @param dataServerType:   You can choose connect local server to get device data or connect device to
 * get data directly. When local server is chosen, a local server must be started, or you can't get any data.
 * @param dataTransmissionType:     You can decide to get newest frame as soon as possible, or get every frame
 * with any frame lost.\n
 * For example CBionicEyes *device = device->create(enumConnect_ImageControl);
 **************************************************/
BE_API BE_Handle BE_CALL BE_create(BE_Connect_Type type);
//logger_ptr可填NULL

/**************************************************
 * Creating BinoSense device instances for remote connection use. When use this to create instance, all
 * devices in local network will be detected and listed, you can choose which device you want to use.
 * @brief Creating BinoSense device instances.
 * @note One instance can only connect one BinoSense device. If you have more BinoSense to connect, more
 * create functions should be called.
 * @param type:             Device connect type, for example, enumConnect_ImageControl means you can
 * control device and get its data including image info.
 * @param dataServerType:   You can choose connect local server to get device data or connect device to
 * get data directly. When local server is chosen, a local server must be started, or you can't get any data.
 * @param dataTransmissionType:     You can decide to get newest frame as soon as possible, or get every frame
 * with any frame lost.\n
 * For example CBionicEyes *device = device->create(enumConnect_ImageControl);
 **************************************************/
BE_API BE_Handle BE_CALL BE_create1(BE_Connect_Type type,
                         BE_Connect_DataServerType dataServerType,
                         BE_Data_TransmissionType dataTransmissionType,
                         BE_LoggerPtr logger_ptr);

/**************************************************
 * Creating BinoSense device instances for remote connection use. This function will connect device which IP
 * address is same to given IP address.
 * @brief Creating BinoSense device instances.
 * @note One instance can only connect one BinoSense device. If you have more BinoSense to connect, more
 * create functions should be called.
 * @param type:             Device connect type, for example, enumConnect_ImageControl means you can
 * control device and get its data including image info.
 * @param dataServerType:   You can choose connect local server to get device data or connect device to
 * get data directly. When local server is chosen, a local server must be started, or you can't get any data.
 * @param dataTransmissionType:     You can decide to get newest frame as soon as possible, or get every frame
 * with any frame lost.\n
 * For example BE_Handle device = BE_create_ip1("192.168.1.20", enumConnect_ImageControl);
 **************************************************/
BE_API BE_Handle BE_CALL BE_create_ip(const char* ipAddr,  BE_Connect_Type type);

/**************************************************
 * Creating BinoSense device instances for remote connection use. This function will connect device which IP
 * address is same to given IP address.
 * @brief Creating BinoSense device instances.
 * @note One instance can only connect one BinoSense device. If you have more BinoSense to connect, more
 * create functions should be called.
 * @param type:             Device connect type, for example, enumConnect_ImageControl means you can
 * control device and get its data including image info.
 * @param dataServerType:   You can choose connect local server to get device data or connect device to
 * get data directly. When local server is chosen, a local server must be started, or you can't get any data.
 * @param dataTransmissionType:     You can decide to get newest frame as soon as possible, or get every frame
 * with any frame lost.\n
 * For example BE_Handle device = BE_create_ip1("192.168.1.20", enumConnect_ImageControl);
 **************************************************/
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


/**************************************************
* @brief free BinoSense device handle.
**************************************************/
BE_API void BE_CALL BE_destroy(BE_Handle handle);

// 获取设备信息函数
/**************************************************
 * @brief Get connected BinoSense device IP address(32 bits integer).
 **************************************************/
BE_API uint32_t BE_CALL       BE_getBeDevice_Ip(BE_Handle handle);
/**************************************************
 * @brief Get connected BinoSense device type.
 **************************************************/
BE_API BE_Device_Type BE_CALL BE_getBeDeviceType(BE_Handle handle);
/**************************************************
 * @brief Get connected BinoSense device IP address(string type, like 192.168.1.1).
 **************************************************/
BE_API const char* BE_CALL    BE_getBeDevice_Ip_str(BE_Handle handle);

// 图像设置函数
/**************************************************
 * Set image resolution(SideBySide size!)(BE_ImageGeneralInfo.image, only for remote use)
 * @brief Creating Set image resolution.
 * @note This will affect function "getBeData" returning image size, but don't affect network transform image size.
 * @param size:     set image size, default (3840, 1080)
 * @param cameraId:     set specified camera, see
 **************************************************/
BE_API void  BE_CALL   BE_setImageResolution(BE_Handle handle, BE_Size size);

/**************************************************
 * Set network transfer image resolution(SideBySide size!), if network isn't ideal, you could down sample the transfer image
 * @brief Creating Set network transfer image resolution.
 * @note This won't affect function "getBeData" returning image size.
 * @param size:     set image size, default (1920, 540)
 **************************************************/
BE_API void  BE_CALL   BE_setImageResolution_Transfer(BE_Handle handle, BE_Size size);
/**************************************************
 * Set image color (BE_ImageGeneralInfo.colorType)
 * @brief Set image color
 * @note This will affect function "getBeData" returning image color type.
 * @param type:     set image color type, default enumColor
 **************************************************/
BE_API void  BE_CALL   BE_setImageColor(BE_Handle handle, BE_ImageColorType type);
/**************************************************
 * @brief Set network transfer image color
 * @note This won't affect function "getBeData" returning image color type.
 * @param type:     set image color type, default enumColor
 **************************************************/
BE_API void  BE_CALL   BE_setImageColor_Transfer(BE_Handle handle, BE_ImageColorType type);
/**************************************************
 * @brief Set network transfer frame rate
 * @param rate:     set transfer frame rate, default 25.0
 **************************************************/
BE_API void  BE_CALL   BE_setDataRate_Transfer(BE_Handle handle, float rate);
/**************************************************
 * @brief Get origin camera image resolution
 **************************************************/
BE_API BE_Size BE_CALL BE_getOriginImageResolution(BE_Handle handle);
/**************************************************
 * @brief Get max image frame rate
 **************************************************/
BE_API float BE_CALL   BE_getMaxImageFrameRate(BE_Handle handle);
/**************************************************
 * @brief Set image loseless compression or lossy compression (only for network transfer)
 * @note loseless compression: imgQual = 100; lossy compression: imgQual (0 = worst, 99 = best)
 **************************************************/
BE_API void  BE_CALL   BE_setImageCompressionQuality(BE_Handle handle, int imgQual);

// 数据获取函数
/**************************************************
 * @brief Determine if the data is ready.
 * @note Before calling "getBeData", you should ensure that this function return True.
 **************************************************/
BE_API bool  BE_CALL    BE_isBeDataReady(BE_Handle handle);
/**************************************************
 * @brief Get bedata.
 * @note Thread safe and returned data has be copied.(Deep copy)
 * @note Before calling this function, you should call
 * isBeDataReady first and ensure function isBeDataReady return True.
 **************************************************/
BE_API void  BE_CALL    BE_getBeData(BE_Handle handle, BE_GeneralData* data);
/**************************************************
 * @brief Get bedata.
 * @note Thread safe and returned data has be copied.(Deep copy)
 * @note Before calling this function, you should call
 * isBeDataReady first and ensure function isBeDataReady return True.
 **************************************************/
BE_API bool  BE_CALL    BE_getBeDataById(BE_Handle handle, BE_GeneralData* data, uint32_t id);
 /**************************************************
  * @brief Get Bionic Eye Calibration Data
  **************************************************/
BE_API uint8_t* BE_CALL BE_getCalibrationInfo(BE_Handle handle, size_t * tOutSize);

// 电机限制的阈值获取
/**************************************************
 * @brief Get motor up and down limit position(For eye).
 * @note Type(enumAllMotor) is not supported.
 * @param type:     Choose which motor you want to get up and down limit.
 * @param up:       Float data reference, return up limit value
 * @param down:     Float data reference, return down limit value
 **************************************************/
BE_API bool BE_getUpDownLimit(BE_Handle handle, BE_MotorType type, float* up, float* down);
/**************************************************
 * @brief Get motor up and down limit position(For neck)
 * @note Type(enumNeckAllMotor) is not supported.
 * @param type:     Choose which motor you want to get up and down limit.
 * @param up:       Float data reference,
 * @param down:     Float data reference,
 **************************************************/
BE_API bool BE_getUpDownLimit_Neck(BE_Handle handle, BE_MotorType_Neck type, float* up, float* down);

// 电机控制函数
/**************************************************
 * @brief Set eye absolute speed(deg/s).
 * @param activeFlag:     Bool data array(size = 6). Choose which motor or motors you want to set speed. If you set
 * activeFlag[id] = false; the no.id motor will auto set speed to 0.
 * @param speed:    Float data array(size = 6). Only for chosen motor, the value will work.
 **************************************************/
BE_API void BE_CALL BE_setAbsoluteSpeed(BE_Handle handle, bool activeFlag[6], float speed[6]);
/**************************************************
 * @brief Set eye absolute position
 * @param type:     Choose which motor or all motors you want to send move cmd
 * @param angle:    Float data array(size = 6). For chosen motor, the value will work.
 * @param moveType:     Set move type
 * @param movebaseType: Set move base Type
 **************************************************/
BE_API void BE_CALL BE_setAbsolutePosition(BE_Handle handle, BE_MotorType type, float angle[6],
                           BE_MovePatternType moveType, BE_MoveBaseType movebaseType);
/**************************************************
 * @brief Set Eye relative Position
 * @param type:     Choose which motor or all motors you want to send move cmd
 * @param angle:    Float data array(size = 6). For chosen motor, the value will work.
 * @param moveType:     Set move type
 * @param movebaseType: Set move base Type
 **************************************************/
BE_API void BE_CALL BE_setRelativePosition(BE_Handle handle, BE_MotorType type, float angle[6],
                           BE_MovePatternType moveType, BE_MoveBaseType movebaseType);
/**************************************************
 * @brief Set eye absolute position
 * @param activeFlag:     Bool data array(size = 6). Choose which motor or motors you want to send move cmd
 * @param angle:    Float data array(size = 6). Only for chosen motor, the value will work.
 * @param moveType:     Set move type
 * @param movebaseType: Set move base Type
 **************************************************/
BE_API void BE_CALL BE_setAbsolutePositionEx(BE_Handle handle, bool activeFlag[6], float angle[6],
                             BE_MovePatternType moveType, BE_MoveBaseType movebaseType);
/**************************************************
 * @brief Set eye relative position.
 * @param activeFlag:     Bool data array(size = 6). Choose which motor or motors you want to send move cmd
 * @param angle:    Float data array(size = 6). Only for chosen motor, the value will work.
 * @param moveType:     Set move type
 * @param movebaseType: Set move base Type
 **************************************************/
BE_API void BE_CALL BE_setRelativePositionEx(BE_Handle handle, bool activeFlag[6], float angle[6],
                             BE_MovePatternType moveType, BE_MoveBaseType movebaseType);

// 空间位置函数
/**************************************************
 * Set space Eye absolute Position （Absolute space）
 * @brief Set space Eye absolute Position
 * @note Only when device is on VOR mode, this function can work!
 * @param motorAngle:     Float data array(size = 6). Input current all motor value.
 * @param motorMoveAngle:    Float data array(size = 6). Set motor move angle value.
 * @param imuAngle:    Float data array(size = 3). Input current all IMU value.
 * @param moveType:     Set move type
 * @param movebaseType: Set move base Type
 **************************************************/
BE_API void BE_CALL BE_setSpaceAbsPosition(BE_Handle handle, float motorAngle[6], float motorMoveAngle[6],
                           float imuAngle[3], BE_MovePatternType moveType,
                           BE_MoveBaseType movebaseType);

// 颈部控制函数
/**************************************************
 * @brief Set neck absolute position (If neck exists)
 * @note Type(enumNeckAllMotor) is not supported.
 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
 * @param moveType:     Set move type
 **************************************************/
BE_API void BE_CALL BE_setAbsolutePosition_Neck(BE_Handle handle, bool activeFlag[3], float angle[3],  BE_MovePatternType moveType);
/**************************************************
 * Set neck relative position (If neck exists) (Array Size = 3)
 * @brief Set neck space eye relative position
 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
 * @param moveType:     Set move type
 **************************************************/
BE_API void BE_CALL BE_setRelativePosition_Neck(BE_Handle handle, bool activeFlag[3], float angle[3],  BE_MovePatternType moveType);
/**************************************************
 * @brief Set neck & eye union position （Absolute space）
 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
 * @param moveType:     Set move type
 **************************************************/
BE_API void BE_CALL BE_setUnionAbsolutePosition_NeckEye(BE_Handle handle, bool activeFlag[3],
                                        float eyeAngle[6], float neckAngle[3],
                                        BE_MovePatternType moveType);
/**************************************************
 * Set neck relative position (If neck exists) (Array Size = 3)
 * @brief Set neck space eye relative position
 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
 * @param moveType:     Set move type
 **************************************************/
BE_API void BE_CALL BE_setUnionRelativePosition_NeckEye(BE_Handle handle, bool activeFlag[3],
                                        float eyeAngle[6], float neckAngle[3],
                                        BE_MovePatternType moveType);

// 初始位置函数
/**************************************************
 * Set current position as init position command. Current motor value will turn 0 after restart.
 * @brief Set current position as init position command
 * @note Current postion will be initial position. Effective after restart.
 **************************************************/
BE_API void BE_CALL BE_setNowPositionAsInitPos(BE_Handle handle);
/**************************************************
 * @brief Chosen motor or motor will go to init position.
 * @param type: Set chosen eye motor.
 **************************************************/
BE_API void BE_CALL BE_goInitPosition(BE_Handle handle, BE_MotorType type);
/**************************************************
 * @brief Get chosen motor or motors init position value;
 * @param type: Set chosen eye motor.
 * @param angle: return motor initial position value
 **************************************************/
BE_API void BE_CALL BE_getInitPosition(BE_Handle handle, BE_MotorType type, float angle[6]);

/**************************************************
 * @brief Chosen neck motor or motor will go to init position.
 * @param type: Set chosen neck motor.
 **************************************************/
BE_API void BE_CALL BE_goInitPosition_Neck(BE_Handle handle, BE_MotorType_Neck type);
/**************************************************
 * @brief Get chosen neck motor or motors init position value.
 * @param type: Set chosen neck motor.
 * @param angle: return neck motor initial position value
 **************************************************/
BE_API void BE_CALL BE_getInitPosition_Neck(BE_Handle handle, BE_MotorType_Neck type, float angle[3]);

// 设备状态函数
/**************************************************
 * @brief return true if neck existed
 **************************************************/
BE_API bool BE_CALL BE_haveNeckLinked(BE_Handle handle);

/**************************************************
 * @brief Open or close VOR function
 * @param vor_eye on/off eye vor
 * @param vor_neck on/off neck vor
 **************************************************/
BE_API void BE_CALL BE_onoff_VOR(BE_Handle handle, bool vor_eye, bool vor_neck);
/**************************************************
 * @brief Open or close SV function
 * @param onoff on/off SV function
 **************************************************/
BE_API void BE_CALL BE_onoff_SV(BE_Handle handle, bool onoff);
/**************************************************
 * @brief Set sv work mode
 * @param type: set sv work mode type (see SVType)
 * @param rect: set work area
 * @param size: set origin image size
 **************************************************/
BE_API void BE_CALL BE_setSvModel(BE_Handle handle, BE_SVType type, BE_Rect rect, BE_Size size);
/**************************************************
 * Set be-device synchronizing signal source
 * @brief Set be-device synchronizing signal source
 * @param flag: True: send signal, False: only recv signal
 **************************************************/
BE_API void BE_CALL BE_setSyncSignalSource(BE_Handle handle, bool flag);

// 相机控制函数
/**************************************************
 * @brief Get camera exposure time set range
 * @param up: return exposure time up_limit
 * @param down: return exposure time down_limit
 **************************************************/
BE_API void BE_CALL  BE_getCameraExposureTimeRange(BE_Handle handle, float* up, float* down);
/**************************************************
 * @brief Get camera exposure time
 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
 **************************************************/
BE_API float BE_CALL BE_getCameraExposureTime(BE_Handle handle, int type);

/**************************************************
 * @brief Set camera exposure time
 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
 * @param autoExposure: True: set auto exposure mode, False: set manual exposure mode
 * @param exposureTime: set exposure time. Only works when autoExposure = False;
 **************************************************/
BE_API void BE_CALL  BE_setCameraExposure(BE_Handle handle, int type, bool autoExposure, float exposureTime);

/**************************************************
 * @brief Get Camera WhiteBalance Temperature set range
 * @param up: return WhiteBalance Temperature up_limit
 * @param down: return WhiteBalance Temperature down_limit
 **************************************************/
BE_API void BE_CALL  BE_getCameraWhiteBalanceTemperatureRange(BE_Handle handle, int* up, int* down);
/**************************************************
 * @brief Get Camera WhiteBalance Temperature
 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
 **************************************************/
BE_API float BE_CALL BE_getCameraWhiteBalanceTemperature(BE_Handle handle, int type);
/**************************************************
 * @brief Set Camera WhiteBalance Temperature
 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
 * @param autoWhitebalance: True: set whitebalance temperature mode, False: set manual whitebalance temperature mode
 * @param WhiteBalanceTemperature: set whitebalance temperature. Only works when autoExposure = False;
 **************************************************/
BE_API void BE_CALL  BE_setCameraWhiteBalanceTemperature(BE_Handle handle, int type, bool autoWhitebalance,
                                         float WhiteBalanceTemperature);

/**************************************************
 * @brief Get camera exposure auto reference range
 * @param up: Return up limit
 * @param down: Return down limit
 **************************************************/
BE_API void BE_CALL BE_getCameraExposureAutoReferenceRange(BE_Handle handle, float* up, float* down);
/**************************************************
 * @brief Set camera exposure auto reference
 * @note This function only supports some specified BinoSense Device. For example, BinoSense V3 is not
 * supported.
 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
 * @param value: set exposure exposure auto reference value
 **************************************************/
BE_API void BE_CALL BE_setCameraExposureAutoReference(BE_Handle handle, int type, int value);

/**************************************************
 * @brief Get camera gain range
 * @param up: Return up limit
 * @param down: Return down limit
 **************************************************/
BE_API void BE_CALL BE_getCameraGainRange(BE_Handle handle, float* up, float* down);

/**************************************************
 * @brief Set camera Gain work mode
 * @note This function only supports some specified BinoSense Device. For example, BinoSense V3 is not
 * supported.
 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
 * @param autoGain: Set camera gain set work in auto mode or manually mode.
 * @param value: set gain value(Only works when autoGain = False)
 **************************************************/
BE_API void BE_CALL BE_setCameraGain(BE_Handle handle, int type, bool autoGain, int value);

// 数据保存函数
/**************************************************
 * Start to save all data when calling this function.
 * @brief Save all be_data
 * @note If you have chosen high image resolution, note the disk capacity.
 * You can change saving image size by calling setImageResolution_Transfer
 * @param startStop: 0, stop; 1, start;
 * @param folderPath: set data_saving path. If path exists, this function will auto generate a new path
 * named path_T.
 **************************************************/
BE_API void BE_CALL BE_saveBeData(BE_Handle handle, bool startStop, const char* folderPath);
/**************************************************
 * Snap be_data when calling this function. May have some time delay(ms).
 * @brief Snap be_data when calling this function.
 * @note You can change saving image size by calling setImageResolution_Transfer
 * @note After calling snapBeData in the first time, the param folderPath will not work until
 * function stopSnapBeData is called.
 * @param description: You can give this snaped data a description. It will be written
 * in CSV file.
 * @param folderPath: set data saving path. If path exists, this function will auto
 * generate a new path named path_T.
 **************************************************/
BE_API bool BE_CALL BE_snapBeData(BE_Handle handle, const char* description, const char* folderPath);
/**************************************************
 * Stop snapping be_data when calling this function. May have some time delay(ms).
 * @brief Stop snapping be_data when calling this function.
 * @note If you want to change a folder to save snaped data, call this function first
 * and then recall snapBeData.
 **************************************************/
BE_API void BE_CALL BE_stopSnapBeData(BE_Handle handle);
/**************************************************
 * Record be_data when calling this function. May have some time delay(ms).
 * @brief Record be_data when calling this function.
 * @note You can change saving image size by calling setImageResolution_Transfer
 * @note After calling recordBeData in the first time, the param folderPath will not work until
 * function stopRecordBeData is called.
 * @param data: The data will be recorded.
 * @param description: You can give this recorded data a description. It will be written
 * in CSV file.
 * @param folderPath: set data saving path. If path exists, this function will auto
 * generate a new path named path_T.
 **************************************************/
BE_API bool BE_CALL BE_recordBeData(BE_Handle handle, BE_GeneralData* data, const char* description,  const char* folderPath);
/**************************************************
 * Stop recording be_data when calling this function.
 * @brief Stop recording be_data when calling this function.
 * @note If you want to change a folder to save recorded data, call this function
 * first and then recall recordBeData. It's similar to snapBeData & stopSnapBeData.
 **************************************************/
BE_API void BE_CALL BE_stopRecordBeData(BE_Handle handle);

// 其他功能函数
/**************************************************
 * Give a trigger signal to data server to send next data(Only for remote connect use,
 * and parameter dataTransmissionType must be enumDataTransmission_OneByOne)
 * @brief Give a trigger signal to data server to send a next data
 **************************************************/
BE_API void BE_CALL BE_triggerDataTransmission(BE_Handle handle);
/*! @brief Set grab data rate, Max 200 fps.*/
BE_API void BE_CALL BE_setBeDataRate(BE_Handle handle, int rate);
/*! @brief Set sync camera delay time: max 30000 us.*/
BE_API void BE_CALL BE_setSyncCameraDelayTime(BE_Handle handle, int timeDelay);

//红外相机
BE_API bool BE_CALL  BE_infraImageTrans(BE_Handle handle, BE_GeneralData* data, BE_Image* infraImage,  BE_Image* tempImage);
BE_API float BE_CALL BE_infraTempCalc(BE_Handle handle, uint32_t value);

/**************************************************
* On/Off camera image grab or network transform. It will help to save system resource or bandwidth resources
* @brief On/Off camera image grab or network transform
* @param flag: Bool data array(size = MAXCAMERASInDEVICE). Set 1 means ON and set 0 means OFF
* @param NetworkType: 1 means only affect network transform, 0 means affect both network and local image grab
**************************************************/
BE_API void BE_CALL BE_onOffCameraImage(BE_Handle handle, bool flag[], bool onlyNetworkType);

//变焦相机控制变焦倍数
/**************************************************
 * @brief Set zoom camera module focal length, range: 1~30
 **************************************************/
BE_API void BE_CALL BE_setZoomFocalLength(BE_Handle handle, uint8_t focalLength);

//鹰眼 雨刮器
/**************************************************
* @brief Set wipers module(Only for eagle eye device),0: close ,1~255 :open, different speed
**************************************************/
BE_API void BE_CALL BE_setWipers(BE_Handle handle, uint8_t value);

// for wiper: para0, para1 work.
//      para0: id, 暂时无用
//      para1: 0 停止运动， 1~255 速度
// for zoom camera focal length: para0 works.
//      para0: 1~30
// for laserranging: para0, para1 work.
//      para0: id, 暂时无用
//      para1: 1/0，开/关
// for infrared filling lamp: para0, para1, para2 work.
//      para0: id, 暂时无用
//      para1: 1/0，开/关  
//      para2: 焦距， 0~255  
// for mouth: para0 works.
//      para0: 0x00 闭合，0x01 微张，0x02 半张，0x03 全张，0x0A 慢速随机，0x0B 快速随机，0x0C 跟随喇叭动
// for lighting: para0 and para1 work.
//      para0: 0x00 额头灯，0x01 胸部灯，0x02 右耳灯，0x03 左耳灯，0x04 底盘灯
//      para1: 0xFE 呼吸，0xFF 关闭，0x00 常亮， 其余为闪烁的频率
// for arm: para0, para1, and para2 work.
//      para0: 0xC8 右手臂， 0xC9 左手臂
//      para1: 0x00 执行动作， 0x01 学习动作
//      para2: 0x00~0x0F 16个动作， 0x10 解锁关节,  0x11 锁定关节, 0x12 设置为初始位置
BE_API void BE_CALL BE_setExtraModuleFunction(BE_Handle handle, BE_ExtraModuleType type, int para0, int para1, int para2);

//设置电机运行速度
BE_API void BE_CALL BE_setMotorPositionSpeed(BE_Handle handle, int saccadeSpeed, int pursuitSpeed);

//设置深度计算的相关参数
//onOff   --是否开启
//svOnOff --是否开启SV
//precision --返回深度值的精度，1000 对应毫米， 100对应厘米， 10 对应分米， 1 对应米
//min   有效深度的最小值
//max   有效深度的最大值
BE_API void BE_CALL BE_setDepthControl(BE_Handle handle, bool onOff, bool svOnOff, int precision, float min, float max);

//根据指定id返回如下数据：
// Z 距离数据， 大小short类型640×480大小， 内存由调用侧指定
// L0LnRT左眼0位置到左眼新位置的旋转平移矩阵 float 6大小的数组 , 内存由调用侧指定
// KLrect 畸变校正后的左眼内参 9 float大小的数组 ，内存由调用侧指定
// bgr_l 左眼校正后彩图 内部的内存，也最好调用侧分配 
BE_API bool BE_CALL BE_getDepthInfo(BE_Handle handle, int id, uint16_t *Z, float *L0LnRT, float *KLrect,   BE_Image*  bgr_l);

//获取最新一个深度帧信息，
// id  返回对应双目原始图像的id
// Z 距离数据， 大小short类型640×480大小， 内存由调用侧指定
// L0LnRT左眼0位置到左眼新位置的旋转平移矩阵 float 6大小的数组 , 内存由调用侧指定
// KLrect 畸变校正后的左眼内参 9 float大小的数组 ，内存由调用侧指定
// bgr_l 左眼校正后彩图 内部的内存，也最好调用侧分配 
BE_API bool BE_CALL BE_getNewestDepthInfo(BE_Handle handle, int *id, uint16_t *Z, float *L0LnRT, float *KLrect,  BE_Image*  bgr_l);

// 内存释放函数
BE_API void BE_CALL BE_freeImage(BE_Image* image);
BE_API void BE_CALL BE_freeCalibrationInfo(uint8_t* info);

BE_API const char * BE_CALL BE_getDeviceName();  //获取名字
BE_API const char * BE_CALL BE_getDeviceIP();    //获取IP  
#ifdef __cplusplus
}
#endif

#endif // BIONIC_EYES_C_H
