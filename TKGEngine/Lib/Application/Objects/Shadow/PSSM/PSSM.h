#pragma once

#include "Application/Objects/Shadow/ShadowMapBase.h"
#include "Application/Resource/inc/Shader_Defined.h"
#include "Application/Resource/inc/ITarget.h"
#include "Application/Resource/inc/ConstantBuffer.h"
#include "Utility/inc/template_thread.h"

namespace TKGEngine
{
	/// <summary>
	/// 平行分割シャドウマップ
	/// </summary>
	class PSSM
		: public ShadowMapBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		PSSM() = default;
		PSSM(LightType type);
		virtual ~PSSM() = default;
		PSSM(const PSSM&) = delete;
		PSSM& operator=(const PSSM&) = delete;


		// ShadowMapの作成
		virtual void Create() override;

		// 影を描画するオブジェクトを計算する
		virtual int CalculateShadowCaster(const std::vector<MainData>& scene_objects, int object_num, const std::shared_ptr<ICamera>& camera, const LightShadowData& data) override;
		// Shadow描画
		virtual void RenderShadow(ID3D11DeviceContext* context, VertexBuffer& instance_buffer) override;
		// リソースをセット
		virtual void SetSRV(ID3D11DeviceContext* context, int slot, int index = 0) const override;
		// ライト行列を返す
		virtual const MATRIX& GetLVP(int index = 0) const override;
		virtual const MATRIX& GetLV(int index = 0) const override;
		virtual const MATRIX& GetLP(int index = 0) const override;
		// シャドウマップのサイズを返す
		virtual float GetShadowSize(int index = 0) const override;
		// 分割シャドウマップを使用する場合の分割数を返す
		virtual unsigned GetCascadeNum() const override;
		// 現在のフレームにシャドウマップの描画を実行するか
		virtual bool IsDrawShadowMap() const override;
		virtual bool IsDrawShadowMap(int index) const override;

		// Shadowのサンプリング情報を返す
		virtual float GetSamplingRadius() const override;
		virtual int GetSamplingCount() const override;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 2)
			{
				archive(
					cereal::base_class<ShadowMapBase>(this),
					CEREAL_NVP(m_cascade_num),
					CEREAL_NVP(m_test_accurate),
					CEREAL_NVP(m_lambda),
					CEREAL_NVP(m_sampling_radius_uv),
					CEREAL_NVP(m_sampling_count)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		virtual void OnGUI() override;
#endif // USE_IMGUI

		// 分割数のセット
		void SetCascadeNum(int num);
		// シャドウマップのクリア
		void ClearFrameData(ID3D11DeviceContext* context);
		// ライトのビュー行列を求める
		MATRIX CalculateLightView(const VECTOR3& light_dir, const VECTOR3& light_up, const VECTOR3& light_pos);
		// 分割位置を求める
		void CalculateSplitPosition(float near_plane, float far_plane);
		// ライトビュー空間での影を落とすオブジェクトが最小で収まるAABBを計算する (オブジェクトが存在しないならフラスタムのAABBを返す)
		Bounds CalculateCasterAABBInLightViewSpace(
			const std::vector<MainData>& casters,
			const Frustum& frustum,
			const MATRIX& light_view
		);
		// キャスターAABBからライトプロジェクション行列を計算する
		MATRIX CalculateLightProjectionMatrix(const Bounds& caster_aabb, const LightShadowData& data);

		// クロップAABBに適応するようにライトビュー行列とライトプロジェクション行列を修正する
		void FixLightViewProjectionMatrix(const Frustum& split_frustum, const std::vector<MainData>& casters, MATRIX& view, MATRIX& proj, MATRIX& LVP);
		// クロップAABBに合うように行列を修正する
		void AdjustCropAABB(const Bounds& crop_aabb, MATRIX& view, MATRIX& proj, MATRIX& LVP);
		// 最終的なLVPでキャスターを求める
		void CalculateCastersWithLVP(const std::vector<MainData>& scene_objects, const int object_num, std::vector<MainData>& casters, MATRIX& view, MATRIX& proj, MATRIX& LVP);
		// 分割範囲ごとにLVPを求める
		void CalculateSplitLVP(int index, const MATRIX& light_view, const std::vector<MainData>& scene_objects, int object_num, const std::shared_ptr<ICamera>& camera, const LightShadowData& data);


		// ==============================================
		// private variables
		// ==============================================
#ifdef USE_IMGUI
		bool m_on_gui_is_init = false;
		int m_on_gui_prev_cascade_num = MAX_CASCADE;
#endif // USE_IMGUI

		// シャドウマップ
		//std::unique_ptr<IColorTarget> m_color_targets[MAX_CASCADE];
		std::unique_ptr<IDepthTarget> m_depth_targets[MAX_CASCADE];
		// LVP用CBuffer
		ConstantBuffer m_cb_LVPs[MAX_CASCADE];
		// 分割視錐台内の影を落とすオブジェクトリスト
		std::vector<MainData> m_casters[MAX_CASCADE];
		// 分割後のライト行列
		MATRIX m_split_view_projection_matrices[MAX_CASCADE];
		MATRIX m_split_view_matrices[MAX_CASCADE];
		MATRIX m_split_projection_matrices[MAX_CASCADE];
		// 分割数(1 <= x <= MAX_CASCADE)
		int m_cascade_num = MAX_CASCADE;
		// 分割平面の位置
		float m_split_positions[MAX_CASCADE + 1] = { 0.0f };
		// シャドウマップに描画されているか
		bool m_is_rendered_target[MAX_CASCADE] = { false };
		// シャドウマップを描画するか
		bool m_draw_shadow_map[MAX_CASCADE] = { false };

		// テストの正確性
		bool m_test_accurate = false;
		// 対数分割スキームと均一分割スキームの適用割合(0 < λ < 1)
		float m_lambda = 0.5f;
		// サンプリング情報
		float m_sampling_radius_uv = 1.0f;
		int m_sampling_count = 1;

		// Splitごとに計算するためのスレッド
		ThreadPool m_calculate_split_LVP_thread = ThreadPool(MAX_CASCADE);
		std::vector<std::future<void>> m_thread_result = std::vector<std::future<void>>();
	};
}

CEREAL_CLASS_VERSION(TKGEngine::PSSM, 2)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::PSSM, "TKGEngine::PSSM")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::ShadowMapBase, TKGEngine::PSSM)