//PCL header
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/visualization/cloud_viewer.h>


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
	int precision = 1;
	float distance_min = 200.0f;
	float distance_max = 5000.0f;
};

bool saving = false;
bool printing = false;
bool using_sv = false;
bool right_pitch_flag[6] = {1,0,0,0,0,0};
bool right_roll_flag[6] = {0,1,0,0,0,0};
bool right_yaw_flag[6] = {0,0,1,0,0,0};
bool left_pitch_flag[6] = {0,0,0,1,0,0};
bool left_roll_flag[6] = {0,0,0,0,1,0};
bool left_yaw_flag[6] = {0,0,0,0,0,1};
float motor_targe_value[6] = {0,0,0,0,0,0};
	
BE_FSMState be_fsm;


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
	std::cout << "using 1 2 3 4 5 6 7 8 9 0 - = to control motors" << std::endl;
	std::cout << "using s to save images" << std::endl;
	std::cout << "using p to print L0Ln_RT" << std::endl;
	std::cout << "using v to set SV on/off" << std::endl;
};


//键盘事件回调函数
void keyboardEventOccurred(const pcl::visualization::KeyboardEvent &event, void* viewer_void) {
    pcl::visualization::PCLVisualizer *viewer = static_cast<pcl::visualization::PCLVisualizer *> (viewer_void);

    // 仅在按键弹起时触发（避免按下和弹起各触发一次）
    if (event.keyUp()) {
        std::string key = event.getKeySym();
        std::cout << "key up: " << key << std::endl;

        if (key == "v" || key == "V") {
			using_sv = !using_sv;
			std::cout << "using_sv " << using_sv << std::endl;
			be_fsm.device->setDepthControl(true, using_sv, be_fsm.precision, be_fsm.distance_min, be_fsm.distance_max);
        }
        else if (key == "s" || key == "S") {
            std::cout << "saving" << std::endl;
			saving = true;
        }
        else if (key == "p" || key == "P") {
			printing = true;
		}
        else if (key == "1") {
			motor_targe_value[3]++;
			be_fsm.device->setAbsolutePosition(left_pitch_flag, motor_targe_value);
			std::cout << "set left pitch +1: " << motor_targe_value[3] << std::endl;
		}
        else if (key == "2") {
			motor_targe_value[3]--;
			be_fsm.device->setAbsolutePosition(left_pitch_flag, motor_targe_value);
			std::cout << "set left pitch -1: " << motor_targe_value[3] << std::endl;
		}
        else if (key == "3") {
			motor_targe_value[4]++;
			be_fsm.device->setAbsolutePosition(left_roll_flag, motor_targe_value);
			std::cout << "set left roll +1: " << motor_targe_value[4] << std::endl;
		}
        else if (key == "4") {
			motor_targe_value[4]--;
			be_fsm.device->setAbsolutePosition(left_roll_flag, motor_targe_value);
			std::cout << "set left roll -1: " << motor_targe_value[4] << std::endl;
		}
        else if (key == "5") {
			motor_targe_value[5]++;
			be_fsm.device->setAbsolutePosition(left_yaw_flag, motor_targe_value);
			std::cout << "set left yaw +1: " << motor_targe_value[5] << std::endl;
		}
        else if (key == "6") {
			motor_targe_value[5]--;
			be_fsm.device->setAbsolutePosition(left_yaw_flag, motor_targe_value);
			std::cout << "set left yaw -1: " << motor_targe_value[5] << std::endl;
		}
        else if (key == "7") {
			motor_targe_value[0]++;
			be_fsm.device->setAbsolutePosition(right_pitch_flag, motor_targe_value);
			std::cout << "set right pitch +1: " << motor_targe_value[0] << std::endl;
		}
        else if (key == "8") {
			motor_targe_value[0]--;
			be_fsm.device->setAbsolutePosition(right_pitch_flag, motor_targe_value);
			std::cout << "set right pitch -1: " << motor_targe_value[0] << std::endl;
		}
        else if (key == "9") {
			motor_targe_value[1]++;
			be_fsm.device->setAbsolutePosition(right_roll_flag, motor_targe_value);
			std::cout << "set right roll +1: " << motor_targe_value[1] << std::endl;
		}
        else if (key == "0") {
			motor_targe_value[1]--;
			be_fsm.device->setAbsolutePosition(right_roll_flag, motor_targe_value);
			std::cout << "set right roll -1: " << motor_targe_value[1] << std::endl;
		}
        else if (key == "minus") {
			motor_targe_value[2]++;
			be_fsm.device->setAbsolutePosition(right_yaw_flag, motor_targe_value);
			std::cout << "set right yaw +1: " << motor_targe_value[2] << std::endl;
		}
        else if (key == "equal") {
			motor_targe_value[2]--;
			be_fsm.device->setAbsolutePosition(right_yaw_flag, motor_targe_value);
			std::cout << "set right yaw -1: " << motor_targe_value[2] << std::endl;
		}
    }
}

