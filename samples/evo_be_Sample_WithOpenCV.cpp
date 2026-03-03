#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <chrono>

#include <opencv2/opencv.hpp>

#include "bionic_eyes_cpp_wrapper.h"
#ifdef USING_ONNX
#include "evo_beinferposestereo_onnx.h"
#include "evo_bevirtualrectifypipeline_hitnet_onnx.h"
#else
#include "evo_beinferposestereo_trt.h"
#include "evo_bevirtualrectifypipeline_hitnet_trt.h"
#endif

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

#ifdef USING_ONNX
evo::depth::BEVirtualRectifyPipeline_Hitnet_Onnx vrht;
#else
evo::depth::BEVirtualRectifyPipeline_Hitnet_Trt vrht;
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
    std::string hitnet_model_path = "";
    std::string left_model_path = "";
    std::string right_model_path = "";
	evo::INPUT_IMAGE_FORMAT format;	
	float distance_min = 200.0f;
	float distance_max = 5000.0f;
};

bool running = false;
bool saving = false;
bool using_sv = false;
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
unsigned char *p_result_left_ori_rgba = nullptr;
float *p_xyz = nullptr;


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
	
    if (ProgramOptionExists(argc, argv, "--hitnetModelPath"))
        be_fsm.hitnet_model_path = GetProgramOptionAsString(argc, argv, "--hitnetModelPath");
    else if (ProgramOptionExists(argc, argv, "-hit"))
        be_fsm.hitnet_model_path = GetProgramOptionAsString(argc, argv, "-hit");
	
    if (ProgramOptionExists(argc, argv, "--leftModelPath"))
        be_fsm.left_model_path = GetProgramOptionAsString(argc, argv, "--leftModelPath");
    else if (ProgramOptionExists(argc, argv, "-l"))
        be_fsm.left_model_path = GetProgramOptionAsString(argc, argv, "-l");
	
    if (ProgramOptionExists(argc, argv, "--rightModelPath"))
        be_fsm.right_model_path = GetProgramOptionAsString(argc, argv, "--rightModelPath");
    else if (ProgramOptionExists(argc, argv, "-r"))
        be_fsm.right_model_path = GetProgramOptionAsString(argc, argv, "-r");


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
		  -sip --serverIP  \tIP of server device: xxx.xxx.xxx.xxx\n\
		  -hit --hitnetModelPath  \tHitnet model path\n\
		  -l --leftModelPath  \tLeft model path\n\
		  -r --rightModelPath  \tRight model path\n"
	<< std::endl;
	std::cout << "using 1 2 3 4 5 6 7 8 9 0 - = to control motors" << std::endl;
	std::cout << "using s to save images" << std::endl;
	std::cout << "using v to set SV on/off" << std::endl;
};

