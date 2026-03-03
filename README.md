## 依赖库

在使用SDK前需要先手动安装或准备

### SDK本体依赖

- CUDA 

	- PC
	
		- 使用CUDA 12， https://developer.nvidia.com/cuda-toolkit-archive

	- Jetson
	
		- 和Jetpack版本相关，目前使用CUDA 11

- TensorRT

	- PC
		
		- 使用TensorRT 10， https://developer.nvidia.com/tensorrt/download

		- 直接用，或作为ONNXRuntime的后端使用
		
	- Jetson
	
		- 和Jetpack版本相关，TensorRT 8
	
	- 直接用的时候，需要自己先通过手动执行`trtexec`生成.engine文件（注意.engine平台是绑定硬件的，需要在实际使用的平台上生成）

- ONNXRuntime（可选）

	- PC
	
		- 目前使用1.23.2，注意下载文件名带gpu的版本， https://github.com/microsoft/onnxruntime/releases/tag/v1.23.2
		
		- 依赖CUDNN 9， https://developer.nvidia.com/cudnn-downloads

	- Jetson
	
		- 目前不支持
	
	- 使用TensorRT作为首选后端。没有TensorRT会使用CUDA作为后端，速度会慢一些


### Sample依赖（SDK本身不依赖）

- OpenCV（各Sample用）

- GLUT（OpenGL Sample用）

	- Ubuntu下安装`libxmu-dev`、`libxi-dev`、`freeglut3-dev`、`libglew-dev`
	
	- Win下使用dependencies里提供的freeglut

- PCL（PCL Sample用）


## SDK文件介绍

主要模块（核心部分）的处理逻辑为**获取原始数据->计算双目位姿->深度计算**

同时也提供一些辅助模块，用于方便的实现如OpenGL的点云绘制等

### 主要模块

1. 仿生眼基础模块

	获取原始数据（图片、电机编码器值等）、控制眼睛运动、控制相机参数等

	- `bionic_eyes_cpp_wrapper.h`

2. 双目位姿计算模块

	编码器值->双目位姿。下列模块选一个用即可

	- `evo_beinferposestereo_onnx.h`
	
		使用ONNX进行位姿推理
	
	- `evo_beinferposestereo_trt.h`
	
		使用TensorRT进行位姿推理

3. 深度计算模块

	原始图片+内参+位姿->深度信息。下列模块选一个用即可

	- `evo_bevirtualrectifypipeline_hitnet_onnx.h`
	
		使用Hitnet ONNX进行位姿推理
	
	- `evo_bevirtualrectifypipeline_hitnet_trt.h`
	
		使用Hitnet TensorRT进行位姿推理
		
	- `evo_bevirtualrectifypipeline_foundationstereo_onnx.h`
	
		使用FoundationStereo ONNX进行位姿推理
	
	- `evo_bevirtualrectifypipeline_foundationstereo_trt.h`
	
		使用FoundationStereo TensorRT进行位姿推理
	
	- `evo_bevirtualrectifypipeline_ess_trt.h`
	
		使用ESS TensorRT进行位姿推理（仅Jetson平台）

### 辅助模块

1. OpenGL辅助模块（图片及点云的绘制、鼠标控制虚拟镜头等）

	- `evo_openglutils.h`
	
2. OpenGL辅助模块CUDA版本（图片及点云的绘制等）

	- `evo_openglutils_cuda.h`
	
### Sample程序

1. OpenCV例子

	`evo_be_Sample_WithOpenCV.cpp`
	
	仅结合OpenCV，展示如何使用OpenCV来显示图片、距离彩图等
	
	其中主要展示了如何完成一个最简单的完整的**获取原始数据->计算双目位姿->深度计算**的全过程（包括使用`mat_converter.hpp`辅助头文件，将数据转为OpenCV的Mat）
	
2. OpenGL例子

	`evo_be_Sample_WithOpenGL.cpp`
	
	结合OpenCV/OpenGL，展示如何使用OpenGL来显示点云，同时使用OpenCV来显示图片、距离彩图等
	
	其中主要展示了如何使用OpenGL绘制有序点云（使用到了辅助模块`evo_openglutils`和`evo_openglutils_cuda`来实现OpenGL部分显示及操作功能）

3. PCL例子

	`evo_be_Sample_WithPCL.cpp`
	
	结合OpenCV/PCL，展示如何使用PCL来显示点云，同时使用OpenCV来显示图片、距离彩图等
	
	其中主要展示了如何将结果转为PCL需要的无序点云并使用PCL绘制
	
	
### CMake配置

1. 如果要使用ONNXRuntime，修改`ONNXRuntime_ROOT_DIR`指定到ONNXRuntime的根目录即可

2. 修改`TensorRT_DIR`指定到TensorRT的根目录

3. 修改`OpenCV_DIR`指定到`OpenCVConfig.cmake`所在的目录，通常是OpenCV的`\lib\cmake\opencv4`


## 注意点

1. 如果使用ONNXRuntime，第一次运行时，会自动先去生成.engine文件，所以会比较慢

2. 使用ONNXRuntime目前在ubuntu需要配套使用SDK编译时的版本，否则会找不到符号
	
	具体依赖的版本号，可以在sample程序运行时报错信息看到

3. 由于部分模块命名比较相似（比如仅使用后缀_onnx和_trt来区分使用的推理引擎），加载头文件和link的库注意要对应