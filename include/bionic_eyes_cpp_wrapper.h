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
	BE_CalibData_w deserialize_from_c_array(const uint8_t *data, size_t data_size) {
		BE_CalibData_w obj = {};
		size_t offset = 0;

		while (offset < data_size) {
			if (offset + sizeof(uint32_t) * 2 > data_size) break;

			uint32_t tag, length;
			memcpy(&tag, data + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			memcpy(&length, data + offset, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			if (offset + length > data_size) break;

			switch (tag) {
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
				obj.motors[0] = std::string(reinterpret_cast<const char *>(data + offset), length);
				break;
			case TAG_MOTORS_1:
				obj.motors[1] = std::string(reinterpret_cast<const char *>(data + offset), length);
				break;
			case TAG_MOTORS_2:
				obj.motors[2] = std::string(reinterpret_cast<const char *>(data + offset), length);
				break;
			case TAG_EYES_0:
				obj.eyes[0] = std::string(reinterpret_cast<const char *>(data + offset), length);
				break;
			case TAG_EYES_1:
				obj.eyes[1] = std::string(reinterpret_cast<const char *>(data + offset), length);
				break;
			}

			offset += length;
		}

		return obj;
	}

	namespace detail {
		// 将std::vector<bool>转换为bool数组
		template<size_t N>
		static void convertVectorToArray(const std::vector<bool> &vec, bool(&arr)[N])
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
		static int GetAllRemoteDeviceList(std::vector<BE_IpInfo> &tDevList)
		{
			BE_IpInfo tDevListTmp[32] = { {0} };
			int nDevNum = BE_GetAllRemoteDeviceList(&tDevListTmp[0], 32);
			if (nDevNum > 0)
			{
				for (int i = 0; i < nDevNum; i++)
					tDevList.push_back(tDevListTmp[i]);
			}
			return nDevNum;
		}
		BionicEyesWrapper(bool openNetworkService = true, int dataDefaultFrameRate = 25, void *logger_ptr = nullptr)
			: handle_(nullptr)
		{
			handle_ = BE_create_local(openNetworkService, dataDefaultFrameRate, logger_ptr);
		}
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
		 * For example BionicEyesWrapper device = BionicEyesWrapper(enumConnect_ImageControl);
		 **************************************************/
		BionicEyesWrapper(BE_Connect_Type type,
			BE_Connect_DataServerType dataServerType = enumDeviceServer_Only,
			BE_Data_TransmissionType dataTransmissionType = enumDataTransmission_ASPAP,
			void *logger_ptr = nullptr)
			: handle_(nullptr)
		{
			handle_ = BE_create1(type, dataServerType, dataTransmissionType, logger_ptr);
		}
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
		 * For example BionicEyesWrapper device = BionicEyesWrapper("192.168.1.20", enumConnect_ImageControl);
		 **************************************************/
		BionicEyesWrapper(const std::string &ipAddr,
			BE_Connect_Type type = enumdisConnect,
			BE_Connect_DataServerType dataServerType = enumDeviceServer_Only,
			BE_Data_TransmissionType dataTransmissionType = enumDataTransmission_ASPAP,
			void *logger_ptr = nullptr)
			: handle_(nullptr)
		{
			handle_ = BE_create_ip1(ipAddr.c_str(), type, dataServerType, dataTransmissionType, logger_ptr);
		}
#ifdef EAGLE
		BionicEyesWrapper(const std::string &SrvipAddr,
			const std::string &devName,
			void *logger_ptr = nullptr,
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
		BionicEyesWrapper(const BionicEyesWrapper &) = delete;
		BionicEyesWrapper &operator=(const BionicEyesWrapper &) = delete;

		// 允许移动
		BionicEyesWrapper(BionicEyesWrapper &&other) noexcept : handle_(other.handle_)
		{
			other.handle_ = nullptr;
		}

		BionicEyesWrapper &operator=(BionicEyesWrapper &&other) noexcept
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
		/**************************************************
		 * @brief Get connected BinoSense device IP address(32 bits integer).
		 **************************************************/
		uint32_t getBeDevice_Ip() const
		{
			return BE_getBeDevice_Ip(handle_);
		}
		/**************************************************
		 * @brief Get connected BinoSense device type.
		 **************************************************/
		BE_Device_Type getBeDeviceType() const
		{
			return BE_getBeDeviceType(handle_);
		}
		/**************************************************
 		* @brief Get connected BinoSense device IP address(string type, like 192.168.1.1).
  		**************************************************/
		std::string getBeDevice_Ip_str() const
		{
			const char *ip = BE_getBeDevice_Ip_str(handle_);
			return ip ? std::string(ip) : std::string();
		}

		// 图像设置
		/**************************************************
		 * Set image resolution(SideBySide size!)(BE_ImageGeneralInfo.image, only for remote use)
		 * @brief Creating Set image resolution.
		 * @note This will affect function "getBeData" returning image size, but don't affect network transform image size.
		 * @param size:     set image size, default (3840, 1080)
		 * @param cameraId:     set specified camera, see
		 **************************************************/
		void setImageResolution(int width = 1920, int height = 1080)
		{
			BE_Size size = { width, height };
			BE_setImageResolution(handle_, size);
		}
		/**************************************************
		 * Set network transfer image resolution(SideBySide size!), if network isn't ideal, you could down sample the transfer image
		 * @brief Creating Set network transfer image resolution.
		 * @note This won't affect function "getBeData" returning image size.
		 * @param size:     set image size, default (1920, 540)
		 **************************************************/
		void setImageResolution_Transfer(int width = 960, int height = 540)
		{
			BE_Size size = { width, height };
			BE_setImageResolution_Transfer(handle_, size);
		}
		/**************************************************
		 * Set image color (BE_ImageGeneralInfo.colorType)
		 * @brief Set image color
		 * @note This will affect function "getBeData" returning image color type.
		 * @param type:     set image color type, default enumColor
		 **************************************************/
		void setImageColor(BE_ImageColorType type = enumColor)
		{
			BE_setImageColor(handle_, type);
		}
		/**************************************************
		 * @brief Set network transfer image color
		 * @note This won't affect function "getBeData" returning image color type.
		 * @param type:     set image color type, default enumColor
		 **************************************************/
		void setImageColor_Transfer(BE_ImageColorType type = enumColor)
		{
			BE_setImageColor_Transfer(handle_, type);
		}
		/**************************************************
		 * @brief Set network transfer frame rate
		 * @param rate:     set transfer frame rate, default 25.0
		 **************************************************/
		void setDataRate_Transfer(float rate = 25.0f)
		{
			BE_setDataRate_Transfer(handle_, rate);
		}
		/**************************************************
		 * @brief Get origin camera image resolution
		 **************************************************/
		std::pair<int, int> getOriginImageResolution() const
		{
			BE_Size size = BE_getOriginImageResolution(handle_);
			return { size.width, size.height };
		}
		/**************************************************
		 * @brief Get max image frame rate
		 **************************************************/
		float getMaxImageFrameRate() const
		{
			return BE_getMaxImageFrameRate(handle_);
		}
		/**************************************************
		 * @brief Set image loseless compression or lossy compression (only for network transfer)
		 * @note loseless compression: imgQual = 100; lossy compression: imgQual (0 = worst, 99 = best)
		 **************************************************/
		void setImageCompressionQuality(int imgQual = 95)
		{
			BE_setImageCompressionQuality(handle_, imgQual);
		}

		// 数据获取
		/**************************************************
		 * @brief Determine if the data is ready.
		 * @note Before calling "getBeData", you should ensure that this function return True.
		 **************************************************/
		bool isBeDataReady() const
		{
			return BE_isBeDataReady(handle_);
		}
		/**************************************************
		 * @brief Get bedata.
		 * @note Thread safe and returned data has be copied.(Deep copy)
		 * @note Before calling this function, you should call
		 * isBeDataReady first and ensure function isBeDataReady return True.
		 **************************************************/
		bool getBeData(BE_GeneralData &data) const
		{
			BE_getBeData(handle_, &data);
			return true;
		}
		/**************************************************
		 * @brief Get bedata.
		 * @note Thread safe and returned data has be copied.(Deep copy)
		 * @note Before calling this function, you should call
		 * isBeDataReady first and ensure function isBeDataReady return True.
		 **************************************************/
		bool getBeData(BE_GeneralData &data, uint32_t id) const
		{
			return BE_getBeDataById(handle_, &data, id);
		}
 		/**************************************************
 		 * @brief Get Bionic Eye Calibration Data
 		 **************************************************/
		BE_CalibData_w  getCalibrationInfo() const
		{
			size_t nOutSize = 0;
			uint8_t *info = BE_getCalibrationInfo(handle_, &nOutSize);
			if (!info) return BE_CalibData_w();

			BE_CalibData_w tmp = deserialize_from_c_array(info, nOutSize);
			delete[] info;
			return tmp;
		}


		// 电机限制
       /**************************************************
        * @brief Get motor up and down limit position(For eye).
        * @note Type(enumAllMotor) is not supported.
        * @param type:     Choose which motor you want to get up and down limit.
        * @param up:       Float data reference, return up limit value
        * @param down:     Float data reference, return down limit value
        **************************************************/
		std::pair<float, float> getUpDownLimit(BE_MotorType type) const
		{
			float up = 0, down = 0;
			BE_getUpDownLimit(handle_, type, &up, &down);
			return { up, down };
		}
		/**************************************************
		 * @brief Get motor up and down limit position(For neck)
		 * @note Type(enumNeckAllMotor) is not supported.
		 * @param type:     Choose which motor you want to get up and down limit.
		 * @param up:       Float data reference,
		 * @param down:     Float data reference,
		 **************************************************/
		std::pair<float, float> getUpDownLimit_Neck(BE_MotorType_Neck type) const
		{
			float up = 0, down = 0;
			BE_getUpDownLimit_Neck(handle_, type, &up, &down);
			return { up, down };
		}

		// 电机控制
		/**************************************************
		 * @brief Set eye absolute speed(deg/s).
		 * @param activeFlag:     Bool data array(size = 6). Choose which motor or motors you want to set speed. If you set
		 * activeFlag[id] = false; the no.id motor will auto set speed to 0.
		 * @param speed:    Float data array(size = 6). Only for chosen motor, the value will work.
		 **************************************************/
		void setAbsoluteSpeed(bool *activeFlag, float *speed)
		{
			BE_setAbsoluteSpeed(handle_, activeFlag, speed);
		}
		/**************************************************
		 * @brief Set eye absolute position
		 * @param type:     Choose which motor or all motors you want to send move cmd
		 * @param angle:    Float data array(size = 6). For chosen motor, the value will work.
		 * @param moveType:     Set move type
		 * @param movebaseType: Set move base Type
		 **************************************************/
		void setAbsoluteSpeed(const std::vector<bool> &activeFlag, const std::vector<float> &speed)
		{
			if (activeFlag.size() >= 6 && speed.size() >= 6)
			{
				bool active_flags[6];
				detail::convertVectorToArray(activeFlag, active_flags);
				BE_setAbsoluteSpeed(handle_, active_flags, const_cast<float *>(speed.data()));
			}
		}
		/**************************************************
		 * @brief Set Eye relative Position
		 * @param type:     Choose which motor or all motors you want to send move cmd
		 * @param angle:    Float data array(size = 6). For chosen motor, the value will work.
		 * @param moveType:     Set move type
		 * @param movebaseType: Set move base Type
		 **************************************************/
		void setAbsolutePosition(BE_MotorType type, float *angle,
			BE_MovePatternType moveType = enumMovePattern_Saccade,
			BE_MoveBaseType movebaseType = enumMoveBase_Independent)
		{
			BE_setAbsolutePosition(handle_, type, angle, moveType, movebaseType);
		}
		/**************************************************
		 * @brief Set eye absolute position
		 * @param activeFlag:     Bool data array(size = 6). Choose which motor or motors you want to send move cmd
		 * @param angle:    Float data array(size = 6). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 * @param movebaseType: Set move base Type
		 **************************************************/
		void setAbsolutePosition(BE_MotorType type, const std::vector<float> &angle,
			BE_MovePatternType moveType = enumMovePattern_Saccade,
			BE_MoveBaseType movebaseType = enumMoveBase_Independent)
		{
			if (angle.size() >= 6)
			{
				BE_setAbsolutePosition(handle_, type, const_cast<float *>(angle.data()), moveType, movebaseType);
			}
		}
		/**************************************************
		 * @brief Set eye relative position.
		 * @param activeFlag:     Bool data array(size = 6). Choose which motor or motors you want to send move cmd
		 * @param angle:    Float data array(size = 6). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 * @param movebaseType: Set move base Type
		 **************************************************/
		void setRelativePosition(BE_MotorType type, float *angle, BE_MovePatternType moveType = enumMovePattern_Saccade, BE_MoveBaseType movebaseType = enumMoveBase_Independent)
		{
			BE_setRelativePosition(handle_, type, angle, moveType, movebaseType);
		}
		/**************************************************
		 * @brief Set eye relative position.
		 * @param activeFlag:     Bool data array(size = 6). Choose which motor or motors you want to send move cmd
		 * @param angle:    Float data array(size = 6). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 * @param movebaseType: Set move base Type
		 **************************************************/
		void setRelativePosition(BE_MotorType type, const std::vector<float> &angle, BE_MovePatternType moveType = enumMovePattern_Saccade, BE_MoveBaseType movebaseType = enumMoveBase_Independent)
		{
			if (angle.size() >= 6)
			{
				BE_setRelativePosition(handle_, type, const_cast<float *>(angle.data()), moveType, movebaseType);
			}
		}
		/**************************************************
		 * @brief Set eye absolute position
		 * @param type:     Choose which motor or all motors you want to send move cmd
		 * @param angle:    Float data array(size = 6). For chosen motor, the value will work.
		 * @param moveType:     Set move type
		 * @param movebaseType: Set move base Type
		 **************************************************/
		void setAbsolutePosition(bool *activeFlag, float *angle,
			BE_MovePatternType moveType = enumMovePattern_Saccade,
			BE_MoveBaseType movebaseType = enumMoveBase_Independent)
		{
			BE_setAbsolutePositionEx(handle_,
				activeFlag,
				angle,
				moveType, movebaseType);
		}
		/**************************************************
		 * @brief Set eye absolute position
		 * @param type:     Choose which motor or all motors you want to send move cmd
		 * @param angle:    Float data array(size = 6). For chosen motor, the value will work.
		 * @param moveType:     Set move type
		 * @param movebaseType: Set move base Type
		 **************************************************/
		void setAbsolutePosition(const std::vector<bool> &activeFlag, const std::vector<float> &angle,
			BE_MovePatternType moveType = enumMovePattern_Saccade,
			BE_MoveBaseType movebaseType = enumMoveBase_Independent)
		{
			if (activeFlag.size() >= 6 && angle.size() >= 6)
			{
				bool active_flags[6];
				detail::convertVectorToArray(activeFlag, active_flags);
				BE_setAbsolutePositionEx(handle_,
					active_flags,
					const_cast<float *>(angle.data()),
					moveType, movebaseType);
			}
		}
		/**************************************************
		 * @brief Set Eye relative Position
		 * @param type:     Choose which motor or all motors you want to send move cmd
		 * @param angle:    Float data array(size = 6). For chosen motor, the value will work.
		 * @param moveType:     Set move type
		 * @param movebaseType: Set move base Type
		 **************************************************/
		void setRelativePosition(bool *activeFlag, float *angle,
			BE_MovePatternType moveType = enumMovePattern_Saccade,
			BE_MoveBaseType movebaseType = enumMoveBase_Independent)
		{
			BE_setRelativePositionEx(handle_,
				activeFlag,
				angle,
				moveType, movebaseType);
		}
		/**************************************************
		 * @brief Set Eye relative Position
		 * @param type:     Choose which motor or all motors you want to send move cmd
		 * @param angle:    Float data array(size = 6). For chosen motor, the value will work.
		 * @param moveType:     Set move type
		 * @param movebaseType: Set move base Type
		 **************************************************/
		void setRelativePosition(const std::vector<bool> &activeFlag, const std::vector<float> &angle,
			BE_MovePatternType moveType = enumMovePattern_Saccade,
			BE_MoveBaseType movebaseType = enumMoveBase_Independent)
		{
			if (activeFlag.size() >= 6 && angle.size() >= 6)
			{
				bool active_flags[6];
				detail::convertVectorToArray(activeFlag, active_flags);
				BE_setRelativePositionEx(handle_,
					active_flags,
					const_cast<float *>(angle.data()),
					moveType, movebaseType);
			}
		}

		// 空间位置
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
		void setSpaceAbsPosition(float *motorAngle,
			float *motorMoveAngle,
			float *imuAngle,
			BE_MovePatternType moveType = enumMovePattern_Saccade,
			BE_MoveBaseType movebaseType = enumMoveBase_Independent)
		{
			BE_setSpaceAbsPosition(handle_,
				motorAngle,
				motorMoveAngle,
				imuAngle,
				moveType, movebaseType);
		}
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
		void setSpaceAbsPosition(const std::vector<float> &motorAngle,
			const std::vector<float> &motorMoveAngle,
			const std::vector<float> &imuAngle,
			BE_MovePatternType moveType = enumMovePattern_Saccade,
			BE_MoveBaseType movebaseType = enumMoveBase_Independent)
		{
			if (motorAngle.size() >= 6 && motorMoveAngle.size() >= 6 && imuAngle.size() >= 3) {
				BE_setSpaceAbsPosition(handle_,
					const_cast<float *>(motorAngle.data()),
					const_cast<float *>(motorMoveAngle.data()),
					const_cast<float *>(imuAngle.data()),
					moveType, movebaseType);
			}
		}

		// 颈部控制
		/**************************************************
		 * @brief Set neck absolute position (If neck exists)
		 * @note Type(enumNeckAllMotor) is not supported.
		 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
		 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 **************************************************/
		void setAbsolutePosition_Neck(bool *activeFlag, float *angle,
			BE_MovePatternType moveType = enumMovePattern_Saccade)
		{
			BE_setAbsolutePosition_Neck(handle_,
				activeFlag,
				angle,
				moveType);
		}
		/**************************************************
		 * @brief Set neck absolute position (If neck exists)
		 * @note Type(enumNeckAllMotor) is not supported.
		 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
		 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 **************************************************/
		void setAbsolutePosition_Neck(const std::vector<bool> &activeFlag, const std::vector<float> &angle,
			BE_MovePatternType moveType = enumMovePattern_Saccade)
		{
			if (activeFlag.size() >= 3 && angle.size() >= 3) {
				bool active_flags[3];
				detail::convertVectorToArray(activeFlag, active_flags);
				BE_setAbsolutePosition_Neck(handle_,
					active_flags,
					const_cast<float *>(angle.data()),
					moveType);
			}
		}
		/**************************************************
		 * Set neck relative position (If neck exists) (Array Size = 3)
		 * @brief Set neck space eye relative position
		 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
		 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 **************************************************/
		void setRelativePosition_Neck(bool *activeFlag, float *angle,
			BE_MovePatternType moveType = enumMovePattern_Saccade)
		{
			BE_setRelativePosition_Neck(handle_,
				activeFlag,
				angle,
				moveType);
		}
		/**************************************************
		 * Set neck relative position (If neck exists) (Array Size = 3)
		 * @brief Set neck space eye relative position
		 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
		 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 **************************************************/
		void setRelativePosition_Neck(const std::vector<bool> &activeFlag, const std::vector<float> &angle,
			BE_MovePatternType moveType = enumMovePattern_Saccade)
		{
			if (activeFlag.size() >= 3 && angle.size() >= 3)
			{
				bool active_flags[3];
				detail::convertVectorToArray(activeFlag, active_flags);
				BE_setRelativePosition_Neck(handle_,
					active_flags,
					const_cast<float *>(angle.data()),
					moveType);
			}
		}

		// 联合控制
		/**************************************************
		 * @brief Set neck & eye union position （Absolute space）
		 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
		 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 **************************************************/
		void setUnionAbsolutePosition_NeckEye(bool *activeFlag,
			float *eyeAngle,
			float *neckAngle,
			BE_MovePatternType moveType = enumMovePattern_Saccade)
		{
			BE_setUnionAbsolutePosition_NeckEye(handle_,
				activeFlag,
				eyeAngle,
				neckAngle,
				moveType);
		}
		/**************************************************
		 * @brief Set neck & eye union position （Absolute space）
		 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
		 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 **************************************************/
		void setUnionAbsolutePosition_NeckEye(const std::vector<bool> &activeFlag,
			const std::vector<float> &eyeAngle,
			const std::vector<float> &neckAngle,
			BE_MovePatternType moveType = enumMovePattern_Saccade)
		{
			if (activeFlag.size() >= 3 && eyeAngle.size() >= 6 && neckAngle.size() >= 3)
			{
				bool active_flags[3];
				detail::convertVectorToArray(activeFlag, active_flags);
				BE_setUnionAbsolutePosition_NeckEye(handle_,
					active_flags,
					const_cast<float *>(eyeAngle.data()),
					const_cast<float *>(neckAngle.data()),
					moveType);
			}
		}

		/**************************************************
		 * Set neck relative position (If neck exists) (Array Size = 3)
		 * @brief Set neck space eye relative position
		 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
		 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 **************************************************/
		void setUnionRelativePosition_NeckEye(bool *activeFlag,
			float *eyeAngle,
			float *neckAngle,
			BE_MovePatternType moveType = enumMovePattern_Saccade)
		{
			BE_setUnionRelativePosition_NeckEye(handle_,
				activeFlag,
				eyeAngle,
				neckAngle,
				moveType);
		}
		/**************************************************
		 * Set neck relative position (If neck exists) (Array Size = 3)
		 * @brief Set neck space eye relative position
		 * @param activeFlag:   Bool data array(size = 3). Choose which motor or motors you want to send move cmd
		 * @param angle:        Float data array(size = 3). Only for chosen motor, the value will work.
		 * @param moveType:     Set move type
		 **************************************************/
		void setUnionRelativePosition_NeckEye(const std::vector<bool> &activeFlag,
			const std::vector<float> &eyeAngle,
			const std::vector<float> &neckAngle,
			BE_MovePatternType moveType = enumMovePattern_Saccade)
		{
			if (activeFlag.size() >= 3 && eyeAngle.size() >= 6 && neckAngle.size() >= 3)
			{
				bool active_flags[3];
				detail::convertVectorToArray(activeFlag, active_flags);
				BE_setUnionRelativePosition_NeckEye(handle_,
					active_flags,
					const_cast<float *>(eyeAngle.data()),
					const_cast<float *>(neckAngle.data()),
					moveType);
			}
		}

		// 初始位置
		/**************************************************
		 * Set current position as init position command. Current motor value will turn 0 after restart.
		 * @brief Set current position as init position command
		 * @note Current postion will be initial position. Effective after restart.
		 **************************************************/
		void setNowPositionAsInitPos()
		{
			BE_setNowPositionAsInitPos(handle_);
		}
		/**************************************************
		 * @brief Chosen motor or motor will go to init position.
		 * @param type: Set chosen eye motor.
		 **************************************************/
		void goInitPosition(BE_MotorType type)
		{
			BE_goInitPosition(handle_, type);
		}
		/**************************************************
		 * @brief Get chosen motor or motors init position value;
		 * @param type: Set chosen eye motor.
		 * @param angle: return motor initial position value
		 **************************************************/
		std::vector<float> getInitPosition(BE_MotorType type) const
		{
			std::vector<float> angle(6, 0.0f);
			BE_getInitPosition(handle_, type, angle.data());
			return angle;
		}
		/**************************************************
		 * @brief Chosen neck motor or motor will go to init position.
		 * @param type: Set chosen neck motor.
		 **************************************************/
		void goInitPosition_Neck(BE_MotorType_Neck type) {
			BE_goInitPosition_Neck(handle_, type);
		}
		/**************************************************
		 * @brief Get chosen neck motor or motors init position value.
		 * @param type: Set chosen neck motor.
		 * @param angle: return neck motor initial position value
		 **************************************************/
		std::vector<float> getInitPosition_Neck(BE_MotorType_Neck type) const {
			std::vector<float> angle(3, 0.0f);
			BE_getInitPosition_Neck(handle_, type, angle.data());
			return angle;
		}

		// 设备状态
		/**************************************************
		 * @brief return true if neck existed
		 **************************************************/
		bool haveNeckLinked() const
		{
			return BE_haveNeckLinked(handle_);
		}
		/**************************************************
		 * @brief Open or close VOR function
		 * @param vor_eye on/off eye vor
		 * @param vor_neck on/off neck vor
		 **************************************************/
		void onoff_VOR(bool vor_eye = true, bool vor_neck = false)
		{
			BE_onoff_VOR(handle_, vor_eye, vor_neck);
		}
		/**************************************************
		 * @brief Open or close SV function
		 * @param onoff on/off SV function
		 **************************************************/
		void onoff_SV(bool onoff)
		{
			BE_onoff_SV(handle_, onoff);
		}
		/**************************************************
		 * @brief Set sv work mode
		 * @param type: set sv work mode type (see SVType)
		 * @param rect: set work area
		 * @param size: set origin image size
		 **************************************************/
		void setSvModel(BE_SVType type = enumGlobalSV,
			const BE_Rect &be_rect = BE_Rect{ 0, 0, 1920, 1080 },
			const BE_Size &be_size = BE_Size{ 1920, 1080 })
		{
			BE_setSvModel(handle_, type, be_rect, be_size);
		}
		/**************************************************
		 * Set be-device synchronizing signal source
		 * @brief Set be-device synchronizing signal source
		 * @param flag: True: send signal, False: only recv signal
		 **************************************************/
		void setSyncSignalSource(bool flag = true) {
			BE_setSyncSignalSource(handle_, flag);
		}

		// 相机控制
		/**************************************************
		 * @brief Get camera exposure time set range
		 * @param up: return exposure time up_limit
		 * @param down: return exposure time down_limit
		 **************************************************/
		std::pair<float, float> getCameraExposureTimeRange() const
		{
			float up = 0, down = 0;
			BE_getCameraExposureTimeRange(handle_, &up, &down);
			return { up, down };
		}
		/**************************************************
 		* @brief Get camera exposure time
 		* @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
		 **************************************************/
		float getCameraExposureTime(int type) const
		{
			return BE_getCameraExposureTime(handle_, type);
		}
		/**************************************************
		 * @brief Set camera exposure time
		 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
		 * @param autoExposure: True: set auto exposure mode, False: set manual exposure mode
		 * @param exposureTime: set exposure time. Only works when autoExposure = False;
		 **************************************************/
		void setCameraExposure(int type, bool autoExposure, float exposureTime = 0)
		{
			BE_setCameraExposure(handle_, type, autoExposure, exposureTime);
		}
		/**************************************************
		 * @brief Get Camera WhiteBalance Temperature set range std::pair<int, int>
		 * return std::pair<up_limit, down_limit> 
		 **************************************************/
		std::pair<int, int> getCameraWhiteBalanceTemperatureRange() const {
			int up = 0, down = 0;
			BE_getCameraWhiteBalanceTemperatureRange(handle_, &up, &down);
			return { up, down };
		}
		/**************************************************
		 * @brief Get Camera WhiteBalance Temperature
		 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
		 **************************************************/
		float getCameraWhiteBalanceTemperature(int type) const {
			return BE_getCameraWhiteBalanceTemperature(handle_, type);
		}
		/**************************************************
		 * @brief Set Camera WhiteBalance Temperature
		 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
		 * @param autoWhitebalance: True: set whitebalance temperature mode, False: set manual whitebalance temperature mode
		 * @param WhiteBalanceTemperature: set whitebalance temperature. Only works when autoExposure = False;
		 **************************************************/
		void setCameraWhiteBalanceTemperature(int type, bool autoWhitebalance,
			float WhiteBalanceTemperature = 6600)
		{
			BE_setCameraWhiteBalanceTemperature(handle_, type, autoWhitebalance,
				WhiteBalanceTemperature);
		}
		/**************************************************
		 * @brief Get camera exposure auto reference range
		 * @param up: Return up limit
		 * @param down: Return down limit
		 **************************************************/
		std::pair<float, float> getCameraExposureAutoReferenceRange() const
		{
			float up = 0, down = 0;
			BE_getCameraExposureAutoReferenceRange(handle_, &up, &down);
			return { up, down };
		}
		/**************************************************
		 * @brief Set camera exposure auto reference
		 * @note This function only supports some specified BinoSense Device. For example, BinoSense V3 is not
		 * supported.
		 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
		 * @param value: set exposure exposure auto reference value
		 **************************************************/
		void setCameraExposureAutoReference(int type, int value = 70)
		{
			BE_setCameraExposureAutoReference(handle_, type, value);
		}
		/**************************************************
		 * @brief Get camera gain range
		 * @param up: Return up limit
		 * @param down: Return down limit
		 **************************************************/
		std::pair<float, float> getCameraGainRange() const
		{
			float up = 0, down = 0;
			BE_getCameraGainRange(handle_, &up, &down);
			return { up, down };
		}
		/**************************************************
		 * @brief Set camera Gain work mode
		 * @note This function only supports some specified BinoSense Device. For example, BinoSense V3 is not
		 * supported.
		 * @param type: Specify camera to change parameter, see BE_ImageSensorType and CameraIndex  (-1 means all camera!!!)
		 * @param autoGain: Set camera gain set work in auto mode or manually mode.
		 * @param value: set gain value(Only works when autoGain = False)
		 **************************************************/
		void setCameraGain(int type, bool autoGain = true, int value = 370)
		{
			BE_setCameraGain(handle_, type, autoGain, value);
		}

		// 数据保存
		/**************************************************
		 * Start to save all data when calling this function.
		 * @brief Save all be_data
		 * @note If you have chosen high image resolution, note the disk capacity.
		 * You can change saving image size by calling setImageResolution_Transfer
		 * @param startStop: 0, stop; 1, start;
		 * @param folderPath: set data_saving path. If path exists, this function will auto generate a new path
		 * named path_T.
		 **************************************************/
		void saveBeData(bool startStop = true, const std::string &folderPath = "./Be_Io_Path/fastSave")
		{
			BE_saveBeData(handle_, startStop, folderPath.c_str());
		}
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
		bool snapBeData(const std::string &description = "", const std::string &folderPath = "./Be_Io_Path/snapSave")
		{
			return BE_snapBeData(handle_, description.c_str(), folderPath.c_str());
		}
		/**************************************************
		 * Stop snapping be_data when calling this function. May have some time delay(ms).
		 * @brief Stop snapping be_data when calling this function.
		 * @note If you want to change a folder to save snaped data, call this function first
		 * and then recall snapBeData.
		 **************************************************/
		void stopSnapBeData()
		{
			BE_stopSnapBeData(handle_);
		}
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
		bool recordBeData(BE_GeneralData &data, const std::string &description = "",
			const std::string &folderPath = "Be_Io_Path/recordSave")
		{
			return BE_recordBeData(handle_, &data, description.c_str(), folderPath.c_str());
		}
		/**************************************************
		 * Stop recording be_data when calling this function.
		 * @brief Stop recording be_data when calling this function.
		 * @note If you want to change a folder to save recorded data, call this function
		 * first and then recall recordBeData. It's similar to snapBeData & stopSnapBeData.
		 **************************************************/
		void stopRecordBeData()
		{
			BE_stopRecordBeData(handle_);
		}

		// 其他功能
		/**************************************************
		 * Give a trigger signal to data server to send next data(Only for remote connect use,
		 * and parameter dataTransmissionType must be enumDataTransmission_OneByOne)
		 * @brief Give a trigger signal to data server to send a next data
		 **************************************************/
		void triggerDataTransmission()
		{
			BE_triggerDataTransmission(handle_);
		}
		/*! @brief Set grab data rate, Max 200 fps.*/
		void setBeDataRate(int rate = 25)
		{
			BE_setBeDataRate(handle_, rate);
		}
		/*! @brief Set sync camera delay time: max 30000 us.*/
		void setSyncCameraDelayTime(int timeDelay = 0)
		{
			BE_setSyncCameraDelayTime(handle_, timeDelay);
		}

		std::pair<BE_Image, BE_Image> infraImageTrans(BE_GeneralData &data)
		{
			BE_Image infraImage = { 0 };
			BE_Image tempImage = { 0 };
			BE_infraImageTrans(handle_, &data, &infraImage, &tempImage);
			return { infraImage, tempImage };
		}


		//单个像素16位 转温度值
		float infraTempCalc(uint32_t value)
		{
			return BE_infraTempCalc(handle_, value);
		}

		void onOffCameraImage(bool *flag, bool onlyNetworkType = true)
		{
			BE_onOffCameraImage(handle_, flag, onlyNetworkType);
		}
		/**************************************************
		* On/Off camera image grab or network transform. It will help to save system resource or bandwidth resources
		* @brief On/Off camera image grab or network transform
		* @param flag: Bool data array(size = MAXCAMERASInDEVICE). Set 1 means ON and set 0 means OFF
		* @param NetworkType: 1 means only affect network transform, 0 means affect both network and local image grab
		**************************************************/
		void onOffCameraImage(const std::vector<bool> &flag, bool onlyNetworkType = true)
		{
			if (flag.size() > 0)
			{
				bool active_flags[4];
				detail::convertVectorToArray(flag, active_flags);
				BE_onOffCameraImage(handle_, active_flags, onlyNetworkType);
			}
		}


		/**************************************************
		 * @brief Set zoom camera module focal length, range: 1~30
		 **************************************************/
		void setZoomFocalLength(uint8_t focalLength = 1)
		{
			BE_setZoomFocalLength(handle_, focalLength);
		}
		/**************************************************
		* @brief Set wipers module(Only for eagle eye device),0: close ,1~255 :open, different speed
		**************************************************/
		void setWipers(uint8_t value = 0)
		{
			BE_setWipers(handle_, value);
		}
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
		void setExtraModuleFunction(BE_ExtraModuleType type, int para0 = 0, int para1 = 0, int para2 = 0)
		{
			BE_setExtraModuleFunction(handle_, type, para0, para1, para2);
		}
		//设置电机运行速度
		void setMotorPositionSpeed(int saccadeSpeed, int pursuitSpeed)
		{
			BE_setMotorPositionSpeed(handle_, saccadeSpeed, pursuitSpeed);
		}
		//设置深度计算的相关参数
		//onOff   --是否开启
		//svOnOff --是否开启SV
		//precision --返回深度值的精度，1000 对应毫米， 100对应厘米， 10 对应分米， 1 对应米
		//min   有效深度的最小值
		//max   有效深度的最大值
		void setDepthControl(bool onOff = false, bool svOnOff = false, int precision = 1, float min = 200.f, float max = 5000.f)
		{
			BE_setDepthControl(handle_, onOff, svOnOff, precision, min, max);
		}

		//根据指定id返回如下数据：
		// Z 距离数据， 大小short类型640×480大小， 内存由调用侧指定
		// L0LnRT左眼0位置到左眼新位置的旋转平移矩阵 float 6大小的数组 , 内存由调用侧指定
		// KLrect 畸变校正后的左眼内参 9 float大小的数组 ，内存由调用侧指定
		// bgr_l 左眼校正后彩图 内部的内存，也最好调用侧分配 
		bool getDepthInfo(int id, std::vector<uint16_t> &Z, std::vector<float> &L0LnRT, std::vector<float> &KLrect, BE_Image &bgr_l)
		{
			Z.resize(640 * 480);

			L0LnRT.resize(6);
			KLrect.resize(9);

			bool bRet = BE_getDepthInfo(handle_, id, Z.data(), L0LnRT.data(), KLrect.data(), &bgr_l);
			return bRet;
		}
		//获取最新一个深度帧信息，
		// id  返回对应双目原始图像的id
		// Z 距离数据， 大小short类型640×480大小， 内存由调用侧指定
		// L0LnRT左眼0位置到左眼新位置的旋转平移矩阵 float 6大小的数组 , 内存由调用侧指定
		// KLrect 畸变校正后的左眼内参 9 float大小的数组 ，内存由调用侧指定
		// bgr_l 左眼校正后彩图 内部的内存，也最好调用侧分配 
		bool getNewestDepthInfo(int &id, std::vector<uint16_t> &Z, std::vector<float> &L0LnRT, std::vector<float> &KLrect, BE_Image &bgr_l)
		{
			Z.resize(640 * 480);
			L0LnRT.resize(6);
			KLrect.resize(9);

			bool bRet = BE_getNewestDepthInfo(handle_, &id, Z.data(), L0LnRT.data(), KLrect.data(), &bgr_l);
			return bRet;
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

	};

	// 方便的别名
	using BE = BionicEyesWrapper;
	using BEConverter = ImageConverter;

} // namespace bionic_eyes

#endif // BIONIC_EYES_CPP_WRAPPER_H
