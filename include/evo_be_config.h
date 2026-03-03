#ifndef EVO_BE_CONFIG_H
#define EVO_BE_CONFIG_H

#define EVO_BE_VENDOR_NAME "Shanghai Eyevolution Technology Co.,Ltd"
#define EVO_BE_VENDOR_SHORTNAME "Eyevolution"
#define EVO_BE_VENDOR_CONTACT "wang.kaifang@ilooktech.com"
#define EVO_BE_VENDOR_DOMAIN "http://www.ilooktech.com/"

#define EVO_BE_VERSION "1.0.0"
#define EVO_BE_VERSION_MAJOR 1
#define EVO_BE_VERSION_MINOR 0
#define EVO_BE_VERSION_PATCH 0

#define EVO_BE_BIT_64
#define EVO_BE_BIT "64"

#ifdef EVO_BE_BIT_
#error "system architecture is not defined."
#endif

#define EVO_BE_ENV_LINUX
/* #undef EVO_BE_SHARED_LIBS */

#define EVO_BE_WITH_CUDA
#define EVO_BE_CUDA_VERSION_MAJOR 
#define EVO_BE_CUDA_VERSION_MINOR 

#define BE_SYSTEM_PATH "/usr/Evo_BionicEyes"

#endif
