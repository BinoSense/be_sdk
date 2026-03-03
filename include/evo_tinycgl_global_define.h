#ifndef EVO_TINYCGL_GLOBAL_DEFINE
#define EVO_TINYCGL_GLOBAL_DEFINE

#include "evo_tinycgl_config.h"

/* program language defination */
#if defined(__cplusplus)
#define EVO_LANG_CPP
#endif
#if defined(__OBJC__)
#define EVO_LANG_OBJC
#endif

#if defined(EVO_ENV_WINDOWS)
#define EVO_TEMPLATE
#if defined(EVO_TINYCGL_SHARED_LIBS)
#define EVO_API __declspec(dllexport)
#define EVO_WINDOWS_DLLIMPORT __declspec(dllimport)
#else
#define EVO_API
#define EVO_WINDOWS_DLLIMPORT
#endif
#endif
#if defined(EVO_ENV_MACOS) || defined(EVO_ENV_ARM) || defined(EVO_ENV_LINUX)
#define EVO_API __attribute__ ((visibility("default")))
#define EVO_TEMPLATE __attribute__ ((visibility("default")))
#include <cmath>
#endif

#include <iostream>
#include <string>
#include <string.h>
#include <vector>

/* Cocoa and Obj-C compatibility */
#if defined(EVO_ENV_MACOS)
#include <CoreFoundation/CFString.h>
#include <CoreGraphics/CGImage.h>
#include <CoreGraphics/CGContext.h>
#include <CoreGraphics/CGBitmapContext.h>
#if defined(EVO_LANG_OBJC)
#import <Foundation/NSString.h>
#endif
#endif

#if defined(__clang__) || defined(__GNUC__)
#define CPP_STANDARD __cplusplus
#elif defined(_MSC_VER)
#define CPP_STANDARD _MSVC_LANG
#endif


namespace evo
{
    typedef enum INPUT_IMAGE_FORMAT
    {
        INPUT_IMAGE_FORMAT_GRAY = 0,
        INPUT_IMAGE_FORMAT_BGR,
        INPUT_IMAGE_FORMAT_RGB,
        INPUT_IMAGE_FORMAT_BGRA,
        INPUT_IMAGE_FORMAT_RGBA,
        INPUT_IMAGE_FORMAT_IYUV,
        LAST_INPUT_IMAGE_FORMAT
    } INPUT_IMAGE_FORMAT;
	
	/**
	*  \enum POINT_CLOUD_TYPE
	*  \ingroup Enumerations
	*  \brief Point cloud type (float) 
	*  \note To convert to pcl::PointXYZRGBA, using POINT_CLOUD_TYPE_POINT_CLOUD_UNORGANIZED_XYZBGRA for the correct color order.
	*		Alpha channel is 255 for the point cloud.
	*/
	namespace depth
	{
		typedef enum POINT_CLOUD_TYPE
		{
			POINT_CLOUD_TYPE_POINT_CLOUD_ORGANIZED_XYZ4,/*!< Point cloud, 4 channels XYZ, channel 4 is empty */
			POINT_CLOUD_TYPE_POINT_CLOUD_ORGANIZED_XYZBGRA,/*!< Organized point cloud, 4 channels XYZ, channel 4 contains color in B-G-R-A order (unsigned char * 4, low->high) (default unit: mm) */
			POINT_CLOUD_TYPE_POINT_CLOUD_ORGANIZED_XYZRGBA,/*!< Organized point cloud, 4 channels XYZ, channel 4 contains color in R-G-B-A order (unsigned char * 4, low->high) (default unit: mm) */
			POINT_CLOUD_TYPE_POINT_CLOUD_ORGANIZED_XYZABGR,/*!< Organized point cloud, 4 channels XYZ, channel 4 contains color in A-B-G-R order (unsigned char * 4, low->high) (default unit: mm) */
			POINT_CLOUD_TYPE_POINT_CLOUD_ORGANIZED_XYZARGB,/*!< Organized point cloud, 4 channels XYZ, channel 4 contains color in A-R-G-B order (unsigned char * 4, low->high) (default unit: mm) */
			POINT_CLOUD_TYPE_POINT_CLOUD_UNORGANIZED_XYZ4,/*!< Unorganized point cloud (INIFINITY removed), 4 channels XYZ, channel 4 is empty */
			POINT_CLOUD_TYPE_POINT_CLOUD_UNORGANIZED_XYZBGRA,/*!< Unorganized point cloud (INIFINITY removed), 4 channels XYZ, channel 4 contains color in B-G-R-A order (unsigned char * 4, low->high) (default unit: mm) */
			POINT_CLOUD_TYPE_POINT_CLOUD_UNORGANIZED_XYZRGBA,/*!< Unorganized point cloud (INIFINITY removed), 4 channels XYZ, channel 4 contains color in R-G-B-A order (unsigned char * 4, low->high) (default unit: mm) */
			POINT_CLOUD_TYPE_POINT_CLOUD_UNORGANIZED_XYZABGR,/*!< Unorganized point cloud (INIFINITY removed), 4 channels XYZ, channel 4 contains color in A-B-G-R order (unsigned char * 4, low->high) (default unit: mm) */
			POINT_CLOUD_TYPE_POINT_CLOUD_UNORGANIZED_XYZARGB,/*!< Unorganized point cloud (INIFINITY removed), 4 channels XYZ, channel 4 contains color in A-R-G-B order (unsigned char * 4, low->high) (default unit: mm) */
			LAST_POINT_CLOUD_TYPE
		} POINT_CLOUD_TYPE;
	}
}

#endif //EVO_TINYCGL_GLOBAL_DEFINE