//保存ply
bool save_ply_ascii(const char* file_name, unsigned char* p_bgra, float* p_point, int number)
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
		temp += std::to_string((int)(p_bgra[4 * i + 2])) + " " + std::to_string((int)(p_bgra[4 * i + 1])) + " " + std::to_string((int)(p_bgra[4 * i])) + " " + std::to_string((int)(p_bgra[4 * i + 3])) + "\r\n";
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

void matrix_multiply(const float R[9], const float vec[3], float result[3])
{
	result[0] = R[0] * vec[0] + R[1] * vec[1] + R[2] * vec[2];
	result[1] = R[3] * vec[0] + R[4] * vec[1] + R[5] * vec[2];
	result[2] = R[6] * vec[0] + R[7] * vec[1] + R[8] * vec[2];
}

void vector_add(const float a[3], const float b[3], float result[3])
{
	result[0] = a[0] + b[0];
	result[1] = a[1] + b[1];
	result[2] = a[2] + b[2];
}

std::vector<std::vector<float>> computeFrustumVertices(
	const float R[9],
	const float T[3],
	const float camera_matrix[9],
	int image_width,
	int image_height,
	float near_plane,
	float far_plane)
{
	std::vector<std::vector<float>> vertices(8, std::vector<float>(3, 0.0f));

	// 提取内参
	float fx = camera_matrix[0];
	float fy = camera_matrix[4];
	float cx = camera_matrix[2];
	float cy = camera_matrix[5];

	// 计算图像四个角点在归一化平面上的坐标（考虑主点偏移）
	// 归一化平面坐标： ( (u - cx)/fx, (v - cy)/fy, 1 )
	float norm_left   = (0 - cx) / fx;      // 左边界 (u=0)
	float norm_right  = (image_width - 1 - cx) / fx;   // 右边界 (u=width-1)
	float norm_top    = (0 - cy) / fy;      // 上边界 (v=0)
	float norm_bottom = (image_height - 1 - cy) / fy;  // 下边界 (v=height-1)

	// 计算近平面和远平面上的四个角点（相机坐标系）
	// 近平面：Z = near_plane
	float near_plane_points[4][3] = {
		{norm_left  * near_plane, norm_top    * near_plane, near_plane},  // 左上
		{norm_right * near_plane, norm_top    * near_plane, near_plane},  // 右上
		{norm_right * near_plane, norm_bottom * near_plane, near_plane},  // 右下
		{norm_left  * near_plane, norm_bottom * near_plane, near_plane}   // 左下
	};

	// 远平面：Z = far_plane
	float far_plane_points[4][3] = {
		{norm_left  * far_plane, norm_top    * far_plane, far_plane},  // 左上
		{norm_right * far_plane, norm_top    * far_plane, far_plane},  // 右上
		{norm_right * far_plane, norm_bottom * far_plane, far_plane},  // 右下
		{norm_left  * far_plane, norm_bottom * far_plane, far_plane}   // 左下
	};

	// 相机坐标系下的8个顶点
	float camera_vertices[8][3];
	for (int i = 0; i < 4; i++) {
		camera_vertices[i][0] = near_plane_points[i][0];
		camera_vertices[i][1] = near_plane_points[i][1];
		camera_vertices[i][2] = near_plane_points[i][2];
		
		camera_vertices[i+4][0] = far_plane_points[i][0];
		camera_vertices[i+4][1] = far_plane_points[i][1];
		camera_vertices[i+4][2] = far_plane_points[i][2];
	}

	// 变换到世界坐标系
	for (int i = 0; i < 8; ++i) {
		float rotated[3];
		matrix_multiply(R, camera_vertices[i], rotated);
		vector_add(rotated, T, vertices[i].data());
	}

	return vertices;
}

