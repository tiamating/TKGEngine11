#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Application/Resource/inc/Shader_Defined.h"
#include "Application/Resource/inc/ConstantBuffer.h"
#include "Application/Resource/inc/VertexBuffer.h"
#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/template_thread.h"

#include <list>
#include <unordered_map>
#include <memory>


// ==============================================
// 前方宣言
// ==============================================
namespace TKGEngine
{
	struct MainData;
	class ICamera;
	class Light;
	class VertexBuffer;
}

namespace TKGEngine
{
	/// <summary>
	/// ライトコンポーネントの管理、パイプラインにセットするマネージャークラス(シングルトン)
	/// </summary>
	class LightManager
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static bool Create();
		static void Destroy();

		static LightManager* GetInstance();

		// 所有するGameObjectIDをキーにしたmapに登録する
		static void RegisterManager(const std::shared_ptr<Light>& light);
		static void UnregisterManager(const GameObjectID owner_id);

		// シーン内のライトをソートする
		static void SortSceneLight();

		// ライトごとに更新の計算をする
		static void CalculateSceneLight(const std::vector<MainData>& scene_objects, const int object_num, const std::shared_ptr<ICamera>& camera);

		// ライトごとに影を描画する
		static void RenderShadow(ID3D11DeviceContext* context);

		// ライトをパイプラインにセットする
		static void SetPipeline(ID3D11DeviceContext* context);


		// ==============================================
		// public methods
		// ==============================================
		/* nothing */
		

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private struct
		// ==============================================
		struct DirectionalLight
		{
			MATRIX cascade_LVPs[MAX_CASCADE];		// 分割ごとのLVP
			MATRIX cascade_light_views[MAX_CASCADE];		// 分割ごとのLightView
			MATRIX cascade_light_projections[MAX_CASCADE];	// 分割ごとのLightProj
			VECTOR4 color = VECTOR4::Zero;			// ライト色
			VECTOR3 direction = VECTOR3::Forward;	// ライト方向
			float strength = 0.4f;					// 影色の強さ
			float shadow_size[4];				// シャドウマップのサイズ
			float inv_shadow_size[4];				// シャドウマップのサイズの逆数
			float bias = 0.01f;						// シャドウバイアス
			float normal_bias = 0.01f;				// 法線方向のシャドウバイアス
			BOOL use_shadow_map = FALSE;			// シャドウマップを使用するか
			unsigned cascade_num = MAX_CASCADE;		// 分割数
			BOOL has_caster[MAX_CASCADE] = { FALSE };	// キャスターを持っているか
			float filter_radius_uv = 1.0f;			// サンプリングフィルタのUV半径
			unsigned sampling_count = 1;			// サンプリング数(最大：MAX_POISSON)
			float dummy1;
			float dummy2;

			void operator=(const std::shared_ptr<Light>& light);
		};
		struct SpotLight
		{
			MATRIX LVP = MATRIX::Identity;			// ライトビュープロジェクション
			MATRIX light_view = MATRIX::Identity;	// ライトビュー
			MATRIX light_projection = MATRIX::Identity;	// ライトプロジェクション
			VECTOR4 pos = VECTOR4::Zero;			// ライト位置
			VECTOR4 color = VECTOR4::Zero;			// ライト色
			VECTOR3 direction = VECTOR3::Forward;	// ライト方向
			float range = 0.0f;						// 範囲(0以下で無効なライト)
			VECTOR3 attenuation = VECTOR3::One;		// 減衰係数(x:定数 y:一次 z:二次)
			float falloff = 1.0f;					// 減衰係数(inner --> outerへの減衰する形)
			float inner_cone_cos = 1.0f;			// cos(θ/2) (θ:内側円錐の広がり角)
			float outer_cone_cos = 1.0f;			// cos(θ/2) (θ:外側円錐の広がり角)
			BOOL use_light_mask = FALSE;			// ライトマスクテクスチャを使用するか
			BOOL use_shadow_map = FALSE;			// シャドウマップを使用するか
			float strength = 0.4f;					// 影色の強さ
			float bias = 0.01f;						// シャドウバイアス
			float normal_bias = 0.01f;				// 法線方向のシャドウバイアス
			float shadow_size;						// シャドウマップのサイズ
			float inv_shadow_size;					// シャドウマップのサイズの逆数
			float dummy1;
			float dummy2;
			float dummy3;

