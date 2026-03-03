cmake_minimum_required(VERSION 3.18)
project(BE_SDK_Sample)

set(PROJECT_PATH                           "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_PATH}/cmake/Modules")
set(DEPENDENCIES_PATH                      "${PROJECT_PATH}/dependencies")
set(BE_SDK_INCLUDE_DIRS                    "${PROJECT_PATH}/include")
set(LIB_PATH                               "${PROJECT_PATH}/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -Wl,-rpath=. ")
set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo;Release" CACHE STRING "" FORCE )

#compile simple sample(ONLY read data) or full sample(calculate pose and depth)
option(BUILD_SIMPLE_SAMPLE_ONLY "only build simple sample" ON)

if (NOT BUILD_SIMPLE_SAMPLE_ONLY)
	#3rd party libraries
	enable_language(CUDA)
	find_package(CUDAToolkit REQUIRED)

	find_package(ONNXRuntime) #no need if using TensorRT only
	find_package(TensorRT REQUIRED)

	option(using_onnx "use ONNXRuntime instead of TensorRT" OFF)
	set(INFER_LIBRARY_NAME "trt")
	if(ONNXRuntime_FOUND)
		if(using_onnx)
			set(INFER_LIBRARY_NAME "onnx")
			add_definitions(-DUSING_ONNX)
			message(STATUS "using ONNXRuntime")
		endif(using_onnx)
	endif(ONNXRuntime_FOUND)
endif(BUILD_SIMPLE_SAMPLE_ONLY)

include_directories(${BE_SDK_INCLUDE_DIRS})

#BE_SDK_BASE_LIBRARIES: base libraries to get data and control the device
#BE_SDK_EXTRA_LIBRARIES: extra libraries to calculate pose and depth
#BE_SDK_OPENGL_UTILS_LIBRARIES: support libraries to run with OpenGL
if(WIN32)
	set(BE_SDK_BASE_LIBRARIES
		${LIB_PATH}/win/bionic_eyes_c.lib
		)
	set(BE_SDK_EXTRA_LIBRARIES
		${LIB_PATH}/win/evo_beinferposestereo_${INFER_LIBRARY_NAME}64.lib 
		${LIB_PATH}/win/evo_bevirtualrectifypipeline_hitnet_${INFER_LIBRARY_NAME}64.lib 
		)
	set(BE_SDK_OPENGL_UTILS_LIBRARIES
		${LIB_PATH}/win/evo_openglutils64.lib 
		${LIB_PATH}/win/evo_openglutils_cuda64.lib 
		)
else(WIN32)
	if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
		set(BE_SDK_BASE_LIBRARIES
			${LIB_PATH}/tegra/libbionic_eyes_c.so
		   )
		set(BE_SDK_EXTRA_LIBRARIES
			${LIB_PATH}/tegra/libevo_beinferposestereo_${INFER_LIBRARY_NAME}64.so 
			${LIB_PATH}/tegra/libevo_bevirtualrectifypipeline_hitnet_${INFER_LIBRARY_NAME}64.so 
		   )
		set(BE_SDK_OPENGL_UTILS_LIBRARIES
			${LIB_PATH}/tegra/libevo_openglutils64.so 
			${LIB_PATH}/tegra/libevo_openglutils_cuda64.so 
		   )
	else()
		set(BE_SDK_BASE_LIBRARIES
			${LIB_PATH}/linux/libbionic_eyes_c.so
		   )
		set(BE_SDK_EXTRA_LIBRARIES
			${LIB_PATH}/linux/libevo_beinferposestereo_${INFER_LIBRARY_NAME}64.so 
			${LIB_PATH}/linux/libevo_bevirtualrectifypipeline_hitnet_${INFER_LIBRARY_NAME}64.so 
		   )
		set(BE_SDK_OPENGL_UTILS_LIBRARIES
			${LIB_PATH}/linux/libevo_openglutils64.so 
			${LIB_PATH}/linux/libevo_openglutils_cuda64.so 
		   )
	endif()
endif(WIN32)


#3rd party libraries (for sample)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#OpenCV simple sample
add_executable(evo_be_Sample_WithOpenCV_simple samples/evo_be_Sample_WithOpenCV_simple.cpp)

target_link_libraries(evo_be_Sample_WithOpenCV_simple
	PRIVATE
	${BE_SDK_BASE_LIBRARIES} 
	${OpenCV_LIBS}
)

if (NOT BUILD_SIMPLE_SAMPLE_ONLY)
	#OpenCV sample
	add_executable(evo_be_Sample_WithOpenCV samples/evo_be_Sample_WithOpenCV.cpp)

	target_link_libraries(evo_be_Sample_WithOpenCV
		PRIVATE
		${BE_SDK_BASE_LIBRARIES} 
		${BE_SDK_EXTRA_LIBRARIES} 
		${OpenCV_LIBS}
	)

	#PCL sample
	find_package(PCL COMPONENTS common io visualization filters)
	if(PCL_FOUND)
		include_directories(${PCL_INCLUDE_DIRS})
		link_directories(${PCL_LIBRARY_DIRS})

		add_executable(evo_be_Sample_WithPCL samples/evo_be_Sample_WithPCL.cpp)

		target_link_libraries(evo_be_Sample_WithPCL
			PRIVATE
			${BE_SDK_BASE_LIBRARIES} 
			${BE_SDK_EXTRA_LIBRARIES} 
			${PCL_LIBRARIES} 
			${OpenCV_LIBS}
		)
	endif(PCL_FOUND)

	#OpenGL sample
	find_package(OpenGL)
	find_package(GLUT)
	if(GLUT_FOUND)
		include_directories(${GLUT_INCLUDE_DIR})
		include_directories(${OPENGL_INCLUDE_DIR})

		add_executable(evo_be_Sample_WithOpenGL samples/evo_be_Sample_WithOpenGL.cpp)

		target_link_libraries(evo_be_Sample_WithOpenGL
			PRIVATE
			${BE_SDK_BASE_LIBRARIES} 
			${BE_SDK_EXTRA_LIBRARIES} 
			${BE_SDK_OPENGL_UTILS_LIBRARIES} 
			${GLUT_LIBRARIES} 
			${OPENGL_LIBRARIES} 
			${OpenCV_LIBS}
		)
	endif(GLUT_FOUND)
endif(BUILD_SIMPLE_SAMPLE_ONLY)