void drawFrustum(
	pcl::visualization::PCLVisualizer &viewer,
	const std::vector<std::vector<float>> &vertices,
	const std::string &id_prefix = "frustum",
	double r = 0.0, double g = 0.0, double b = 1.0,
	double line_width = 2.0)
{
	// 定义连接关系（12条边）
	std::vector<std::pair<int, int>> edges = {
		// 近平面四条边
		{0, 1}, {1, 2}, {2, 3}, {3, 0},
		// 远平面四条边
		{4, 5}, {5, 6}, {6, 7}, {7, 4},
		// 连接近远平面的四条边
		{0, 4}, {1, 5}, {2, 6}, {3, 7}
	};

	// 绘制每条边
	for (size_t i = 0; i < edges.size(); ++i) {
		pcl::PointXYZ p1(vertices[edges[i].first][0],
			vertices[edges[i].first][1],
			vertices[edges[i].first][2]);
		pcl::PointXYZ p2(vertices[edges[i].second][0],
			vertices[edges[i].second][1],
			vertices[edges[i].second][2]);

		std::string line_id = id_prefix + "_line_" + std::to_string(i);
		viewer.addLine(p1, p2, r, g, b, line_id);
		viewer.setShapeRenderingProperties(
			pcl::visualization::PCL_VISUALIZER_LINE_WIDTH,
			line_width, line_id);
	}
}

