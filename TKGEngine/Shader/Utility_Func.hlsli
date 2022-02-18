#ifndef _UTILITY_FUNC_HLSLI_
#define _UTILITY_FUNC_HLSLI_

#include "ConstantBuffer_Defined.hlsli"

///////////////////////////////////////////////
// Reversed-Zからビュー空間線形深度値を計算する
// https://thxforthefish.com/posts/reverse_z/ 
//
// reversed_z	:	反転Zの値
// 
// return	:	ビュー空間内Z値
///////////////////////////////////////////////
inline float CalcReversedDepth(in float reversed_z)
{
	return (-1.0 * camera.param.x * camera.param.y) / (reversed_z * camera.param.z + camera.param.x);
}



#endif// _UTILITY_FUNC_HLSLI_