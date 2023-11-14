//Code generated automatically by TMVA for Inference of Model file [simple_model_pion.onnx] at [Wed Nov  8 12:16:10 2023] 

#ifndef TMVA_SOFIE_SIMPLE_MODEL_PION
#define TMVA_SOFIE_SIMPLE_MODEL_PION

#include<algorithm>
#include<vector>
#include "TMVA/SOFIE_common.hxx"
#include <fstream>

namespace TMVA_SOFIE_simple_model_pion{
namespace BLAS{
	extern "C" void sgemv_(const char * trans, const int * m, const int * n, const float * alpha, const float * A,
	                       const int * lda, const float * X, const int * incx, const float * beta, const float * Y, const int * incy);
	extern "C" void sgemm_(const char * transa, const char * transb, const int * m, const int * n, const int * k,
	                       const float * alpha, const float * A, const int * lda, const float * B, const int * ldb,
	                       const float * beta, float * C, const int * ldc);
}//BLAS
struct Session {
std::vector<float> fTensor_20 = std::vector<float>(50);
float * tensor_20 = fTensor_20.data();
std::vector<float> fTensor_19 = std::vector<float>(5000);
float * tensor_19 = fTensor_19.data();
std::vector<float> fTensor_outputbias = std::vector<float>(1);
float * tensor_outputbias = fTensor_outputbias.data();
std::vector<float> fTensor_l2bias = std::vector<float>(50);
float * tensor_l2bias = fTensor_l2bias.data();
std::vector<float> fTensor_18 = std::vector<float>(1400);
float * tensor_18 = fTensor_18.data();
std::vector<float> fTensor_inputbias = std::vector<float>(100);
float * tensor_inputbias = fTensor_inputbias.data();
std::vector<float> fTensor_output = std::vector<float>(1);
float * tensor_output = fTensor_output.data();
std::vector<float> fTensor_outputbiasbcast = std::vector<float>(1);
float * tensor_outputbiasbcast = fTensor_outputbiasbcast.data();
std::vector<float> fTensor_14 = std::vector<float>(50);
float * tensor_14 = fTensor_14.data();
std::vector<float> fTensor_13 = std::vector<float>(50);
float * tensor_13 = fTensor_13.data();
std::vector<float> fTensor_10 = std::vector<float>(100);
float * tensor_10 = fTensor_10.data();
std::vector<float> fTensor_l2biasbcast = std::vector<float>(50);
float * tensor_l2biasbcast = fTensor_l2biasbcast.data();
std::vector<float> fTensor_9 = std::vector<float>(100);
float * tensor_9 = fTensor_9.data();
std::vector<float> fTensor_inputbiasbcast = std::vector<float>(100);
float * tensor_inputbiasbcast = fTensor_inputbiasbcast.data();


Session(std::string filename ="") {
   if (filename.empty()) filename = "simple_model_pion.dat";
   std::ifstream f;
   f.open(filename);
   if (!f.is_open()){
      throw std::runtime_error("tmva-sofie failed to open file for input weights");
   }
   std::string tensor_name;
   int length;
   f >> tensor_name >> length;
   if (tensor_name != "tensor_20" ) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor name; expected name is tensor_20 , read " + tensor_name;
      throw std::runtime_error(err_msg);
    }
   if (length != 50) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor size; expected size is 50 , read " + std::to_string(length) ;
      throw std::runtime_error(err_msg);
    }
    for (int i =0; i < length; ++i) 
       f >> tensor_20[i];
   f >> tensor_name >> length;
   if (tensor_name != "tensor_19" ) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor name; expected name is tensor_19 , read " + tensor_name;
      throw std::runtime_error(err_msg);
    }
   if (length != 5000) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor size; expected size is 5000 , read " + std::to_string(length) ;
      throw std::runtime_error(err_msg);
    }
    for (int i =0; i < length; ++i) 
       f >> tensor_19[i];
   f >> tensor_name >> length;
   if (tensor_name != "tensor_outputbias" ) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor name; expected name is tensor_outputbias , read " + tensor_name;
      throw std::runtime_error(err_msg);
    }
   if (length != 1) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor size; expected size is 1 , read " + std::to_string(length) ;
      throw std::runtime_error(err_msg);
    }
    for (int i =0; i < length; ++i) 
       f >> tensor_outputbias[i];
   f >> tensor_name >> length;
   if (tensor_name != "tensor_l2bias" ) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor name; expected name is tensor_l2bias , read " + tensor_name;
      throw std::runtime_error(err_msg);
    }
   if (length != 50) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor size; expected size is 50 , read " + std::to_string(length) ;
      throw std::runtime_error(err_msg);
    }
    for (int i =0; i < length; ++i) 
       f >> tensor_l2bias[i];
   f >> tensor_name >> length;
   if (tensor_name != "tensor_18" ) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor name; expected name is tensor_18 , read " + tensor_name;
      throw std::runtime_error(err_msg);
    }
   if (length != 1400) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor size; expected size is 1400 , read " + std::to_string(length) ;
      throw std::runtime_error(err_msg);
    }
    for (int i =0; i < length; ++i) 
       f >> tensor_18[i];
   f >> tensor_name >> length;
   if (tensor_name != "tensor_inputbias" ) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor name; expected name is tensor_inputbias , read " + tensor_name;
      throw std::runtime_error(err_msg);
    }
   if (length != 100) {
      std::string err_msg = "TMVA-SOFIE failed to read the correct tensor size; expected size is 100 , read " + std::to_string(length) ;
      throw std::runtime_error(err_msg);
    }
    for (int i =0; i < length; ++i) 
       f >> tensor_inputbias[i];
   f.close();
   {
      float * data = TMVA::Experimental::SOFIE::UTILITY::UnidirectionalBroadcast<float>(tensor_inputbias,{ 100 }, { 1 , 100 });
      std::copy(data, data + 100, tensor_inputbiasbcast);
      delete [] data;
   }
   {
      float * data = TMVA::Experimental::SOFIE::UTILITY::UnidirectionalBroadcast<float>(tensor_l2bias,{ 50 }, { 1 , 50 });
      std::copy(data, data + 50, tensor_l2biasbcast);
      delete [] data;
   }
   {
      float * data = TMVA::Experimental::SOFIE::UTILITY::UnidirectionalBroadcast<float>(tensor_outputbias,{ 1 }, { 1 , 1 });
      std::copy(data, data + 1, tensor_outputbiasbcast);
      delete [] data;
   }
}

