## 目录结构

be_sdk_mini/

├── cmake/			cmake规则

├── dependencies/	示例代码的依赖库

├── include/		头文件

│   ├── platform/	不同平台的头文件

├── lib/			库文件

│   ├── linux/		Ubuntu PC库（包括be_sdk库文件）

│   ├── tegra/		Jetson库（包括be_sdk库文件）

│   └── win/		Win PC库（包括be_sdk库文件）

├── samples/		示例代码

├── CMakeLists.txt

└── README.md


## Sample依赖库（SDK本身不依赖）

在使用前需要先手动安装或准备

- OpenCV（各Sample用）

	- Ubuntu下可apt安装`libopencv-dev`，或使用自己编译的版本
	
	- Win下可以用下载的binary，或使用自己编译的版本

- GLUT（OpenGL Sample用）

	- Ubuntu下apt安装`libxmu-dev`、`libxi-dev`、`freeglut3-dev`
	
	- Win下使用dependencies目录里提供的freeglut

- PCL（PCL Sample用）

	- Ubuntu下安装`libpcl-dev`，或使用自己编译的版本
	
		**注意2204使用apt安装的PCL可能有兼容性问题**
	
	- Win下去PCL的Release页面下载 https://github.com/PointCloudLibrary/pcl/releases
	
		**注意用的VS版本不能低于预编译的PCL的VS版本**


## SDK文件介绍

### 主要模块

1. 仿生眼基础模块

	获取原始数据（图片、电机编码器值等）、控制眼睛运动、控制相机参数等

	- `bionic_eyes_cpp_wrapper.h`

### 辅助模块

1. OpenGL辅助模块（图片及点云的绘制、鼠标控制虚拟镜头等）

	- `evo_openglutils.h`
	
### Sample程序

不带参数运行，就会打印参数的含义和使用方法

默认可以在仿生眼设备本地实时进行深度计算，通讯直接获取到。**仿生眼设备本地计算的深度由于本地算力和传输大小限制，会进行缩图**

1. OpenCV最简例子

	`evo_be_Sample_WithOpenCV_simple.cpp`
	
	仅结合OpenCV，最简单的例子，展示如何使用OpenCV来显示图片、打印编码器值等
	
	其中主要展示了如何完成一个最简单的完整的仅获取数据的过程（包括使用`mat_converter.hpp`辅助头文件，将数据转为OpenCV的Mat）
	
2. OpenGL显示收到的点云数据例子

	`evo_be_Sample_WithOpenGL.cpp`
	
	结合OpenGL/OpenCV，展示如何使用OpenCV来显示图片，同时使用OpenGL来显示点云等。
	
	其中距离数据使用的是在仿生眼设备本地计算得到的结果，将通信得到的z距离数据还原为xyz距离后显示（使用到了辅助模块`evo_openglutils`来实现OpenGL部分显示及操作功能）
	
3. PCL显示收到的点云数据例子

	`evo_be_Sample_WithPCL.cpp`
	
	结合OpenCV/PCL，展示如何使用OpenCV来显示图片，同时使用PCL来显示点云等
	
	其中距离数据使用的是在仿生眼设备本地计算得到的结果，将通信得到的z距离数据还原为xyz距离，再转为无序点云后显示
	
	
### CMake配置

- 修改`OpenCV_DIR`指定到`OpenCVConfig.cmake`所在的目录，通常是OpenCV的`/lib/cmake/opencv4`

- GLUT和PCL会自动去找，找不到的话自己手动修改


## 常见问题

1. 临时目录问题

- Ubuntu的话，在运行之前，需要确保有`/usr/Evo_BionicEyes`目录，然后确保有可读写的权限，然后还要有`/usr/Evo_BionicEyes/tmp_path`目录

```
sudo mkdir /usr/Evo_BionicEyes
sudo chmod 777 /usr/Evo_BionicEyes/
mkdir /usr/Evo_BionicEyes/tmp_path
```

- Win的话，在运行之前，需要确保有`C:/Evo_BionicEyes`目录，然后还要有`C:/Evo_BionicEyes/tmp_path`目录

2. OpenGL的sample不使用NV独立显卡而使用集成显卡导致无法正常运行的问题

- Ubuntu的话，在console先执行

```
export __NV_PRIME_RENDER_OFFLOAD=1
export __GLX_VENDOR_LIBRARY_NAME=nvidia
```

然后再运行程序

- Win的话，设置->系统->屏幕->显示卡->应用程序的自定义设置

然后添加应用，将程序加进去，GPU首选项选成NV独立显卡

3. win下编译完成后，依赖的dll文件需要自己手动复制到编译生成的程序的目录才能运行（如cmake设置了build目录，使用Release方式，那通常目录就是build/Release）