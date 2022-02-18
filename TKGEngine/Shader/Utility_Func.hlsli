#ifndef _UTILITY_FUNC_HLSLI_
#define _UTILITY_FUNC_HLSLI_

#include "ConstantBuffer_Defined.hlsli"

///////////////////////////////////////////////
// Reversed-Z����r���[��Ԑ��`�[�x�l���v�Z����
// https://thxforthefish.com/posts/reverse_z/ 
//
// reversed_z	:	���]Z�̒l
// 
// return	:	�r���[��ԓ�Z�l
///////////////////////////////////////////////
inline float CalcReversedDepth(in float reversed_z)
{
	return (-1.0 * camera.param.x * camera.param.y) / (reversed_z * camera.param.z + camera.param.x);
}



#endif// _UTILITY_FUNC_HLSLI_