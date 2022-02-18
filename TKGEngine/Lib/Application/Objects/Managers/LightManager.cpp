

#include "LightManager.h"

#include "Components/inc/CTransform.h"
#include "Components/interface/ICamera.h"
#include "Components/inc/CLight.h"
#include "Systems/inc/LogSystem.h"
#include "Systems/inc/IGraphics.h"

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	LightManager* LightManager::m_instance = nullptr;

	static constexpr int START_SHADOW_BUFFER_SIZE = 1000;
	static constexpr int ADD_SHADOW_BUFFER_SIZE = 500;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////

	bool LightManager::Create()
	{
		if (!m_instance)
		{
			m_instance = new LightManager;
			return m_instance->Initialize();
		}

		assert(0 && "LightManager::Create() has called twice.");
		return false;
	}

	void LightManager::Destroy()
	{
		if (m_instance)
		{
			m_instance->Finalize();
			delete m_instance;
			m_instance = nullptr;
		}
	}

	LightManager* LightManager::GetInstance()
	{
		return m_instance;
	}

	void LightManager::RegisterManager(const std::shared_ptr<Light>& light)
	{
		if (!m_instance)
			return;
		m_instance->m_light_list.emplace(light->GetOwnerID(), light);
	}

	void LightManager::UnregisterManager(const GameObjectID owner_id)
	{
		if (!m_instance)
			return;
		m_instance->m_light_list.erase(owner_id);
	}


	void LightManager::SortSceneLight()
	{
		if (m_instance)
		{
			m_instance->SortLightType();
		}
	}

	void LightManager::CalculateSceneLight(const std::vector<MainData>& scene_objects, const int object_num, const std::shared_ptr<ICamera>& camera)
	{
		if (m_instance)
		{
			m_instance->OnCalculateSceneLight(scene_objects, object_num, camera);
		}
	}

	void LightManager::RenderShadow(ID3D11DeviceContext* context)
	{
		if (m_instance)
		{
			m_instance->OnRenderShadow(context);
		}
	}

	void LightManager::SetPipeline(ID3D11DeviceContext* context)
	{
		if (m_instance)
		{
			m_instance->OnSetPipeline(context);
		}
	}

	bool LightManager::Initialize()
	{
		// スレッドのサイズ確保
		m_thread_result.reserve(MAX_THREAD_NUM);

		// CBufferの作成
		if (!m_cb_light.Create(sizeof(CB_Light), false))
		{
			assert(0 && "failed ConstantBuffer::Create(CB_Light) LightManager::Initialize()");
			return false;
		}
		// インスタンスバッファの作成
		m_current_buffer_size = START_SHADOW_BUFFER_SIZE;
		if (!m_instance_buffer.Create(
			nullptr,
			static_cast<int>(sizeof(MainInstance)),
			START_SHADOW_BUFFER_SIZE,
			VERTEX_ELEMENT_TYPE::MAIN_INSTANCE,
			BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC))
		{
			assert(0 && "failed VertexBuffer::Create() LightManager::Initialize()");
			return false;
		}

		return true;
	}

	void LightManager::Finalize()
	{
		/* nothing */
	}

	void LightManager::SortLightType()
	{
		// リストのクリア
		{
			m_directional_list.clear();
			m_point_list.clear();
			m_spot_list.clear();
		}

		// ライトタイプごとに分ける
		for (auto& list_data : m_light_list)
		{
			auto& light = list_data.second;

			// ライトがアクティブか
			if (!light->IsActiveBehaviour())
				continue;

			switch (light->GetType())
			{
				case LightType::Directional:
					// 1つしか平行光源は存在しない
					if (m_directional_list.size() >= 1)
					{
						LOG_ASSERT("There is no more than one directional light.");
					}
					else
					{
						m_directional_list.emplace_back(light);
					}
					break;
				case LightType::Point:
					// ポイントライトの最大数チェック
					if (m_point_list.size() >= MAX_POINTLIGHT)
					{
						LOG_ASSERT("There is no more than MAX_POINTLIGHT point light.");
					}
					else
					{
						m_point_list.emplace_back(light);
					}
					break;
				case LightType::Spot:
					// スポットライトの最大数チェック
					if (m_spot_list.size() >= MAX_SPOTLIGHT)
					{
						LOG_ASSERT("There is no more than MAX_SPOTLIGHT spot light.");
					}
					else
					{
						m_spot_list.emplace_back(light);
					}
					break;
			}
		}
	}

	void LightManager::OnApplyParameter()
	{
		// 平行光源はシーン内に1つしか有効にできない
		if (m_directional_list.size() > 1)
		{
			LOG_ASSERT("Current scenes have many directional light(Num : %d)", m_directional_list.size());
		}

		// CBufferにセット
		{
			CB_Light cb = {};
			// Directional
			for (const auto& dir : m_directional_list)
			{
				// データセット
				cb.directional = dir;
				break;
			}
			// Spot
			{
				auto itr = m_spot_list.begin();
				const auto itr_end = m_spot_list.end();
				for (int i = 0; i < MAX_SPOTLIGHT; ++i)
				{
					if (itr == itr_end)
						break;
					// データセット
					cb.spots[i] = *itr;
					++itr;
				}
			}
			// Point
			{
				auto itr = m_point_list.begin();
				const auto itr_end = m_point_list.end();
				for (int i = 0; i < MAX_POINTLIGHT; ++i)
				{
					if (itr == itr_end)
						break;
					// データセット
					cb.points[i] = *itr;
					++itr;
				}
			}
			// パラメータの更新
			m_cb_light.SetParam(&cb, sizeof(cb));
		}
	}

	void LightManager::OnCalculateSceneLight(const std::vector<MainData>& scene_objects, const int object_num, const std::shared_ptr<ICamera>& camera)
	{
		// ライトごとにスレッドを立ててCasterを計算する
		{
			// Directional
			OnCalculateCaster(m_directional_list, scene_objects, object_num, camera);
			// Spot
			OnCalculateCaster(m_spot_list, scene_objects, object_num, camera);
			// Point
			OnCalculateCaster(m_point_list, scene_objects, object_num, camera);
		}
		// 計算終了を待機
		int draw_instance_count = 0;
		for (auto& result : m_thread_result)
		{
			if (!result.valid())
				continue;
			draw_instance_count += result.get();
		}
		m_thread_result.clear();

		// インスタンスバッファのリサイズチェック
		{
			bool change_buffer_size = false;
			while (draw_instance_count > m_current_buffer_size)
			{
				change_buffer_size = true;
				m_current_buffer_size += ADD_SHADOW_BUFFER_SIZE;
			}
			// サイズ変更が必要なら拡張
			if(change_buffer_size)
			{
				// インスタンスバッファ拡張
				if (!m_instance_buffer.Resize(static_cast<int>(sizeof(MainInstance)), m_current_buffer_size))
				{
					assert(0 && "failed InstanceBuffer::Resize(). LightManager::OnCalculateSceneLight()");
					return;
				}
			}
		}
		// ライト用定数バッファを適用する
		OnApplyParameter();
	}

	void LightManager::OnCalculateCaster(const std::list<std::shared_ptr<Light>>& light_list, const std::vector<MainData>& scene_objects, const int object_num, const std::shared_ptr<ICamera>& camera)
	{
		for (const auto& light : light_list)
		{
			// 影を描画するか
			if (!light->GetRenderShadow())
				continue;
			// 影を描画するオブジェクトの計算
			const auto func = [&](const std::vector<MainData>& objs, const int num, const std::shared_ptr<ICamera>& cam) { return light->CalculateShadowCaster(objs, num, cam); };
			m_thread_result.emplace_back(m_calculate_shadow_caster_thread.Add(func, scene_objects, object_num, camera));
		}
	}

	void LightManager::OnRenderShadow(ID3D11DeviceContext* context)
	{
		// ライトごとに影を描画する
		{
			// Directional
			OnRenderShadowCheckAndRender(m_directional_list, context);
			// Spot
			OnRenderShadowCheckAndRender(m_spot_list, context);
			// Point
			OnRenderShadowCheckAndRender(m_point_list, context);
		}
	}

	void LightManager::OnRenderShadowCheckAndRender(const std::list<std::shared_ptr<Light>>& light_list, ID3D11DeviceContext* context)
	{
		const auto itr_end = light_list.end();
		for (auto itr = light_list.begin(); itr != itr_end; ++itr)
		{
			const auto& light = *itr;
			// 影を描画するオブジェクトがあるか
			if (!light->IsRenderShadowCaster())
				continue;
			// 影描画の実行
			light->RenderShadowCaster(context, m_instance_buffer);
		}
	}

	void LightManager::OnSetPipeline(ID3D11DeviceContext* context)
	{
		// CBufferの更新
		m_cb_light.SetVS(context, CBS_LIGHT);
		m_cb_light.SetPS(context, CBS_LIGHT);

		// Shadowテクスチャとライトマスクテクスチャのセット
		SetTexture(context, m_directional_list);
		SetTexture(context, m_spot_list);
		SetTexture(context, m_point_list);
	}

	void LightManager::SetTexture(ID3D11DeviceContext* context, const std::list<std::shared_ptr<Light>>& light_list)
	{
		const auto itr_end = light_list.end();
		int index = -1;
		for (auto itr = light_list.begin(); itr != itr_end; ++itr)
		{
			++index;
			const auto& light = *itr;
			// ライトごとにシャドウマップをセットする
			switch (light->GetType())
			{
				case LightType::Directional:
					OnSetTextureDirectional(context, light, index);
					break;
				case LightType::Spot:
					OnSetTextureSpot(context, light, index);
					break;
				case LightType::Point:
					OnSetTexturePoint(context, light, index);
					break;
			}
		}
	}

	void LightManager::OnSetTextureDirectional(ID3D11DeviceContext* context, const std::shared_ptr<Light>& light, const int index)
	{
		assert(index == 0 && "Directional Light is max one.");

		// 分割シャドウ分セットする
		const int split_num = light->GetCascadeNum();
		for (int i = 0; i < split_num; ++i)
		{
			light->SetSRV(context, TEXSLOT_SHADOWMAP_CASCADE0 + i, i);
		}
	}

	void LightManager::OnSetTextureSpot(ID3D11DeviceContext* context, const std::shared_ptr<Light>& light, const int index)
	{
		// シャドウのセット
		light->SetSRV(context, TEXSLOT_SHADOWMAP_SPOT0 + index);
		// マスクテクスチャのセット
		light->SetLightMaskTexture(context, TEXSLOT_SPOT_MASK0 + index);
	}

	void LightManager::OnSetTexturePoint(ID3D11DeviceContext* context, const std::shared_ptr<Light>& light, const int index)
	{
		// TODO : ポイントライトのシャドウ対応
	}

	void LightManager::DirectionalLight::operator=(const std::shared_ptr<Light>& light)
	{
		// パラメータの代入
		for (int i = 0; i < MAX_CASCADE; ++i)
		{
			cascade_LVPs[i] = light->GetLVP(i);
			cascade_light_views[i] = light->GetLV(i);
			cascade_light_projections[i] = light->GetLP(i);
			shadow_size[i] = light->GetShadowSize(i);
			inv_shadow_size[i] = 1.0f / shadow_size[i];
			has_caster[i] = light->IsRenderShadowCaster(i);
		}
		color = light->GetColor();
		direction = light->GetTransform()->Forward();
		use_shadow_map = light->GetRenderShadow();
		strength = light->GetStrength();
		bias = light->GetBias();
		normal_bias = light->GetNormalBias();
		cascade_num = light->GetCascadeNum();
		filter_radius_uv = light->GetSamplingRadius();
		sampling_count = light->GetSamplingCount();
	}

	void LightManager::SpotLight::operator=(const std::shared_ptr<Light>& light)
	{
		const auto transform = light->GetTransform();

		// パラメータの代入
		LVP = light->GetLVP();
		light_view = light->GetLV();
		light_projection = light->GetLP();
		shadow_size = light->GetShadowSize();
		inv_shadow_size = 1.0f / shadow_size;
		pos = VECTOR4(transform->Position(), 1.0f);
		color = light->GetColor();
		direction = transform->Forward();
		range = light->GetRange();
		attenuation = light->GetAttenuation();
		inner_cone_cos = cosf(MyMath::AngleToRadian(light->GetSpotAngleInner()) * 0.5f);
		outer_cone_cos = cosf(MyMath::AngleToRadian(light->GetSpotAngleOuter()) * 0.5f);
		falloff = light->GetFalloff();
		use_light_mask = light->UseLightMask();
		use_shadow_map = light->GetRenderShadow();
		strength = light->GetStrength();
		bias = light->GetBias();
		normal_bias = light->GetNormalBias();
	}

	void LightManager::PointLight::operator=(const std::shared_ptr<Light>& light)
	{
		// LVPの取得
		// TODO : ポイントライトは現在、影を描画しない

		// パラメータの代入
		pos = VECTOR4(light->GetTransform()->Position(), 1.0f);
		color = light->GetColor();
		attenuation = light->GetAttenuation();
		range = light->GetRange();
		use_shadow_map = FALSE;// light->DrawShadow();
		strength = light->GetStrength();
		bias = light->GetBias();
		normal_bias = light->GetNormalBias();
	}

}// namespace TKGEngine