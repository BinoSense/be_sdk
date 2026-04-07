/// Glut
#include "GL/freeglut_std.h"
#include "GL/freeglut_ext.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include "bionic_eyes_cpp_wrapper.h"
#include "evo_openglutils.h"

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

	
//camera control
evo::CameraControlGL* camera_gl = nullptr;
//opengl interop
evo::OpenGLPointCloudInterop* interop_cloud = nullptr;
evo::OpenGLImageInterop* interop_image_left = nullptr;

struct BE_FSMState // Finite state machine
{
    BE_Connect_Type connect_type;
    BE_Connect_DataServerType connect_dataServerType;
    BE_Data_TransmissionType data_transmissionType;
    std::string server_ipAddr = "";
    bionic_eyes::BionicEyesWrapper *device = nullptr;
	int precision = 1;
	float distance_min = 200.0f;
	float distance_max = 5000.0f;
};

bool fullscreen = true;
bool keep_image_ratio = true;
bool running = false;
bool saving = false;
bool printing = false;
bool using_sv = false;//是否使用SV计算
bool showing_pointcloud_0 = false;//显示0位置位置初始朝向点云还是当前左眼相机朝向点云
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

int id;
int w = 640, h = 480;//TODO：初始化大小不该写死
std::vector<uint16_t> Z;//z距离
std::vector<float> L0Ln_RT(6);//左眼0位置到新位置的RT
std::vector<float> K0_rectified(9);//去畸变的左眼新内参
std::vector<float> XYZ(640 * 480 * 3);//TODO：初始化大小不该写死
std::vector<float> XYZ0(640 * 480 * 3);//TODO：初始化大小不该写死
BE_Image bgr_l = {0};//去畸变的左眼图像
cv::Mat cv_rgba(h, w, CV_8UC4);

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
		"[--connect <mode>] [--server <mode>] [--serverIP <ip>]\n\
		options:\n\
		  -c --connect   \tConnect mode: i (image), c (control), ic (image & control)\n\
		  -s --server    \tServer mode: lo (LocalServer_Only), lf (LocalServer_First), do (DeviceServer_Only), df (DeviceServer_First), no (not using net)\n\
		  -sip --serverIP  \tIP of server device: xxx.xxx.xxx.xxx\n"
	<< std::endl;
	std::cout << "using z x to control OpenGL virtual camera zoom" << std::endl;
	std::cout << "using 1 2 3 4 5 6 7 8 9 0 - = to control motors" << std::endl;
	std::cout << "using f to switch full screen" << std::endl;
	std::cout << "using r to switch images keep ratio" << std::endl;
	std::cout << "using s to save images" << std::endl;
	std::cout << "using p to print L0Ln_RT" << std::endl;
	std::cout << "using v to set SV on/off" << std::endl;
	std::cout << "using c to switch painting point cloud using current position/0 position" << std::endl;
};

//mouse press event
void handleMousepress(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            last_x = x;
            last_y = y;
            left_button_pressed = true;
        }
        else
        {
            left_button_pressed = false;
        }
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            last_x = x;
            last_y = y;
            right_button_pressed = true;
        }
        else
        {
            right_button_pressed = false;
        }
    }
}

//mouse move event
void handleMousemove(int x, int y)
{
    if (left_button_pressed)
    {
        camera_gl->setRotation(x - last_x, y - last_y, 0.01f);
    }
    else if (right_button_pressed)
    {
        camera_gl->setTranslation(x - last_x, y - last_y, 1.25f);
    }
    last_x = x;
    last_y = y;
    camera_gl->update();
}

void handleMousewheel(int button, int dir, int x, int y)
{
    if (dir > 0)
    {
        camera_gl->setZoomScale(camera_gl->getZoomScale() * 0.85f);
    }
    else
    {
        camera_gl->setZoomScale(camera_gl->getZoomScale() * 1.15f);
    }
    camera_gl->update();
}