void handleKey(char key)
{
	switch (key)
    {
    case 27://exit
        running = false;
        break;
	case 'v':
		using_sv = !using_sv;
		std::cout << "using_sv " << using_sv << std::endl;
		vrht.setSVValid(using_sv);
		break;	
	case 's':
		saving = true;
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
			be_fsm.format = evo::INPUT_IMAGE_FORMAT_BGR;
		}
		else
		{	
			w = be_fsm.beData.Image_data[enumBoth].width / 2;
			h = be_fsm.beData.Image_data[enumBoth].height / 1.5;//I420
			be_fsm.format = evo::INPUT_IMAGE_FORMAT_IYUV;		
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
			be_fsm.format = evo::INPUT_IMAGE_FORMAT_BGR;	
		}
		else
		{		
			w = be_fsm.beData.Image_data[enumLeft].width;
			h = be_fsm.beData.Image_data[enumLeft].height / 1.5;//I420
			be_fsm.format = evo::INPUT_IMAGE_FORMAT_IYUV;	
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
	
	//初始化位姿推理模块
#ifdef USING_ONNX
	evo::BEInferPoseStereo_Onnx ips;
#else
	evo::BEInferPoseStereo_Trt ips;
#endif
    ips.init(be_fsm.left_model_path, be_fsm.right_model_path, calib_data.R[6], calib_data.t[6]); 
	
	//初始化深度计算模块（注意0、1的左右定义，是反的）
    vrht.init(be_fsm.hitnet_model_path, w, h, be_fsm.format);
    vrht.setDistanceRange(be_fsm.distance_min, be_fsm.distance_max);
    vrht.setCameraMatrix(calib_data.K[1], calib_data.D[1], calib_data.K[0], calib_data.D[0]);
	
	
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
				imgL_raw = imgL_temp.clone();
				imgR_raw = imgR_temp.clone();
				
				
				//推理外参
				ips.process(be_fsm.beData.motorData);
				float* p_rt_vector = ips.retrieveLR();
				float* p_l0_rt_vector = ips.retrieve0L();
				std::vector<float> lr_r(p_rt_vector, p_rt_vector + 3);//左眼到右眼的旋转向量
				std::vector<float> lr_t(p_rt_vector + 3, p_rt_vector + 6);//左眼到右眼的平移向量
				std::vector<float> l0_r(p_l0_rt_vector, p_l0_rt_vector + 3);//左眼0位置到新位置的旋转向量
				std::vector<float> l0_t(p_l0_rt_vector + 3, p_l0_rt_vector + 6);//左眼0位置到新位置的平移向量
				
							
				//运行深度计算处理的pipeline
				vrht.process(imgL_raw.data, imgR_raw.data, lr_r, lr_t, l0_r, l0_t);

				//获取转回去原角度的左图
				unsigned char* p_result_left_ori = vrht.retrieveRectifiedOriBGR(false);//BGR用于图片显示
				cv::Mat left_ori = cv::Mat(h, w, CV_8UC3, p_result_left_ori);
				//获取转回去原角度的距离z彩图
				unsigned char* p_result_color_ori = vrht.retrieveZOriColor(false);
				cv::Mat result_color_ori = cv::Mat(h, w, CV_8UC4, p_result_color_ori);
				
				
				//处理按键事件保存
				if (saving)
				{
					if (be_fsm.format == evo::INPUT_IMAGE_FORMAT_BGR)
					{
						cv::imwrite("l.png", imgL_raw);
						cv::imwrite("r.png", imgR_raw);					
					}
					else if (be_fsm.format == evo::INPUT_IMAGE_FORMAT_IYUV)
					{
						std::fstream ofile;
						ofile.open("0.data", (std::ios_base::openmode)(std::ios_base::trunc | std::ios_base::out | std::ios_base::binary));
				
						ofile.write((char*)imgL_raw.data, imgL_raw.cols * imgL_raw.rows * imgL_raw.channels());
						ofile.flush();
						ofile.close();
				
						ofile.open("1.data", (std::ios_base::openmode)(std::ios_base::trunc | std::ios_base::out | std::ios_base::binary));
						
						ofile.write((char*)imgR_raw.data, imgR_raw.cols * imgR_raw.rows * imgR_raw.channels());
						ofile.flush();
						ofile.close();
					}
					cv::imwrite("l_ori.png", left_ori);
					cv::imwrite("result_color_ori.png", result_color_ori);
					vrht.savePLYOri("points_ori.ply", false);
					vrht.savePLYZeroPoint("point_0.ply", false);
					
					//计算深度用的矫正后的左右图
					unsigned char* p_left_rectified = vrht.retrieveRectifiedBGR(true, false);
					unsigned char* p_right_rectified = vrht.retrieveRectifiedBGR(false, false);
					cv::Mat left_rectified = cv::Mat(h, w, CV_8UC3, p_left_rectified);
					cv::Mat right_rectified = cv::Mat(h, w, CV_8UC3, p_right_rectified);
					cv::imwrite("left_rectified.png", left_rectified);
					cv::imwrite("right_rectified.png", right_rectified);
					
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
					std::cout << "R " << p_rt_vector[0] << ", " << p_rt_vector[1] << ", " << p_rt_vector[2] << std::endl;
					std::cout << "T " << p_rt_vector[3] << ", " << p_rt_vector[4] << ", " << p_rt_vector[5] << std::endl;
					std::cout << "L R " << p_l0_rt_vector[0] << ", " << p_l0_rt_vector[1] << ", " << p_l0_rt_vector[2] << std::endl;
					std::cout << "L T " << p_l0_rt_vector[3] << ", " << p_l0_rt_vector[4] << ", " << p_l0_rt_vector[5] << std::endl;
				}
						
			
				//计算FPS
				last = now;
				now = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> elapsed = now - last;
				fps = 1.0f / elapsed.count();
				std::ostringstream os;
				os << fps;
				std::string number = os.str();
				cv::putText(result_color_ori, number, cv::Point(20, 60), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 200, 200));
				cv::imshow("result_color_ori", result_color_ori);
				cv::imshow("left_ori", left_ori);
				
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
