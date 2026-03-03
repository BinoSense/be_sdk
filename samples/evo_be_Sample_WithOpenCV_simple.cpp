#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>

#include <opencv2/opencv.hpp>

#include "bionic_eyes_cpp_wrapper.h"

#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#define msleep(ms) usleep(1000 * (ms))

#else // for windows
#include <windows.h>
#include <time.h>

#define msleep(ms) Sleep(ms)
#endif



struct BE_FSMState // Finite state machine
{
    BE_Connect_Type connect_type;
    BE_Connect_DataServerType connect_dataServerType;
    BE_Data_TransmissionType data_transmissionType;
    std::string server_ipAddr = "";
    bionic_eyes::BionicEyesWrapper *device = nullptr;
    BE_GeneralData beData;
    double scale = 1;
	bool is_bgr;	
};

bool running = false;
bool saving = false;
bool printing = false;
bool right_pitch_flag[6] = {1,0,0,0,0,0};
bool right_roll_flag[6] = {0,1,0,0,0,0};
bool right_yaw_flag[6] = {0,0,1,0,0,0};
bool left_pitch_flag[6] = {0,0,0,1,0,0};
bool left_roll_flag[6] = {0,0,0,0,1,0};
bool left_yaw_flag[6] = {0,0,0,0,0,1};
float motor_targe_value[6] = {0,0,0,0,0,0};
	
BE_FSMState be_fsm;

bool left_button_pressed = false;
bool right_button_pressed = false;
int last_x, last_y;

std::chrono::time_point<std::chrono::high_resolution_clock> now, last;
float fps;
int beDataId_last = -1;
int w, h;//image width/height
bionic_eyes::BE_CalibData_w calib_data;


bool ProgramOptionExists(int argc, char **argv, const std::string &option)
{
	return std::find(argv, argv + argc, option) != argv + argc;
}

std::string GetProgramOptionAsString(
        int argc,
        char **argv,
        const std::string &option)
{
	char **itr = std::find(argv, argv + argc, option);
	if (itr != argv + argc && ++itr != argv + argc)
	{
		return std::string(*itr);
	}
	return "";
}

int cmdOptionParser(int argc, char *argv[], BE_FSMState &be_fsm)
{
    be_fsm.connect_type = enumConnect_ImageControl;
    be_fsm.connect_dataServerType = enumLocalServer_First;
    be_fsm.data_transmissionType = enumDataTransmission_ASPAP;
    if (ProgramOptionExists(argc, argv, "-t"))
        be_fsm.data_transmissionType = enumDataTransmission_OneByOne;

    std::string option;
	
    if (ProgramOptionExists(argc, argv, "--scale"))
        be_fsm.scale = std::stof(GetProgramOptionAsString(argc, argv, "--scale"));

    if (ProgramOptionExists(argc, argv, "--connect"))
        option = GetProgramOptionAsString(argc, argv, "--connect");
    else if (ProgramOptionExists(argc, argv, "-c"))
        option = GetProgramOptionAsString(argc, argv, "-c");

    if (option == "c")
        be_fsm.connect_type = enumConnect_Control;
    else if (option == "ic" || option == "ci")
        be_fsm.connect_type = enumConnect_ImageControl;
    else if (option == "i")
        be_fsm.connect_type = enumConnect_Image;

    if (ProgramOptionExists(argc, argv, "--server"))
        option = GetProgramOptionAsString(argc, argv, "--server");
    else if (ProgramOptionExists(argc, argv, "-s"))
        option = GetProgramOptionAsString(argc, argv, "-s");

    if (option == "lo")
        be_fsm.connect_dataServerType = enumLocalServer_Only;
    else if (option == "lf")
        be_fsm.connect_dataServerType = enumLocalServer_First;
    else if (option == "do")
        be_fsm.connect_dataServerType = enumDeviceServer_Only;
    else if (option == "df")
        be_fsm.connect_dataServerType = enumDeviceServer_First;
    else if (option == "no")//本地
        return -1;

    if (ProgramOptionExists(argc, argv, "--serverIP"))
        be_fsm.server_ipAddr = GetProgramOptionAsString(argc, argv, "--serverIP");
    else if (ProgramOptionExists(argc, argv, "-sip"))
        be_fsm.server_ipAddr = GetProgramOptionAsString(argc, argv, "-sip");
    else
        return 0;
	
    std::string pingCMD = "ping -c1 -s1 ";
    pingCMD = pingCMD + be_fsm.server_ipAddr;
    pingCMD = pingCMD + " > /dev/null 2>&1";
    return !(bool)system(pingCMD.c_str());
}

