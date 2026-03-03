#ifndef EVO_OPENGL_UTILS_CUDA
#define EVO_OPENGL_UTILS_CUDA

#include "evo_tinycgl_global_define.h"

#if defined(EVO_ENV_WINDOWS) && !defined(EVO_OPENGLUTILS_CUDA_EXPORTS)
#define EVO_OPENGLUTILS_CUDA_API EVO_WINDOWS_DLLIMPORT
#else
#define EVO_OPENGLUTILS_CUDA_API EVO_API
#endif

namespace evo
{
	class EVO_OPENGLUTILS_CUDA_API OpenGLUtilsCUDA
	{
	public:
		static bool selectGLDevice(int index = -1);//call this function to bind OpenGL context to CUDA. -1 for current device
	};

	class OpenGLPointCloudInteropCUDAVars;
	class EVO_OPENGLUTILS_CUDA_API OpenGLPointCloudInteropCUDA
	{
	public:
		OpenGLPointCloudInteropCUDA();
		~OpenGLPointCloudInteropCUDA();

		bool init(int width, int height, int image_element_size = sizeof(unsigned char), int xyz_element_size = sizeof(float), int image_channels = 4);//element size: image(unsigned char), xyz(unsigned char/float)
		bool uninit(); 
		void draw(void *p_image, void *p_xyz, float point_size = 1.0f);//image:channel:3/4, BGR or gray, xyz:channel:3
		void draw(void *p_xyz_image, float point_size = 1.0f);//image_channels 3: xyz-argb, image_channels 4: xyz-rgba

	private:
		OpenGLPointCloudInteropCUDAVars *vars;
	};

	class OpenGLImageInteropCUDAVars;
	class EVO_OPENGLUTILS_CUDA_API OpenGLImageInteropCUDA
	{
	public:
		OpenGLImageInteropCUDA();
		~OpenGLImageInteropCUDA();

		bool init(int width, int height, int image_element_size = sizeof(unsigned char), int image_channels = 4);//element size: image(unsigned char), image_channels: 1/4
		bool uninit();
		void draw(void *p_image, int widget_width = -1, int widget_height = -1);//if input widget size, try to show the image with original ratio, otherwise, fill the widget

	private:
		OpenGLImageInteropCUDAVars *vars;
	};
}

#endif //EVO_OPENGL_UTILS_CUDA
