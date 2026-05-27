## v2.1.0 2026-04-30

1. 工程部分

- 增加win的支持

- 修正win下查找PCL库时，若使用RelWithDebInfo会找到debug库的问题

- 示例程序部分完善

	- `evo_be_Sample_WithPCL.cpp`改为使用PCLVisualizer来绘制，尝试回避之前使用CloudViewer可能的兼容性问题

- 文件修改为无BOM的UTF-8，解决跨平台的乱码问题

2. 算法部分

暂无

3. 编译、测试情况

- Jetson

    - 编译 & 测试
	
        - 在Orin NX（Jetpack 6.2.1+补丁，L4T 36.4.7）（GCC 11.4.0，libstdc++.so.6最高GLIBCXX_3.4.30）**除了PCL Sample其他都通过**
		
- Ubuntu PC

    - 编译
	
        - 在2004（GCC 9.4.0，libstdc++.so.6最高GLIBCXX_3.4.28）全部通过
		
    - 测试
	
        - 在2004（GCC 9.4.0，libstdc++.so.6最高GLIBCXX_3.4.28）全部通过（新环境，仅安装`cmake-gui`、`build-essential`、`libopencv-dev`、`libxmu-dev`、`libxi-dev`、`freeglut3-dev`、`libpcl-dev`）
		
        - 在2204（GCC 11.4.0，libstdc++.so.6最高GLIBCXX_3.4.30）**除了PCL Sample其他都通过**（新环境，仅安装`cmake-gui`、`build-essential`、`libopencv-dev`、`libxmu-dev`、`libxi-dev`、`freeglut3-dev`、`libpcl-dev`）**2204使用apt安装的PCL可能有兼容性问题**
		
        - 在2404（GCC 13.3.0, libstdc++.so.6最高GLIBCXX_3.4.33）全部通过
		
- Win PC

    - 编译 & 测试
	
		- 在Win 11（VS2022）全部通过


## v2.0.0 2026-04-07

1. 工程部分

- 仿生眼基础模块解决导出符号过多的问题

- 示例程序部分完善

	- 增加OpenGL绘制收到仿生眼设备本地计算的距离的Sample程序`evo_be_Sample_WithOpenGL.cpp`
	
	- 增加PCL绘制收到仿生眼设备本地计算的距离的Sample程序`evo_be_Sample_WithPCL.cpp`

	- 修改sample为不带参数运行时打印使用方法信息
	
	- 调整部分变量名以避免歧义

- 完善文档，修正Ubuntu事先新建文件夹的说明错误问题

2. 算法部分

- 仿生眼基础模块增加远程获取距离信息的功能

- 增加提供OpenGL画点云、控制虚拟相机等功能的辅助模块`evo_openglutils`

3. 编译、测试情况

- Jetson

    - 编译
	
        - 在Orin NX（Jetpack 5.1.1，L4T 35.3.1）（GCC 7.5.0，libstdc++.so.6最高GLIBC3.4.28）全部通过
		
    - 测试
	
        - 在Orin NX（Jetpack 6.2.1+补丁，L4T 36.4.7）（GCC 11.4.0，libstdc++.so.6最高GLIBCXX_3.4.30）**除了PCL Sample其他都通过**
		
- Ubuntu PC

    - 编译
	
        - 在2004（GCC 9.4.0，libstdc++.so.6最高GLIBCXX_3.4.28）全部通过
		
    - 测试
	
        - 在2004（GCC 9.4.0，libstdc++.so.6最高GLIBCXX_3.4.28）全部通过（新环境，仅安装`cmake-gui`、`build-essential`、`libopencv-dev`、`libxmu-dev`、`libxi-dev`、`freeglut3-dev`、`libpcl-dev`）
		
        - 在2204（GCC 11.4.0，libstdc++.so.6最高GLIBCXX_3.4.30）**除了PCL Sample其他都通过**（新环境，仅安装`cmake-gui`、`build-essential`、`libopencv-dev`、`libxmu-dev`、`libxi-dev`、`freeglut3-dev`、`libpcl-dev`）**2204使用apt安装的PCL可能有兼容性问题**
		
        - 在2404（GCC 13.3.0, libstdc++.so.6最高GLIBCXX_3.4.33）全部通过
		

## v1.1.1 2026-03-19

降低cmake最低依赖版本为3.16（没有用CUDA不需要3.18），以对应Ubuntu 2004使用apt安装cmake的情况


## v1.1.0 2026-03-18

1. 从BE_SDK分离出来，作为最小的库，不使用OnnxRuntime、TensorRT进行位姿、深度推理

2. 工程部分

- 去掉了OpenBLAS的依赖

- 去掉了不需要的头文件

- 完善了CMakeLists的写法

- 完善文档，补充了在新电脑配置时需要注意的问题

3. 算法部分

- 增加了线程公用的变量的锁

- 修正很多内存泄露问题

4. 编译、测试情况

- Jetson

    - 编译
	
        - 在Orin NX（Jetpack 5.1.1，L4T 35.3.1）（GCC 7.5.0，libstdc++.so.6最高GLIBC3.4.28）
		
    - 测试
	
        - 在Orin NX（Jetpack 6.2.1+补丁，L4T 36.4.7）（GCC 11.4.0，libstdc++.so.6最高GLIBCXX_3.4.30）通过
		
- Ubuntu PC

    - 编译
	
        - 在2004（GCC 9.4.0，libstdc++.so.6最高GLIBCXX_3.4.28）
		
    - 测试
	
        - 在2004（GCC 7.5.0，libstdc++.so.6最高GLIBCXX_3.4.28）通过（旧环境，手动安装了GCC7）
		
        - 在2004（GCC 9.4.0，libstdc++.so.6最高GLIBCXX_3.4.28）通过（新环境，仅安装`cmake-gui`、`build-essential`、`libopencv-dev`）， **需要手动改`CMakeLists.txt`的`cmake_minimum_required(VERSION 3.16)`，否则apt装的cmake版本不够** 
		
        - 在2204（GCC 11.4.0，libstdc++.so.6最高GLIBCXX_3.4.30）通过（新环境，仅安装`cmake-gui`、`build-essential`、`libopencv-dev`）
		
        - 在2404（GCC 13.3.0, libstdc++.so.6最高GLIBCXX_3.4.33）通过