void printHelpMessage()
{
	std::cout << "usage: " <<  
		"[--connect <mode>] [--scale <value>] [--server <mode>] [--serverIP <ip>] [--hitnetModelPath <path>] [--leftModelPath <path>] [--rightModelPath <path>]\n\
		options:\n\
		  -h --help      \tShow this help message\n\
		  --scale		\tScale of image (NOT support local)\n\
		  -c --connect   \tConnect mode: i (image), c (control), ic (image & control)\n\
		  -s --server    \tServer mode: lo (LocalServer_Only), lf (LocalServer_First), do (DeviceServer_Only), df (DeviceServer_First), no (not using net)\n\
		  -sip --serverIP  \tIP of server device: xxx.xxx.xxx.xxx\n"
	<< std::endl;
	std::cout << "using 1 2 3 4 5 6 7 8 9 0 - = to control motors" << std::endl;
	std::cout << "using s to save images" << std::endl;
	std::cout << "using p to print encoder values" << std::endl;
};

void handleKey(char key)
{
	switch (key)
    {
    case 27://exit
        running = false;
        break;
	case 's':
		saving = true;
		break;
	case 'p':
		printing = true;
		break;
	case '1':
		motor_targe_value[3]++;
		be_fsm.device->setAbsolutePosition(left_pitch_flag, motor_targe_value);
		std::cout << "set left pitch +1: " << motor_targe_value[3] << std::endl;
		break;
	case '2':
		motor_targe_value[3]--;
		be_fsm.device->setAbsolutePosition(left_pitch_flag, motor_targe_value);
		std::cout << "set left pitch -1: " << motor_targe_value[3] << std::endl;
		break;
	case '3':
		motor_targe_value[4]++;
		be_fsm.device->setAbsolutePosition(left_roll_flag, motor_targe_value);
		std::cout << "set left roll +1: " << motor_targe_value[4] << std::endl;
		break;
	case '4':
		motor_targe_value[4]--;
		be_fsm.device->setAbsolutePosition(left_roll_flag, motor_targe_value);
		std::cout << "set left roll -1: " << motor_targe_value[4] << std::endl;
		break;
	case '5':
		motor_targe_value[5]++;
		be_fsm.device->setAbsolutePosition(left_yaw_flag, motor_targe_value);
		std::cout << "set yaw pitch +1: " << motor_targe_value[5] << std::endl;
		break;
	case '6':
		motor_targe_value[5]--;
		be_fsm.device->setAbsolutePosition(left_yaw_flag, motor_targe_value);
		std::cout << "set yaw pitch -1: " << motor_targe_value[5] << std::endl;
		break;
	case '7':
		motor_targe_value[0]++;
		be_fsm.device->setAbsolutePosition(right_pitch_flag, motor_targe_value);
		std::cout << "set right pitch +1: " << motor_targe_value[0] << std::endl;
		break;
	case '8':
		motor_targe_value[0]--;
		be_fsm.device->setAbsolutePosition(right_pitch_flag, motor_targe_value);
		std::cout << "set right pitch -1: " << motor_targe_value[0] << std::endl;
		break;
	case '9':
		motor_targe_value[1]++;
		be_fsm.device->setAbsolutePosition(right_roll_flag, motor_targe_value);
		std::cout << "set right roll +1: " << motor_targe_value[1] << std::endl;
		break;
	case '0':
		motor_targe_value[1]--;
		be_fsm.device->setAbsolutePosition(right_roll_flag, motor_targe_value);
		std::cout << "set right roll -1: " << motor_targe_value[1] << std::endl;
		break;
	case '-':
		motor_targe_value[2]++;
		be_fsm.device->setAbsolutePosition(right_yaw_flag, motor_targe_value);
		std::cout << "set right pitch +1: " << motor_targe_value[2] << std::endl;
		break;
	case '=':
		motor_targe_value[2]--;
		be_fsm.device->setAbsolutePosition(right_yaw_flag, motor_targe_value);
		std::cout << "set right pitch -1: " << motor_targe_value[2] << std::endl;
		break;
	default:
		break;
    }
}


