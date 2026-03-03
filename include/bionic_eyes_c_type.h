#ifndef BIONIC_EYES_C_TYPE_H
#define BIONIC_EYES_C_TYPE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// 枚举类型定义
typedef enum {
	enumLeft = 0,
	enumRight = 1,
	enumBoth = 2,
} CameraIndex;

typedef enum {
		enumdisConnect = 0,			        /**< No connect BE device(s) */
		enumConnect_Control = 1,	        /**< Only connect to control BE device */
		enumConnect_Image = 2,		        /**< Only connect to get image info */
		enumConnect_ImageControl = 3,       /**< Connect to control BE device and get image info */
} BE_Connect_Type;

typedef enum {
		enumLocalServer_First = 0,	        /**< For remote use, first connect Local server */
		enumDeviceServer_First = 1,         /**< For remote use, first connect BinoSense device server */
		enumDeviceServer_Only = 2,	        /**< For remote use, only connect BinoSense device server */
		enumLocalServer_Only = 3,	        /**< For remote use, only connect Local server */
} BE_Connect_DataServerType;

typedef enum {
    enumDataTransmission_ASPAP = 0,
    enumDataTransmission_OneByOne = 1,
} BE_Data_TransmissionType;

/**************************************************
* @brief Enumeration of IMU Data Type
 **************************************************/
typedef enum 
{
		enumAcc = 0,           /**< Acceleration */
		enumGyo = 1,           /**< Gyroscope */
		enumMag = 2,           /**< Magnetometer */
		enumEuler = 3,         /**< Euler angle */
		enumQuat = 4,          /**< Quaternion */
		enumLaser = 5,         /**< LaserData */
} BE_IMUDataType;

typedef enum {
		enumRightPitch = 0,     /**< Right Pitch Motor */
		enumRightRoll = 1,	    /**< Right Roll Motor */
		enumRightYaw = 2,	    /**< Right Yaw Motor */
		enumLeftPitch = 3,	    /**< Left Pitch Motor */
		enumLeftRoll = 4,	    /**< Left Roll Motor */
		enumLeftYaw = 5,	    /**< Left Yaw Motor */
		enumAllMotor = 6,	    /**< All Motor */
		enumNoMotor = 7,	    /**< No Motor */
} BE_MotorType;

typedef enum {
		enumNeckPitch    = 0,	  /**< Neck Pitch Motor */
		enumNeckRoll     = 1,     /**< Neck Roll Motor */
		enumNeckYaw      = 2,     /**< Neck Yaw Motor */
		enumNeckAllMotor = 3,     /**< Neck All Motor */
		enumNeckNoMotor  = 4,     /**< Neck No Motor */
} BE_MotorType_Neck;

typedef enum {
		enumMovePattern_Saccade = 0,	        /**< Saccade move (Fast mode) */
		enumMovePattern_SmoothPursuit = 1,      /**< Smooth pursuit move (Smooth mode) */
} BE_MovePatternType;

typedef enum {
	    /**************************************************
         * Binosense device moves independently
         * (left roll/pitch/yaw moves have no relationship
         * with right roll/pitch/yaw moves).
         * In this mode, SV don't work.
         **************************************************/
        enumMoveBase_Independent = 0,
        /**************************************************
         * Binosense moves based on left eye(only left
         * roll/pitch/yaw moves is base,
         * right eye move is same to left eye).
         **************************************************/
        enumMoveBase_LeftEye     = 1,
        /**************************************************
         * Binosense moves based on right eye(only right
         * roll/pitch/yaw moves is base,
         * left eye move is same to right eye).
         **************************************************/
		enumMoveBase_RightEye    = 2,
} BE_MoveBaseType;

typedef enum {
		enumMono  = 0,
		enumColor = 1,
} BE_ImageColorType;



typedef enum {
		enumGlobalSV  = 0,         /**< Global sv */
		enumCentreSV  = 1,         /**< Centre sv */
		enumAreaSV    = 2,	       /**< Area sv */
} BE_SVType;

typedef enum {
        enumLaserRanging,
        enumInfraredFillingLamp,
        enumZoomCamera,
        enumWiper,
        enumMouth_SDF,                  /**< Single Degree of Freedom Mouth*/
        enumLighting,
        enumRobotArm_5DOF,
} BE_ExtraModuleType;

typedef enum {
        enumUnknownDevice     = 0,     /**< Unknown device */
        enumHumanEye_Serial   = 1,     /**< BE device which is serial structure type */
        enumHumanEye_Parallel = 2,     /**< BE device which is parallel structure type */
        enumEagleEye          = 3,     /**< Bionic eagle eye device */
        enumDevice_ThreeAxis  = 4,     /**< Three axis platform device */
} BE_Device_Type;


typedef enum {
    enumBGR,
    enumYUV420P,
    enumNV12,
    enumRGB
} BE_Image_Storage_Type;

// 定义TLV格式的标签类型
enum TLVTag {
    TAG_K_0 = 1,
    TAG_K_1 = 2,
    TAG_D_0 = 3,
    TAG_D_1 = 4,
    TAG_R_0 = 5,
    TAG_R_1 = 6,
    TAG_R_2 = 7,
    TAG_R_3 = 8,
    TAG_R_4 = 9,
    TAG_R_5 = 10,
    TAG_R_6 = 11,
    TAG_T_0 = 12,
    TAG_T_1 = 13,
    TAG_T_2 = 14,
    TAG_T_3 = 15,
    TAG_T_4 = 16,
    TAG_T_5 = 17,
    TAG_T_6 = 18,
    TAG_MOTORS_0 = 19,
    TAG_MOTORS_1 = 20,
    TAG_MOTORS_2 = 21,
    TAG_EYES_0 = 22,
    TAG_EYES_1 = 23
};
// 大小和矩形结构体 (替代OpenCV的cv::Size和cv::Rect)
typedef struct 
{
    int width;
    int height;
} BE_Size;

typedef struct 
{
    int x;
    int y;
    int width;
    int height;
} BE_Rect;

// 图像数据
typedef struct 
{
    uint8_t* data;
    int      width;
    int      height;
    int      channels;
    int      format; //格式 YUV420, NV12, RGB, BGR, RGBA, BGRA
    size_t   data_size;
} BE_Image;

// 通用数据结构体
typedef struct 
{
    BE_Image  Image_data[4];
 
    uint64_t  frame_id;
    uint64_t  timestamp;

    float     imuData[16][4];                           /**< IMU Data 目前使用1-6对应 IMUDataType ，每个Type对应4个数据*/
    float     motorData[6];                             /**< Motor Coder Data */
    double    gpsData[3];	                            /**< GPS Data(longitude, latitude, height) */
    int64_t   gpsTime;                                  /*GPS UTC Time*/
    int64_t   isMovingFastly;                           /**< whether eyes move fastly when grab this data */
} BE_GeneralData;


#endif