			void operator=(const std::shared_ptr<Light>& light);
		};
		struct PointLight
		{
			MATRIX LVPs[2] = { MATRIX::Identity };	// ライトビュープロジェクション
			MATRIX light_views[2] = { MATRIX::Identity };		// ライトビュー
			MATRIX light_projections[2] = { MATRIX::Identity };	// ライトプロジェクション
			VECTOR4 pos = VECTOR4::Zero;			// ライト位置
			VECTOR4 color = VECTOR4::Zero;			// ライト色
			VECTOR3 attenuation = VECTOR3::One;		// 減衰係数(x:定数 y:一次 z:二次)
			float range = 0.0f;						// 範囲(0以下で無効なライト)
			BOOL use_shadow_map = FALSE;			// シャドウマップを使用するか
			float strength = 0.4f;					// 影色の強さ
			float bias = 0.01f;						// シャドウバイアス
			float normal_bias = 0.01f;				// 法線方向のシャドウバイアス
			float shadow_size[2];					// シャドウマップのサイズ
			float inv_shadow_size[2];				// シャドウマップのサイズの逆数

			void operator=(const std::shared_ptr<Light>& light);
		};

		// CBufferデータ用構造体
		struct CB_Light
		{
			DirectionalLight directional;
			SpotLight spots[MAX_SPOTLIGHT];
			PointLight points[MAX_POINTLIGHT];
		};

	private:
		// ==============================================
		// private methods
		// ==============================================
		LightManager() = default;
		virtual ~LightManager() = default;

		bool Initialize();
		void Finalize();

		// 現在アクティブなライトをタイプごとにソートする
		// TODO : 有効なライト数が最大数を超えている場合はカメラからの距離によって選択する
		void SortLightType();

		// カメラを受け取って距離から有効な動的ライトの選択を行う
		void OnApplyParameter();

		// ライトごとに更新の計算をする
		void OnCalculateSceneLight(const std::vector<MainData>& scene_objects, const int object_num, const std::shared_ptr<ICamera>& camera);
		// 影を描画するオブジェクトの計算をライトごとに実行する
		void OnCalculateCaster(const std::list<std::shared_ptr<Light>>& light_list, const std::vector<MainData>& scene_objects, int object_num, const std::shared_ptr<ICamera>& camera);
		// ライトごとに影を描画する
		void OnRenderShadow(ID3D11DeviceContext* context);
		// ライトごとに影を描画するかチェックし、実行する
		void OnRenderShadowCheckAndRender(const std::list<std::shared_ptr<Light>>& light_list, ID3D11DeviceContext* context);
		// パイプラインにセットする
		void OnSetPipeline(ID3D11DeviceContext* context);
		// パイプラインにセット時にライトマスクとシャドウマップをセットする
		void SetTexture(ID3D11DeviceContext* context, const std::list<std::shared_ptr<Light>>& light_list);
		void OnSetTextureDirectional(ID3D11DeviceContext* context, const std::shared_ptr<Light>& light, int index);
		void OnSetTextureSpot(ID3D11DeviceContext* context, const std::shared_ptr<Light>& light, int index);
		void OnSetTexturePoint(ID3D11DeviceContext* context, const std::shared_ptr<Light>& light, int index);

		// ==============================================
		// private variables
		// ==============================================
		// Singletonインスタンス
		static LightManager* m_instance;

		// 存在するライトリスト
		std::unordered_map<GameObjectID, std::shared_ptr<Light>> m_light_list;
		// ライトタイプごとのソートリスト
		std::list<std::shared_ptr<Light>> m_directional_list;
		std::list<std::shared_ptr<Light>> m_spot_list;
		std::list<std::shared_ptr<Light>> m_point_list;

		// Light CBuffer
		ConstantBuffer m_cb_light;
		// Shadowインスタンスバッファ
		VertexBuffer m_instance_buffer;
		int m_current_buffer_size = 0;

		// シャドウキャスター計算用スレッド
		static constexpr int MAX_THREAD_NUM = 1 + MAX_POINTLIGHT + MAX_SPOTLIGHT;
		ThreadPool m_calculate_shadow_caster_thread = ThreadPool(MAX_THREAD_NUM);
		std::vector<std::future<int>> m_thread_result = std::vector< std::future<int>>();
		
	};

}// namespace TKGEngine