int main(int argc, char *argv[])
{
    if (ProgramOptionExists(argc, argv, "--help") || ProgramOptionExists(argc, argv, "-h"))
    {
        printHelpMessage();
        std::exit(EXIT_FAILURE);
    }

	int res = cmdOptionParser(argc, argv, be_fsm);
	
    if (res == -1)//本地
	{
		be_fsm.device = new bionic_eyes::BionicEyesWrapper(false);
		be_fsm.scale = 1;//TODO: 本地目前不支持分辨率修改
	}
	else
	{
		if(res == 1)
			be_fsm.device = new bionic_eyes::BionicEyesWrapper(be_fsm.server_ipAddr, be_fsm.connect_type, be_fsm.connect_dataServerType, be_fsm.data_transmissionType);
		else
			be_fsm.device = new bionic_eyes::BionicEyesWrapper(be_fsm.connect_type, be_fsm.connect_dataServerType, be_fsm.data_transmissionType);
		msleep(5000);
		
		be_fsm.device->setImageColor_Transfer(enumColor);
		be_fsm.device->setImageColor(enumColor);
		auto originalSize = be_fsm.device->getOriginImageResolution();
		be_fsm.device->setImageResolution_Transfer(originalSize.first * be_fsm.scale, originalSize.second * be_fsm.scale);
		be_fsm.device->setImageResolution(originalSize.first * be_fsm.scale, originalSize.second * be_fsm.scale);
		be_fsm.device->setImageCompressionQuality(95);
		be_fsm.device->setDataRate_Transfer(25);
		msleep(250);
		while (!(be_fsm.device->isBeDataReady()))
		{
			msleep(40);
			std::cout << "Waiting for data..." << std::endl;
			continue;
		}
	   // be_fsm.device->setAbsolutePosition(enumAllMotor, std::vector<float>(6, 0.0f).data());
		if (be_fsm.data_transmissionType == enumDataTransmission_OneByOne)
			for (int k = 0; k < 5; k++)
			{
				be_fsm.device->triggerDataTransmission();
				be_fsm.device->getBeData(be_fsm.beData);
				msleep(20);
			}

		be_fsm.device->getBeData(be_fsm.beData);
    }


    be_fsm.device->onoff_SV(false);
    be_fsm.device->onoff_VOR(false);
	be_fsm.device->goInitPosition(enumAllMotor);
	be_fsm.device->getBeData(be_fsm.beData);
	
	int w, h;//image width/height
	if (be_fsm.beData.Image_data[enumBoth].width > 0)//sbs
	{
		const int channels = be_fsm.beData.Image_data[enumBoth].channels;
		if (channels == 3)
		{
			w = be_fsm.beData.Image_data[enumBoth].width / 2;
			h = be_fsm.beData.Image_data[enumBoth].height;
			be_fsm.is_bgr = true;
		}
		else
		{	
			w = be_fsm.beData.Image_data[enumBoth].width / 2;
			h = be_fsm.beData.Image_data[enumBoth].height / 1.5;//I420
			be_fsm.is_bgr = false;		
		}
		std::cout << "sbs, image width: " << w << ", height:" << h << ", channels:" << channels << std::endl;
	}
	else//left&right
	{
		const int channels = be_fsm.beData.Image_data[enumLeft].channels;
		if (channels == 3)
		{
			w = be_fsm.beData.Image_data[enumLeft].width;
			h = be_fsm.beData.Image_data[enumLeft].height;
			be_fsm.is_bgr = true;	
		}
		else
		{		
			w = be_fsm.beData.Image_data[enumLeft].width;
			h = be_fsm.beData.Image_data[enumLeft].height / 1.5;//I420
			be_fsm.is_bgr = false;	
		}
		std::cout << "left/right, image width: " << w << ", height:" << h << std::endl;
	}

	//获取内参及0位外参
	bionic_eyes::BE_CalibData_w calib_data = be_fsm.device->getCalibrationInfo();
    //内参对应设置的scale的变化
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			calib_data.K[j][i] *= be_fsm.scale;
		}
	}
	
	
	std::chrono::time_point<std::chrono::high_resolution_clock> now, last;
	float fps;
	int beDataId_last = -1;
	running = true;

	//main loop
	while (running)
	{
		be_fsm.device->getBeData(be_fsm.beData);
		if (!be_fsm.beData.isMovingFastly)
		{
			if (be_fsm.beData.frame_id != beDataId_last)
			{
				//取左右原图，如果是sbs的则分成左右
				cv::Mat imgL_temp, imgR_temp;
				cv::Mat imgL_raw, imgR_raw;
				if (be_fsm.beData.Image_data[enumBoth].width > 0)//sbs
				{
					cv::Mat Org = ImageConverter::toMat(&be_fsm.beData.Image_data[enumBoth], false);
					imgL_temp = Org.colRange(0, w);
					imgR_temp = Org.colRange(w, 2 * w);
				}
				else
				{
					imgL_temp = ImageConverter::toMat(&be_fsm.beData.Image_data[enumLeft], false);
					imgR_temp = ImageConverter::toMat(&be_fsm.beData.Image_data[enumRight], false);
				}
				if (be_fsm.is_bgr)
				{
					imgL_raw = imgL_temp.clone();
					imgR_raw = imgR_temp.clone();
				}
				else
				{
					cv::cvtColor(imgL_temp, imgL_raw, cv::COLOR_YUV2BGR_I420);
					cv::cvtColor(imgR_temp, imgR_raw, cv::COLOR_YUV2BGR_I420);
				}
				
				
				//打印编码器值
				if (printing)
				{
					for (int i = 0; i < 6; i++)
					{
						std::cout << be_fsm.beData.motorData[i] << ", ";
					}
					std::cout << std::endl;
				}
				
				//处理按键事件保存
				if (saving)
				{
					cv::imwrite("l.png", imgL_raw);
					cv::imwrite("r.png", imgR_raw);	
					saving = false;
					
					std::cout << "K left ";
					for (int i = 0; i < calib_data.K[1].size(); i++)
					{
						std::cout << calib_data.K[1][i] << ", ";
					}
					std::cout << std::endl;					
					std::cout << "D left ";
					for (int i = 0; i < calib_data.D[1].size(); i++)
					{
						std::cout << calib_data.D[1][i] << ", ";
					}
					std::cout << std::endl;
					std::cout << "K right ";
					for (int i = 0; i < calib_data.K[0].size(); i++)
					{
						std::cout << calib_data.K[0][i] << ", ";
					}
					std::cout << std::endl;					
					std::cout << "D right ";
					for (int i = 0; i < calib_data.D[0].size(); i++)
					{
						std::cout << calib_data.D[0][i] << ", ";
					}
					std::cout << std::endl;
					std::cout << "0 R ";
					for (int i = 0; i < calib_data.R[6].size(); i++)
					{
						std::cout << calib_data.R[6][i] << ", ";
					}
					std::cout << std::endl;					
					std::cout << "0 T ";
					for (int i = 0; i < calib_data.t[6].size(); i++)
					{
						std::cout << calib_data.t[6][i] << ", ";
					}
					std::cout << std::endl;
				}
						
			
				//计算FPS
				last = now;
				now = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> elapsed = now - last;
				fps = 1.0f / elapsed.count();
				std::ostringstream os;
				os << fps;
				std::string number = os.str();
				cv::putText(imgL_raw, number, cv::Point(20, 60), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 200, 200));
				cv::imshow("imgL", imgL_raw);
				cv::imshow("imgR", imgR_raw);
				
				beDataId_last = be_fsm.beData.frame_id;
			}
			else
				msleep(10);
			
			//handle key press event
			handleKey((char)cv::waitKey(10));
		}
	
	}
	
	

    std::cout << "uninit" << std::endl;
	if (be_fsm.device != nullptr)
		delete be_fsm.device;
    BE_freeImage(&be_fsm.beData.Image_data[0]);
    BE_freeImage(&be_fsm.beData.Image_data[1]);
    BE_freeImage(&be_fsm.beData.Image_data[2]);
    BE_freeImage(&be_fsm.beData.Image_data[3]);
	
	
	
	
    return 0;
}
