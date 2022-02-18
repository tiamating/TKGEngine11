#ifndef _TKGENGINE_SHADER_DEFINED_HEADER_
#define _TKGENGINE_SHADER_DEFINED_HEADER_

#ifndef __cplusplus
#define MATRIX	matrix
#define VECTOR2 float2
#define VECTOR3 float3
#define VECTOR4 float4
#endif// #ifndef __cplusplus

#ifdef __cplusplus
#include "Utility/inc/myfunc_vector.h"
static constexpr const char* CB_MATERIAL_NAME = "CB_MATERIAL";

namespace TKGEngine
{
	// リソースの可視性を指定する
	enum class ShaderVisibility
	{
		VS = 1 << 0,
		PS = 1 << 1,
		GS = 1 << 2,
		DS = 1 << 3,
		HS = 1 << 4,
		CS = 1 << 5,
		ALL = 0xff
	};
#endif// #ifdef __cplusplus

	// マテリアルのバッファ名
#define CB_MATERIALNAME CB_MATERIAL
	// ボーン設定
#define MAX_BONES (200)
#define MAX_BONE_INFLUENCES (4)
	// ライト設定
#define MAX_SPOTLIGHT (8)
#define MAX_POINTLIGHT (8)
	// シャドウ設定
#define MAX_CASCADE (4)

// ==================================================================
// コンスタントバッファのスロット
// ==================================================================
#define CBS_POSTPROCESS		8	// ポストエフェクト

#define CBS_ENVIRONMENT		0	// 環境
#define CBS_TIME			1	// 時間
#define CBS_CAMERA			2	// カメラ
#define CBS_VP				3	// VP matrix
#define CBS_LIGHT			4	// 光源
#define CBS_MODEL			5	// アニメーションデータ
#define CBS_MATERIAL		6	// マテリアル
#define CBS_TEXTURE			7	// マテリアルテクスチャ

