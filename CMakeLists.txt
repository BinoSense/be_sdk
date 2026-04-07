cmake_minimum_required(VERSION 3.16)
project(BE_SDK_mini)

set(${CMAKE_PROJECT_NAME}_SUMMARY              "BE_SDK_mini by Eyevolution")
set(${CMAKE_PROJECT_NAME}_VENDOR_NAME          "Shanghai Eyevolution Technology Co.,Ltd")
set(${CMAKE_PROJECT_NAME}_VENDOR_SHORTNAME     "Eyevolution")
set(${CMAKE_PROJECT_NAME}_VENDOR_CONTACT       "aiguan@ilooktech.com")
set(${CMAKE_PROJECT_NAME}_VENDOR_DOMAIN        "http://www.ilooktech.com/")
set(${CMAKE_PROJECT_NAME}_VERSION_MAJOR        2)
set(${CMAKE_PROJECT_NAME}_VERSION_MINOR        0)
set(${CMAKE_PROJECT_NAME}_VERSION_PATCH        0)
set(PROJECT_VERSION
  "${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}.${${CMAKE_PROJECT_NAME}_VERSION_MINOR}.${${CMAKE_PROJECT_NAME}_VERSION_PATCH}")

set(PROJECT_PATH                           "${CMAKE_CURRENT_SOURCE_DIR}")
set(BE_SDK_INCLUDE_DIRS                    "${PROJECT_PATH}/include")
set(LIB_PATH                               "${PROJECT_PATH}/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -Wl,-rpath=. ")
set(CMAKE_CONFIGURATION_TYPES "RelWithDebInfo;Release" CACHE STRING "" FORCE )
# default built type
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "set build type" FORCE)
endif()

#BE_SDK_BASE_LIBRARIES: base libraries to get data and control the device
#BE_SDK_OPENGL_UTILS_LIBRARIES: support libraries to run with OpenGL
if(WIN32)
	set(BE_SDK_BASE_LIBRARIES
		${LIB_PATH}/win/bionic_eyes_c.lib
		)
	set(BE_SDK_OPENGL_UTILS_LIBRARIES
		${LIB_PATH}/win/evo_openglutils64.lib 
		)
else()
	if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
		set(BE_SDK_BASE_LIBRARIES
			${LIB_PATH}/tegra/libbionic_eyes_c.so
		   )
		set(BE_SDK_OPENGL_UTILS_LIBRARIES
			${LIB_PATH}/tegra/libevo_openglutils64.so 
		   )
	else()
		set(BE_SDK_BASE_LIBRARIES
			${LIB_PATH}/linux/libbionic_eyes_c.so
		   )
		set(BE_SDK_OPENGL_UTILS_LIBRARIES
			${LIB_PATH}/linux/libevo_openglutils64.so 
		   )
	endif()
endif()


#3rd party libraries (for sample)
find_package(OpenCV REQUIRED)
find_package(OpenGL)
find_package(GLUT)
find_package(PCL COMPONENTS common io visualization filters)

#OpenCV simple sample to show image and control motor
add_executable(evo_be_Sample_WithOpenCV_simple samples/evo_be_Sample_WithOpenCV_simple.cpp)
target_include_directories(evo_be_Sample_WithOpenCV_simple 
	PRIVATE
	${BE_SDK_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(evo_be_Sample_WithOpenCV_simple
	PRIVATE
	${BE_SDK_BASE_LIBRARIES} 
	${OpenCV_LIBS}
)

#OpenGL sample to show pointcloud
if (GLUT_FOUND)
	add_executable(evo_be_Sample_WithOpenGL samples/evo_be_Sample_WithOpenGL.cpp)
	target_include_directories(evo_be_Sample_WithOpenGL 
		PRIVATE
		${BE_SDK_INCLUDE_DIRS}
		${GLUT_INCLUDE_DIR}
		${OPENGL_INCLUDE_DIR}
		${OpenCV_INCLUDE_DIRS}
	)
	target_link_libraries(evo_be_Sample_WithOpenGL
		PRIVATE
		${BE_SDK_OPENGL_UTILS_LIBRARIES} 
		${BE_SDK_BASE_LIBRARIES} 
		${GLUT_LIBRARIES} 
		${OPENGL_LIBRARIES} 
		${OpenCV_LIBS}
	)
endif()

#PCL sample to show pointcloud
if(PCL_FOUND)
	add_executable(evo_be_Sample_WithPCL samples/evo_be_Sample_WithPCL.cpp)
	target_include_directories(evo_be_Sample_WithPCL 
		PRIVATE
		${BE_SDK_INCLUDE_DIRS}
		${PCL_INCLUDE_DIRS}
		${OpenCV_INCLUDE_DIRS}
	)
	target_link_directories(evo_be_Sample_WithPCL
		PRIVATE
		${PCL_LIBRARY_DIRS})
	target_link_libraries(evo_be_Sample_WithPCL
		PRIVATE
		${BE_SDK_BASE_LIBRARIES} 
		${PCL_LIBRARIES} 
		${OpenCV_LIBS}
	)
endif(PCL_FOUND)