//key press event
void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27://exit
        running = false;
        break;
	case 'f':
		if (fullscreen)
		{
			//Configure Window Postion
			glutInitWindowPosition(0, 0);
			//Configure Window Size
			glutReshapeWindow(w, h);
			fullscreen = false;
		}
		else
		{
			glutFullScreen();
			fullscreen = true;
		}
		break;
	case 'r':
		keep_image_ratio = !keep_image_ratio;
		std::cout << "keep_image_ratio " << keep_image_ratio << std::endl;
		break;
	case 'v':
		using_sv = !using_sv;
		std::cout << "using_sv " << using_sv << std::endl;
		be_fsm.device->setDepthControl(true, using_sv, be_fsm.precision, be_fsm.distance_min, be_fsm.distance_max);
		break;
	case 's':
		saving = true;
		break;
	case 'p':
		printing = true;
		break;
	case 'c':
		showing_pointcloud_0 = !showing_pointcloud_0;
		std::cout << "showing_pointcloud_0 " << showing_pointcloud_0 << std::endl;
		break;
	case 'z':
        camera_gl->setZoomScale(camera_gl->getZoomScale() * 0.85f);
		camera_gl->update();
		break;
	case 'x':
        camera_gl->setZoomScale(camera_gl->getZoomScale() * 1.15f);
		camera_gl->update();
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

//保存ply
bool save_ply_ascii(const char* file_name, unsigned char* p_rgba, float* p_point, int number)
{
	std::fstream fs;
	fs.open(file_name, std::ios::out | std::ios::binary);
	if (!fs.is_open())
	{
		std::cerr << "can not open file: " << file_name << std::endl;
		return false;
	}

	fs << "ply" << std::endl;
	fs << "format ascii 1.0" << std::endl;
	fs << "comment made by evo_utils" << std::endl;

	fs << "element vertex " << number << std::endl;
	fs << "property float x" << std::endl;
	fs << "property float y" << std::endl;
	fs << "property float z" << std::endl;
	fs << "property uchar red" << std::endl;
	fs << "property uchar green" << std::endl;
	fs << "property uchar blue" << std::endl;
	fs << "property uchar alpha" << std::endl;
	fs << "end_header" << std::endl;
	fs.flush();

	for (int i = 0; i < number; i++)
	{
		std::string temp;
		temp += std::to_string(p_point[3 * i]) + " " + std::to_string(p_point[3 * i + 1]) + " " + std::to_string(p_point[3 * i + 2]) + " ";
		temp += std::to_string((int)(p_rgba[4 * i])) + " " + std::to_string((int)(p_rgba[4 * i + 1])) + " " + std::to_string((int)(p_rgba[4 * i + 2])) + " " + std::to_string((int)(p_rgba[4 * i + 3])) + "\r\n";
		fs << temp;
	}
	fs.flush();
	fs.close();

	return true;
}

//z距离转为完整的xyz距离
void zToXyz(const unsigned short *z, float *xyz, int width, int height, int precision, const float k[9])
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = i * width + j;
			int index_xyz = index * 3;
			unsigned short zz = z[index];
			if (zz == std::numeric_limits<unsigned short>::max())
			{
				xyz[index_xyz] = INFINITY;
				xyz[index_xyz + 1] = INFINITY;
				xyz[index_xyz + 2] = INFINITY;
			}
			else
			{
				float zzz = (float)zz / precision;
				xyz[index_xyz] = (j - k[2]) * zzz / k[0];//(x-cx)*z/fx
				xyz[index_xyz + 1] = (i - k[5]) * zzz / k[4];//(y-cy)*z/fy
				xyz[index_xyz + 2] = zzz;
			}
		}
	}
}

//将xyz转回0位置，由于rt是左眼0位置到新位置的RT，所以这里要反着来
void xyzRotateToZeroPoint(const float *xyz, float *xyz0, int width, int height, float *rt)
{
	cv::Mat r_left_vec(3, 1, CV_32FC1, rt);
	cv::Mat t_left_vec(3, 1, CV_32FC1, rt + 3);
	cv::Mat r_left;
	cv::Rodrigues(r_left_vec, r_left);
	
    // 旋转矩阵的逆等于它的转置
    cv::Mat r_left_inv = r_left.t(); 
    // 逆平移 = -R_inv * t_vec
    cv::Mat t_left_vec_inv = -r_left_inv * t_left_vec;
	
	cv::Mat m_34 = cv::Mat::zeros(3, 4, CV_32F);
    r_left_inv.copyTo(m_34(cv::Rect(0, 0, 3, 3)));
    t_left_vec_inv.copyTo(m_34(cv::Rect(3, 0, 1, 3)));
	
	cv::Mat input(1, width * height, CV_32FC3, (void*)xyz);
	cv::Mat output(1, width * height, CV_32FC3, (void*)xyz0);
	
    cv::transform(input, output, m_34);
}