	// Instance Struct Types
	struct MainInstance
	{
#ifdef __cplusplus
		MATRIX transform_matrix;
		VECTOR4 instance_color;
#else
		matrix transform_matrix : TRANSFORM_MATRIX;
		float4 instance_color : INSTANCE_COLOR;
#endif// #ifdef __cplusplus
	};
	struct UIInstance
	{
#ifdef __cplusplus
		MATRIX transform_matrix;
		VECTOR4 instance_color;
		VECTOR4 texture_transform;
#else
		matrix transform_matrix : TRANSFORM_MATRIX;
		float4 instance_color : INSTANCE_COLOR;
		float4 texture_transform : TEXCOORD_TRANSFORM;
#endif// #ifdef __cplusplus
	};
	// ~Instance Struct Types

// ==================================================================
// サンプラーステートスロットの定義
// ==================================================================
#define SAMPLSLOT_ONDEMAND0     0
#define SAMPLSLOT_ONDEMAND1     1
#define SAMPLSLOT_ONDEMAND2     2
#define SAMPLSLOT_ONDEMAND3     3

#define SAMPLSLOT_LINEAR_MIRROR 4
#define SAMPLSLOT_LINEAR_CLAMP  5
#define SAMPLSLOT_LINEAR_WRAP   6
#define SAMPLSLOT_POINT_MIRROR  7
#define SAMPLSLOT_POINT_CLAMP   8
#define SAMPLSLOT_POINT_WRAP    9
#define SAMPLSLOT_ANISO_MIRROR  10
#define SAMPLSLOT_ANISO_CLAMP   11
#define SAMPLSLOT_ANISO_WRAP    12
#define SAMPLSLOT_CMP_DEPTH     13

// ==================================================================
// 共通Texture
// ==================================================================
#define TEXSLOT_DEPTH				0
#define TEXSLOT_COLOR				1

#define TEXSLOT_GBUFFER0			2
#define TEXSLOT_GBUFFER1			3
#define TEXSLOT_GBUFFER2			4
#define TEXSLOT_GBUFFER3			5

#define TEXSLOT_GLOBALENVMAP		6
//#define TEXSLOT_GLOBALLIGHTMAP	7
//#define TEXSLOT_ENVMAPARRAY		8

#define TEXSLOT_SPRITE				19

// ==================================================================
// 任意のテクスチャバインド時に使用するスロット
// ==================================================================
#define TEXSLOT_ONDEMAND0			20
#define TEXSLOT_ONDEMAND1			21
#define TEXSLOT_ONDEMAND2			22
#define TEXSLOT_ONDEMAND3			23
#define TEXSLOT_ONDEMAND4			24
#define TEXSLOT_ONDEMAND5			25
#define TEXSLOT_ONDEMAND6			26
#define TEXSLOT_ONDEMAND7			27
#define TEXSLOT_ONDEMAND8			28
#define TEXSLOT_ONDEMAND9			29
#define TEXSLOT_ONDEMAND10			30
#define TEXSLOT_ONDEMAND11			31
#define TEXSLOT_ONDEMAND12			32
#define TEXSLOT_ONDEMAND13			33
#define TEXSLOT_ONDEMAND14			34
#define TEXSLOT_ONDEMAND15			35
#define TEXSLOT_ONDEMAND16			36
#define TEXSLOT_ONDEMAND17			37
#define TEXSLOT_ONDEMAND18			38
#define TEXSLOT_ONDEMAND19			39
#define TEXSLOT_ONDEMAND20			40
#define TEXSLOT_ONDEMAND21			41
#define TEXSLOT_ONDEMAND22			42

// Object Renderer
#define TEXSLOT_RENDERER_BASECOLORMAP		TEXSLOT_ONDEMAND0
#define TEXSLOT_RENDERER_NORMALMAP			TEXSLOT_ONDEMAND1
#define TEXSLOT_RENDERER_SURFACEMAP			TEXSLOT_ONDEMAND2
#define TEXSLOT_RENDERER_EMISSIVEMAP		TEXSLOT_ONDEMAND3
#define TEXSLOT_RENDERER_DISPLACEMENTMAP	TEXSLOT_ONDEMAND4
#define TEXSLOT_RENDERER_ROUGHNESS			TEXSLOT_ONDEMAND5
#define TEXSLOT_RENDERER_METALNESS			TEXSLOT_ONDEMAND6
#define TEXSLOT_RENDERER_OCCLUSION			TEXSLOT_ONDEMAND7
#define TEXSLOT_RENDERER_ONDEMAND3			TEXSLOT_ONDEMAND8
#define TEXSLOT_RENDERER_ONDEMAND4			TEXSLOT_ONDEMAND9
#define TEXSLOT_RENDERER_ONDEMAND5			TEXSLOT_ONDEMAND10
#define TEXSLOT_RENDERER_ONDEMAND6			TEXSLOT_ONDEMAND11
#define TEXSLOT_RENDERER_ONDEMAND7			TEXSLOT_ONDEMAND12
#define TEXSLOT_RENDERER_ONDEMAND8			TEXSLOT_ONDEMAND13
#define TEXSLOT_RENDERER_ONDEMAND9			TEXSLOT_ONDEMAND14
#define TEXSLOT_RENDERER_ONDEMAND10			TEXSLOT_ONDEMAND15
#define TEXSLOT_RENDERER_ONDEMAND11			TEXSLOT_ONDEMAND16
#define TEXSLOT_RENDERER_ONDEMAND12			TEXSLOT_ONDEMAND17
#define TEXSLOT_RENDERER_ONDEMAND13			TEXSLOT_ONDEMAND18
#define TEXSLOT_RENDERER_ONDEMAND14			TEXSLOT_ONDEMAND19
#define TEXSLOT_RENDERER_ONDEMAND15			TEXSLOT_ONDEMAND20
#define TEXSLOT_RENDERER_ONDEMAND16			TEXSLOT_ONDEMAND21
#define TEXSLOT_RENDERER_ONDEMAND17			TEXSLOT_ONDEMAND22

// ==================================================================
// Shadow Map
// ==================================================================
// Directional Light
#define TEXSLOT_SHADOWMAP_CASCADE0	50
#define TEXSLOT_SHADOWMAP_CASCADE1	51
#define TEXSLOT_SHADOWMAP_CASCADE2	52
#define TEXSLOT_SHADOWMAP_CASCADE3	53

// Spot Light
#define TEXSLOT_SHADOWMAP_SPOT0		54
#define TEXSLOT_SHADOWMAP_SPOT1		55
#define TEXSLOT_SHADOWMAP_SPOT2		56
#define TEXSLOT_SHADOWMAP_SPOT3		57
#define TEXSLOT_SHADOWMAP_SPOT4		58
#define TEXSLOT_SHADOWMAP_SPOT5		59
#define TEXSLOT_SHADOWMAP_SPOT6		60
#define TEXSLOT_SHADOWMAP_SPOT7		61

// ==================================================================
// Lighting
// ==================================================================
// Spot Light Mask
#define TEXSLOT_SPOT_MASK0		70
#define TEXSLOT_SPOT_MASK1		71
#define TEXSLOT_SPOT_MASK2		72
#define TEXSLOT_SPOT_MASK3		73
#define TEXSLOT_SPOT_MASK4		74
#define TEXSLOT_SPOT_MASK5		75
#define TEXSLOT_SPOT_MASK6		76
#define TEXSLOT_SPOT_MASK7		77

// ==================================================================
// Skinning
// ==================================================================
#define SKINNINGSLOT_IN_VERTEX_POS	TEXSLOT_ONDEMAND0
#define SKINNINGSLOT_IN_VERTEX_NOR	TEXSLOT_ONDEMAND1
#define SKINNINGSLOT_IN_VERTEX_TAN	TEXSLOT_ONDEMAND2
#define SKINNINGSLOT_IN_VERTEX_BON	TEXSLOT_ONDEMAND3
#define SKINNINGSLOT_IN_VERTEX_WEI	TEXSLOT_ONDEMAND4

#define SKINNINGSLOT_OUT_VERTEX_POS		0
#define SKINNINGSLOT_OUT_VERTEX_NOR		1
#define SKINNINGSLOT_OUT_VERTEX_TAN		2

#ifdef __cplusplus