std::vector<float> infer(float* tensor_input){

//--------- Gemm
   char op_0_transA = 'n';
   char op_0_transB = 'n';
   int op_0_m = 1;
   int op_0_n = 100;
   int op_0_k = 14;
   float op_0_alpha = 1;
   float op_0_beta = 1;
   int op_0_lda = 14;
   int op_0_ldb = 100;
   std::copy(tensor_inputbiasbcast, tensor_inputbiasbcast + 100, tensor_9);
   BLAS::sgemm_(&op_0_transB, &op_0_transA, &op_0_n, &op_0_m, &op_0_k, &op_0_alpha, tensor_18, &op_0_ldb, tensor_input, &op_0_lda, &op_0_beta, tensor_9, &op_0_n);
   float op_1_alpha = 0.00999999978;

//------ LEAKY RELU
   for (int id = 0; id < 100 ; id++){
      tensor_10[id] = ((tensor_9[id] >= 0 )? tensor_9[id] : op_1_alpha * tensor_9[id]);
   }

//--------- Gemm
   char op_2_transA = 'n';
   char op_2_transB = 'n';
   int op_2_m = 1;
   int op_2_n = 50;
   int op_2_k = 100;
   float op_2_alpha = 1;
   float op_2_beta = 1;
   int op_2_lda = 100;
   int op_2_ldb = 50;
   std::copy(tensor_l2biasbcast, tensor_l2biasbcast + 50, tensor_13);
   BLAS::sgemm_(&op_2_transB, &op_2_transA, &op_2_n, &op_2_m, &op_2_k, &op_2_alpha, tensor_19, &op_2_ldb, tensor_10, &op_2_lda, &op_2_beta, tensor_13, &op_2_n);
   float op_3_alpha = 0.00999999978;

//------ LEAKY RELU
   for (int id = 0; id < 50 ; id++){
      tensor_14[id] = ((tensor_13[id] >= 0 )? tensor_13[id] : op_3_alpha * tensor_13[id]);
   }

//--------- Gemm
   char op_4_transA = 'n';
   char op_4_transB = 'n';
   int op_4_m = 1;
   int op_4_n = 1;
   int op_4_k = 50;
   float op_4_alpha = 1;
   float op_4_beta = 1;
   int op_4_lda = 50;
   int op_4_ldb = 1;
   std::copy(tensor_outputbiasbcast, tensor_outputbiasbcast + 1, tensor_output);
   BLAS::sgemm_(&op_4_transB, &op_4_transA, &op_4_n, &op_4_m, &op_4_k, &op_4_alpha, tensor_20, &op_4_ldb, tensor_14, &op_4_lda, &op_4_beta, tensor_output, &op_4_n);
   std::vector<float> ret (tensor_output, tensor_output + 1);
   return ret;
}
};
} //TMVA_SOFIE_simple_model_pion

#endif  // TMVA_SOFIE_SIMPLE_MODEL_PION
