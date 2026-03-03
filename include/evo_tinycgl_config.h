#ifndef EVO_TINYCGL_CONFIG_H
#define EVO_TINYCGL_CONFIG_H

#define EVO_TINYCGL_VENDOR_NAME "Shanghai Eyevolution Technology Co.,Ltd"
#define EVO_TINYCGL_VENDOR_SHORTNAME "Eyevolution"
#define EVO_TINYCGL_VENDOR_CONTACT "aiguan@ilooktech.com"
#define EVO_TINYCGL_VENDOR_DOMAIN "http://www.ilooktech.com/"

#define EVO_TINYCGL_VERSION "0.1.0"
#define EVO_TINYCGL_VERSION_MAJOR 0
#define EVO_TINYCGL_VERSION_MINOR 1
#define EVO_TINYCGL_VERSION_PATCH 0

#define EVO_BIT_64
#define EVO_BIT "64"

#ifdef EVO_BIT_
#error "system architecture is not defined."
#endif

#define EVO_ENV_LINUX
#define EVO_TINYCGL_SHARED_LIBS

#define EVO_TINYCGL_WITH_CUDA
#define EVO_TINYCGL_CUDA_VERSION_MAJOR 12
#define EVO_TINYCGL_CUDA_VERSION_MINOR 8

#endif