void removeFrustum(
	pcl::visualization::PCLVisualizer &viewer,
	const std::string &id_prefix = "frustum")
{
	std::vector<std::pair<int, int>> edges = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0},
		{4, 5}, {5, 6}, {6, 7}, {7, 4},
		{0, 4}, {1, 5}, {2, 6}, {3, 7}
	};

	for (size_t i = 0; i < edges.size(); ++i) {
		std::string line_id = id_prefix + "_line_" + std::to_string(i);
		viewer.removeShape(line_id);
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
   
	
	//设置窗口可以修改大小
    cv::namedWindow("left 640x480", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
	
	
	//create empty point cloud with smart ptr
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGBA>);

	//create PCL viewer
	boost::shared_ptr<pcl::visualization::CloudViewer> viewer(new pcl::visualization::CloudViewer("Cloud Viewer"));
	
    // 注册键盘回调
    viewer->registerKeyboardCallback(keyboardEventOccurred, (void*)viewer.get());
	
	
	bool first_time = true;
	std::chrono::time_point<std::chrono::high_resolution_clock> now, last;
	float fps;
	int beDataId_last = -1;

	// 用于存储上一帧的RT，判断是否需要更新视锥体
	float prev_L_RT_vec[6] = { 0, 0, 0, 0, 0, 0 };
	
	float* p_l0ln_rt_vector;//左眼0位置到新位置的RT
	
	int w = 640, h = 480;//TODO：初始化大小不该写死
	int id;
	std::vector<uint16_t> Z;//z距离
	std::vector<float> L0Ln_RT(6);//左眼0位置到新位置的RT
	std::vector<float> K0_rectified(9);//去畸变的左眼新内参
	std::vector<float> XYZ(w * h * 3);
	std::vector<float> XYZ0(w * h * 3);
	BE_Image bgr_l = {0};//去畸变的左眼图像
	cv::Mat cv_bgra(h, w, CV_8UC4);
	
	//main loop
	while (!viewer->wasStopped(10))
	{
		//获取深度信息
		if (be_fsm.device->isBeDataReady())
		{
			//获取传输的深度信息
			if (be_fsm.device->getNewestDepthInfo(id, Z, L0Ln_RT, K0_rectified, bgr_l))
			{
				if (bgr_l.width == 0)
					continue;
				
				//转OpenCV Mat
				cv::Mat cv_bgr = ImageConverter::toMat(&bgr_l, false);
				//转成BGRA供PCL点云显示、ply储存
				cv::cvtColor(cv_bgr, cv_bgra, cv::COLOR_BGR2BGRA);
				//将得到当前左眼的深度z转为xyz
				zToXyz(Z.data(), XYZ.data(), w, h, be_fsm.precision, K0_rectified.data());
				//生成0位置初始方向点云
				xyzRotateToZeroPoint(XYZ.data(), XYZ0.data(), w, h, L0Ln_RT.data());

				//处理按键事件保存
				if (saving)
				{
					cv::imwrite("bgr_l.png", cv_bgr);
					save_ply_ascii("point.ply", cv_bgra.data, XYZ.data(), w * h);
					save_ply_ascii("point0.ply", cv_bgra.data, XYZ0.data(), w * h);
					saving = false;
				}
				
				//转为PCL的数据格式，并且将单位从mm改为m（注意PCL显示需要用删了INFINITY的无序点云，这里要把无效点滤掉）
				cloud->clear();
				for (int m = 0; m < w * h; m++) {
					float x = XYZ0[m * 3];
					float y = XYZ0[m * 3 + 1];
					float z = XYZ0[m * 3 + 2];
					if (std::isfinite(x) && std::isfinite(y) && std::isfinite(z)) {
						pcl::PointXYZRGBA p;
						p.x = x / 1000.0f;
						p.y = y / 1000.0f;
						p.z = z / 1000.0f;
						int *color_ptr = (int *)cv_bgra.data + m;	//B-G-R-A
						p.rgba = *color_ptr;
						cloud->push_back(p);	//add point to cloud 
					}
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
				cv::putText(cv_bgr, std::to_string(id), cv::Point(20, 60), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 200, 200));
				cv::imshow("left 640x480", cv_bgr);
			}
			else
				msleep(10);
			
			cv::waitKey(1); 
			
			
			//PCL显示点云
			viewer->showCloud(cloud);
			
			
			// 首次初始化：绘制标系、设置相机视角 
			if (first_time)
			{
				// 设置初始视角
				viewer->runOnVisualizationThreadOnce([&](pcl::visualization::PCLVisualizer &viz) {
					// 设置相机参数
					viz.setCameraPosition(
						0, 0, -4.0,   // 相机位置
						0.0, 0.0, 0.0,   // 焦点位置
						0.0, -1.0, 0.0    // 上方向
					);

					// 设置背景颜色
					viz.setBackgroundColor(0, 0, 0);
					});
				first_time = false;
			}
			
			
			//绘制相机位姿
			bool rt_changed = false;

			// 检查RT向量是否变化
			for (int i = 0; i < 6; i++) {
				if (std::abs(L0Ln_RT[i] - prev_L_RT_vec[i]) > 0.001f) {
					rt_changed = true;
					break;
				}
			}

			// 如果RT发生变化，更新视锥体
			if (rt_changed) {
				viewer->runOnVisualizationThreadOnce([&](pcl::visualization::PCLVisualizer &viz) {
					// 移除旧的视锥体
					removeFrustum(viz, "camera_frustum");

					// 将旋转向量转换为旋转矩阵
					cv::Mat cv_l_r_vec(3, 1, CV_32FC1, L0Ln_RT.data());
					cv::Mat cv_l_r(3, 3, CV_32FC1);
					cv::Rodrigues(cv_l_r_vec, cv_l_r);
					cv::Mat cv_l_r_inv = cv_l_r.t();// 旋转矩阵的逆等于它的转置

					// 平移将单位从mm改为m
					float lnrn_t_m[3] = {
						L0Ln_RT[3] / 1000.0f,
						L0Ln_RT[4] / 1000.0f,
						L0Ln_RT[5] / 1000.0f
					};
					cv::Mat cv_l_t(3, 1, CV_32FC1, lnrn_t_m);
					cv::Mat cv_l_t_inv = -cv_l_r_inv * cv_l_t;// 逆平移 = -R_inv * t_vec

					auto vertices = computeFrustumVertices((float*)cv_l_r_inv.data, (float*)cv_l_t_inv.data, K0_rectified.data(), w, h, be_fsm.distance_min / 1000.0f, be_fsm.distance_max / 1000.0f);

					// 绘制新的视锥体
					drawFrustum(viz, vertices, "camera_frustum", 0.0, 0.0, 1.0, 2.0);

					//std::cout << "FOV frustum updated due to RT change." << std::endl;
					});
			}

			// 更新上一帧的RT（只要RT变了就更新）
			if (rt_changed) {
				for (int i = 0; i < 6; i++) {
					prev_L_RT_vec[i] = L0Ln_RT[i];
				}
			}
		}
	
	}
	
	

    std::cout << "uninit" << std::endl;
	cloud->points.clear();
	if (be_fsm.device != nullptr)
    {
		delete be_fsm.device;
        be_fsm.device = nullptr;
    }
    BE_freeImage(&bgr_l);
		
	
    return 0;
}