	// スロット番号のスタート番号
	constexpr int TEXTURE_RENDERER_SLOT_BEGIN_NUM = TEXSLOT_RENDERER_BASECOLORMAP;

	// 上のTEXSLOT_RENDERER_XXXを書き換えたら名前を変える	
	// Texture slot names
constexpr const char* TEXTURE_RENDERER_SLOT_NAMES[] =
{
	"BaseColorMap",
	"NormalMap",
	"SurfaceMap",
	"EmissiveMap",
	"DisplacementMap",
	"RoughnessMap",
	"MetalnessMap",
	"OcclusionMap",
	"Ondemand3",
	"Ondemand4",
	"Ondemand5",
	"Ondemand6",
	"Ondemand7",
	"Ondemand8",
	"Ondemand9",
	"Ondemand10",
	"Ondemand11",
	"Ondemand12",
	"Ondemand13",
	"Ondemand14",
	"Ondemand15",
	"Ondemand16",
	"Ondemand17"
};
#endif // #ifdef __cplusplus

// PostEffect
#define TEXSLOT_POSTEFFECT0	TEXSLOT_ONDEMAND0
#define TEXSLOT_POSTEFFECT1	TEXSLOT_ONDEMAND1
#define TEXSLOT_POSTEFFECT2	TEXSLOT_ONDEMAND2
#define TEXSLOT_POSTEFFECT3	TEXSLOT_ONDEMAND3
#define TEXSLOT_POSTEFFECT4	TEXSLOT_ONDEMAND4

#ifndef __cplusplus
// Macro
#define CBUFFER(name, slot) cbuffer name : register(b ## slot)

#define RAWBUFFER(name,slot) ByteAddressBuffer name : register(t ## slot)
#define RWRAWBUFFER(name,slot) RWByteAddressBuffer name : register(u ## slot)

#define STRUCTUREDBUFFER(name, type, slot) StructuredBuffer< type > name : register(t ## slot)
#define RWSTRUCTUREDBUFFER(name, type, slot) RWStructuredBuffer< type > name : register(u ## slot)

#define TEXTURE1D(name, type, slot) Texture1D< type > name : register(t ## slot)
#define TEXTURE1DARRAY(name, type, slot) Texture1DArray< type > name : register(t ## slot)
#define RWTEXTURE1D(name, type, slot) RWTexture1D< type > name : register(u ## slot)

#define TEXTURE2D(name, type, slot) Texture2D< type > name : register(t ## slot)
#define TEXTURE2DARRAY(name, type, slot) Texture2DArray< type > name : register(t ## slot)
#define RWTEXTURE2D(name, type, slot) RWTexture2D< type > name : register(u ## slot)

#define TEXTURECUBE(name, type, slot) TextureCube< type > name : register(t ## slot)
#define TEXTURECUBEARRAY(name, type, slot) TextureCubeArray< type > name : register(t ## slot)

#define TEXTURE3D(name, type, slot) Texture3D< type > name : register(t ## slot)
#define RWTEXTURE3D(name, type, slot) RWTexture3D< type > name : register(u ## slot)

#define SAMPLERSTATE(name, slot) SamplerState name : register(s ## slot)
#define SAMPLERCOMPARISONSTATE(name, slot) SamplerComparisonState name : register(s ## slot)

#endif // #ifndef __cplusplus

#ifdef __cplusplus
}// namespace TKGEngine
#endif // #ifdef __cplusplus


#endif // _TKGENGINE_SHADER_DEFINED_HEADER_