void draw()
{
    int ww = glutGet(GLUT_WINDOW_WIDTH);
    int hh = glutGet(GLUT_WINDOW_HEIGHT);
	
    evo::OpenGLUtils::clear();
	

	//获取深度信息
	if (be_fsm.device->isBeDataReady())
	{
		//获取传输的深度信息
		if (be_fsm.device->getNewestDepthInfo(id, Z, L0Ln_RT, K0_rectified, bgr_l))
		{
			if (bgr_l.width == 0)
				return;
			//转OpenCV Mat
			cv::Mat cv_bgr = ImageConverter::toMat(&bgr_l, false);
			//转成RGBA供OpenGL显示、ply储存
			cv::cvtColor(cv_bgr, cv_rgba, cv::COLOR_BGR2RGBA);
			//将得到当前左眼的深度z转为xyz
			zToXyz(Z.data(), XYZ.data(), w, h, be_fsm.precision, K0_rectified.data());
			//生成0位置初始方向点云
			xyzRotateToZeroPoint(XYZ.data(), XYZ0.data(), w, h, L0Ln_RT.data());

			//处理按键事件保存
			if (saving)
			{
				cv::imwrite("bgr_l.png", cv_bgr);
				save_ply_ascii("point.ply", cv_rgba.data, XYZ.data(), w * h);
				save_ply_ascii("point0.ply", cv_rgba.data, XYZ0.data(), w * h);
				saving = false;
			}
			
			//打印L0Ln_RT
			if (printing)
			{
				for (int i = 0; i < 6; i++)
				{
					std::cout << L0Ln_RT[i] << " ";
				}
				std::cout << std::endl;
			}				
		
			//显示id
			//cv::putText(cv_bgr, std::to_string(id), cv::Point(20, 60), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 200, 200));
			//cv::imshow("left 640x480", cv_bgr);
		}
	}
	else
		msleep(10);
	
	//左右二分窗口，一半画图，一半画点云
    int viewport_w = ww / 2;
    int viewport_h = hh;
	
    //右边画点云
    glViewport(viewport_w, 0, viewport_w, viewport_h);
	
	//控制虚拟相机
	//每帧根据当前视口重新设置投影矩阵
    float hfov = camera_gl->getHorizontalFOV();
    // 使用当前视口的宽高比
    camera_gl->setProjection(hfov, hfov * ((float)viewport_h / viewport_w), camera_gl->getZNear(), camera_gl->getZFar());
    camera_gl->update();
    camera_gl->show();
	
	//画坐标轴
    evo::OpenGLUtils::drawAxes_3d(1000);
		
	//下面展示了使用点云的两种画法，最终在世界坐标系画出来的点云是一样的
	//XYZ：左眼当前相机坐标系->世界坐标系（相机在L0Ln_RT位置）
	//XYZ0：左眼0位置相机坐标系->世界坐标系（相机在原点）
	if (showing_pointcloud_0)
	{
		//画0位置点云
		interop_cloud->draw(cv_rgba.data, XYZ0.data());
		
		//画0位置左眼的FOV
		//evo::OpenGLUtils::drawFOV_3d(K0_rectified[0], K0_rectified[4], K0_rectified[2], K0_rectified[5], be_fsm.distance_min, be_fsm.distance_max, w, h);
	}
	else
	{
		//生成新的model矩阵
		std::vector<float> m_l0ln = evo::OpenGLUtils::buildModelMatrix(L0Ln_RT.data(), L0Ln_RT.data() + 3);//左眼0位置->左眼当前位置
		
		//画当前左眼相机朝向点云（相机坐标系->世界坐标系）
		interop_cloud->draw(cv_rgba.data, XYZ.data(), m_l0ln.data());
		
		//画0位置左眼的FOV
		//evo::OpenGLUtils::drawFOV_3d(K0_rectified[0], K0_rectified[4], K0_rectified[2], K0_rectified[5], be_fsm.distance_min, be_fsm.distance_max, w, h);
		//画当前左眼的FOV
		evo::OpenGLUtils::drawFOV_3d(K0_rectified[0], K0_rectified[4], K0_rectified[2], K0_rectified[5], be_fsm.distance_min, be_fsm.distance_max, w, h, m_l0ln.data(), 1.0f, 
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	}
	
	
	//切回画2D
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//左边画图片
    glViewport(0, 0, viewport_w, viewport_h);
	if (keep_image_ratio)
	{
		interop_image_left->draw(cv_rgba.data, viewport_w, viewport_h);
	}
	else
	{
		interop_image_left->draw(cv_rgba.data);
	}

	

    glutSwapBuffers();

    glutPostRedisplay();
	
    cv::waitKey(1);

    if (!running) {
        glutLeaveMainLoop();
        glutDestroyWindow(1);
    }
}


int main(int argc, char *argv[])
{
	printHelpMessage();
	
    if (argc == 1)
		return 0;

	int res = cmdOptionParser(argc, argv, be_fsm);
    if (res == -1)//本地
	{
		be_fsm.device = new bionic_eyes::BionicEyesWrapper(false);//TODO: 本地目前不支持
	}
	else
	{
		if(res == 1)
			be_fsm.device = new bionic_eyes::BionicEyesWrapper(be_fsm.server_ipAddr, be_fsm.connect_type, be_fsm.connect_dataServerType, be_fsm.data_transmissionType);
		else
			be_fsm.device = new bionic_eyes::BionicEyesWrapper(be_fsm.connect_type, be_fsm.connect_dataServerType, be_fsm.data_transmissionType);
		msleep(5000);
		
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
				msleep(20);
			}
    }
    


    be_fsm.device->onoff_SV(false);
    be_fsm.device->onoff_VOR(false);
	be_fsm.device->goInitPosition(enumAllMotor);
	

	//开启深度信息传输
	be_fsm.device->setDepthControl(true, using_sv, be_fsm.precision, be_fsm.distance_min, be_fsm.distance_max);
	
	
	//init glut
    glutInit(&argc, argv);

    //Setting up The Display
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    //Configure Window Postion
    glutInitWindowPosition(0, 0);

    //Configure Window Size
    glutInitWindowSize(1000, 1000);

    //Create Window
    glutCreateWindow(argv[0]);
    glutFullScreen();

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    //opengl interop init
    interop_cloud = new evo::OpenGLPointCloudInterop();
    interop_cloud->init(w, h, sizeof(unsigned char), sizeof(float), 4);
    interop_image_left = new evo::OpenGLImageInterop();
    interop_image_left->init(w, h, sizeof(unsigned char), 4);

	//opengl vitrual camera init
    camera_gl = new evo::CameraControlGL(0, 0, 0, 0, 0, -1);
    camera_gl->setInitOffsetFromPositionMatrix(0, 0, 4000);
    float hfov = camera_gl->getHorizontalFOV();
    camera_gl->setProjection(hfov, hfov, camera_gl->getZNear(), camera_gl->getZFar());
    camera_gl->update();

    glEnable(GL_DOUBLE);
    glEnable(GL_TEXTURE_2D);

    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(handleMousepress);
    glutMotionFunc(handleMousemove);
    glutMouseWheelFunc(handleMousewheel);

    //Set Draw Loop
    running = true;
    glutDisplayFunc(draw);
    glutMainLoop();
	
	

    std::cout << "uninit" << std::endl;
    if (interop_cloud != nullptr)
    {
        interop_cloud->uninit();
        delete interop_cloud;
        interop_cloud = nullptr;
    }
    if (interop_image_left != nullptr)
    {
        interop_image_left->uninit();
        delete interop_image_left;
        interop_image_left = nullptr;
    }
    if (camera_gl != nullptr)
    {
        delete camera_gl;
        camera_gl = nullptr;
    }
	if (be_fsm.device != nullptr)
    {
		delete be_fsm.device;
        be_fsm.device = nullptr;
    }	
	
    BE_freeImage(&bgr_l);
	
    return 0;
}
