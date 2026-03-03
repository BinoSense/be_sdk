#ifndef EVO_BEINFERPOSESTEREO_TRT_H
#define EVO_BEINFERPOSESTEREO_TRT_H

#include "evo_tinycgl_global_define.h"

#if defined(EVO_ENV_WINDOWS) && !defined(EVO_BEINFERPOSESTEREO_TRT_EXPORTS)
#define EVO_BEINFERPOSESTEREO_TRT_API EVO_WINDOWS_DLLIMPORT
#else
#define EVO_BEINFERPOSESTEREO_TRT_API EVO_API
#endif

namespace evo
{
    class BEInferPoseStereo_TrtVars;
    class EVO_BEINFERPOSESTEREO_TRT_API BEInferPoseStereo_Trt
    {
    public:
        BEInferPoseStereo_Trt();
        ~BEInferPoseStereo_Trt();

        void init(std::string left_model_path, std::string right_model_path, std::string rt_path);
        void init(std::string left_model_path, std::string right_model_path, std::vector<float> r, std::vector<float> t);
        void init(std::string left_model_path, std::string right_model_path, std::vector<double> r, std::vector<double> t);

        //输入大小为6，均在内存里
        void process(std::vector<float> input);
        void process(float *input);

        //输出大小为6
        float *retrieveLR();//左眼到右眼的RT
        float *retrieve0L();//左眼0位置到新位置的RT
        float *retrieve0R();//右眼0位置到新位置的RT

    private:
        BEInferPoseStereo_TrtVars* vars;
    };
}

#endif
