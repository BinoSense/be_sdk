#ifndef EVO_OPENGL_UTILS
#define EVO_OPENGL_UTILS

#include "evo_tinycgl_global_define.h"

#if defined(EVO_ENV_WINDOWS) && !defined(EVO_OPENGLUTILS_EXPORTS)
#define EVO_OPENGLUTILS_API EVO_WINDOWS_DLLIMPORT
#else
#define EVO_OPENGLUTILS_API EVO_API
#endif

namespace evo
{
	class EVO_OPENGLUTILS_API OpenGLUtils
	{
	public:
		static void clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

		//画3D线
		static void drawLine_3d(float p1_x, float p1_y, float p1_z, 
			float p2_x, float p2_y, float p2_z,			
			float line_width = 1.0f, 
			float c1_r = 1.0f, float c1_g = 0.0f, float c1_b = 0.0f,
			float c2_r = 1.0f, float c2_g = 0.0f, float c2_b = 0.0f);
		//画3D面
		static void drawPlane_3d(float plane[4][3], 
			float line_width = 1.0f, 
			float r = 0.0f, float g = 0.0f, float b = 0.0f);

		//画坐标轴
		//modelview(4x4)为空表示不设置
		static void drawAxes_3d(float length = 50.0f,
			float* modelview = nullptr,
			float line_width = 1.0f,
			float cx_r = 1.0f, float cx_g = 0.0f, float cx_b = 0.0f,
			float cy_r = 0.0f, float cy_g = 1.0f, float cy_b = 0.0f,
			float cz_r = 0.0f, float cz_g = 0.0f, float cz_b = 1.0f);
		static void drawAxes_3d(float length,
			std::vector<float*> vector_modelview,
			float line_width = 1.0f,
			float cx_r = 1.0f, float cx_g = 0.0f, float cx_b = 0.0f,
			float cy_r = 0.0f, float cy_g = 1.0f, float cy_b = 0.0f,
			float cz_r = 0.0f, float cz_g = 0.0f, float cz_b = 1.0f);

		//画相机的可视范围
		//modelview(4x4)为空表示不设置
		static void drawFOV_3d(float fx, float fy, float cx, float cy, 
			float distance_min, float distance_max, int img_width, int img_height,
			float *modelview = nullptr,
			float line_width = 1.0f,
			float c1_r = 1.0f, float c1_g = 1.0f, float c1_b = 1.0f,
			float c2_r = 1.0f, float c2_g = 1.0f, float c2_b = 1.0f);
		static void drawFOV_3d(float fx, float fy, float cx, float cy, 
			float distance_min, float distance_max, int img_width, int img_height,
			std::vector<float*> vector_modelview,
			float line_width = 1.0f,
			float c1_r = 1.0f, float c1_g = 1.0f, float c1_b = 1.0f,
			float c2_r = 1.0f, float c2_g = 1.0f, float c2_b = 1.0f);

		//根据RT向量生成model矩阵
		static std::vector<float> buildModelMatrix(const float* rotation_vec, const float* trans_vec);//rotation_vec:3x1, trans_vec:3x1
	};

	class CameraControlGLVars;
	class EVO_OPENGLUTILS_API CameraControlGL
	{
	public:
		CameraControlGL(const float position_x = 0.0f, const float position_y = 0.0f, const float position_z = 0.0f,
			const float direction_x = 0.0f, const float direction_y = 0.0f, const float direction_z = -1.0f);
		~CameraControlGL();

		void setPositionMatrix(const float position_x = 0.0f, const float position_y = 0.0f, const float position_z = 0.0f);

		/*
		Set an offset between the eye of the camera and its position.
		Note: Useful to use the camera as a trackball camera with z > 0 and x = 0, y = 0.
		Note: coordinates are in local space.
		*/
		void setInitOffsetFromPositionMatrix(const float offset_x = 0.0f, const float offset_y = 0.0f, const float offset_z = 4000.0f);

		void setProjection(float horizontalFOV, float verticalFOV, float znear, float zfar);//degree, mm

		//camera control
		void setZoomScale(const float scale);//zoom(%)
		float getZoomScale() const;

		void setTranslation(const int delta_x, const int delta_y, const float gain = 1.25f);//translate

		void setRotation(const int delta_x, const int delta_y, const float gain = 0.01f);//rotate

		void update();//update the view & projection matrix

		void show();//show in OpenGL (used in paintGL())

		float getHorizontalFOV() const;
		float getVerticalFOV() const;

		float getZNear() const;
		float getZFar() const;

	private:
		CameraControlGLVars *vars;
	};


	class OpenGLImageInteropVars;
	class EVO_OPENGLUTILS_API OpenGLImageInterop
	{
	public:
		OpenGLImageInterop();
		~OpenGLImageInterop();

		bool init(int width, int height, int image_element_size = sizeof(unsigned char), int image_channels = 4);//element size: image(unsigned char), image_channels: 1/4
		void uninit();
		
		void draw(void *p_image, int widget_width = -1, int widget_height = -1);//if input widget size, try to show the image with original ratio, otherwise, fill the widget

	private:
		OpenGLImageInteropVars *vars;
	};


	class OpenGLPointCloudInteropVars;
	class EVO_OPENGLUTILS_API OpenGLPointCloudInterop
	{
	public:
		OpenGLPointCloudInterop();
		~OpenGLPointCloudInterop();

		bool init(int width, int height, int image_element_size = sizeof(unsigned char), int xyz_element_size = sizeof(float), int image_channels = 4);//element size: image(unsigned char), xyz(unsigned char/float)
		bool uninit();

		//modelview(4x4)为空表示不设置
		void draw(void *p_image, void *p_xyz, float *modelview = nullptr, float point_size = 1.0f);//image:channel:3/4, RGB or gray, xyz:channel:3
		void draw(void *p_image, void *p_xyz, std::vector<float*> vector_modelview, float point_size = 1.0f);//image:channel:3/4, RGB or gray, xyz:channel:3
		void draw_pointcloud(void *p_xyz_image, float *modelview = nullptr, float point_size = 1.0f);//image_channels 3: xyz-argb, image_channels 4: xyz-rgba
		void draw_pointcloud(void *p_xyz_image, std::vector<float*> vector_modelview, float point_size = 1.0f);//image_channels 3: xyz-argb, image_channels 4: xyz-rgba

	private:
		OpenGLPointCloudInteropVars * vars;
	};
}

#endif //EVO_OPENGL_UTILS
