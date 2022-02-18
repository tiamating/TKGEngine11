
#include "../inc/Effect.h"

#include "Application/Objects/Components/interface/ICamera.h"

#include "Systems/inc/StateManager.h"
#include "Systems/inc/TKGEngine_Defined.h"
#include "Systems/inc/Graphics_Defined.h"

#include <cassert>
#include <d3d11.h>

#pragma comment(lib, "Effekseer.lib")
#pragma comment(lib, "EffekseerRendererDX11.lib")

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	Texture Effect::m_copy_texture;
	Effect::DistortionEffectCallback* Effect::m_distortion_callbacks[2] = { nullptr, nullptr };
	Effekseer::ManagerRef Effect::m_effekseer_managers[static_cast<int>(EffectUpdateType::Max_EffectUpdateType)];
	EffekseerRendererDX11::RendererRef Effect::m_effekseer_renderers[static_cast<int>(EffectUpdateType::Max_EffectUpdateType)];
	std::unordered_map<std::string, Effekseer::EffectRef> Effect::m_caches[static_cast<int>(EffectUpdateType::Max_EffectUpdateType)];
	std::list<Effect*> Effect::m_entity_ref_caches;

	// 最大描画スプライト数
	constexpr int MAX_INSTANCE = 20000;

	// ハンドル配列の上限時に追加する要素数
	constexpr int ADD_ARRAY_SIZE = 5;

	// 無効なハンドル値
	constexpr Effekseer::Handle INVALID_HANDLE = -1;

	////////////////////////////////////////////////////////
	// Global Methods
	////////////////////////////////////////////////////////
	Effekseer::Matrix44 MatrixToEfMatrix(const MATRIX& m)
	{
		Effekseer::Matrix44 ret;
		memcpy(ret.Values, m.m, sizeof(m.m));
		return ret;
	}

	Effekseer::Vector3D VECTOR3ToEfVector3D(const VECTOR3& v)
	{
		Effekseer::Vector3D ret;
		ret.X = v.x;
		ret.Y = v.y;
		ret.Z = v.z;
		return ret;
	}


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Effect::Effect()
	{
		m_entity_ref_cache_iterator = std::end(m_entity_ref_caches);
		m_effekseer_handles.resize(2, INVALID_HANDLE);
	}

	Effect& Effect::operator=(const Effect& effect)
	{
		m_effekseer_effect = effect.m_effekseer_effect;
		m_update_type = effect.m_update_type;
		return *this;
	}

	bool Effect::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height)
	{
		// Effekseerレンダラ、マネージャ生成
		for (int i = 0; i < static_cast<int>(EffectUpdateType::Max_EffectUpdateType); ++i)
		{
			// Renderer
			if (m_effekseer_renderers[i] == nullptr)
			{
				m_effekseer_renderers[i] = EffekseerRendererDX11::Renderer::Create(device, context, MAX_INSTANCE, D3D11_COMPARISON_GREATER_EQUAL);
				// 作成に失敗
				if (m_effekseer_renderers[i] == nullptr)
				{
					assert(0 && "failed EffekseerRendererDX11::Renderer::Create()");
					return false;
				}

				// ステートを復帰するか
				m_effekseer_renderers[i]->SetRestorationOfStatesFlag(true);

				// 歪み対応
				m_distortion_callbacks[i] = new DistortionEffectCallback;
				m_effekseer_renderers[i]->SetDistortingCallback(m_distortion_callbacks[i]);
			}
			// Manager
			if (m_effekseer_managers[i] == nullptr)
			{
				m_effekseer_managers[i] = Effekseer::Manager::Create(MAX_INSTANCE, false);
				// 作成に失敗
				if (m_effekseer_managers[i] == nullptr)
				{
					assert(0 && "failed Effekseer::Manager::Create()");
					return false;
				}

				// Effekseerレンダラの各種設定
				m_effekseer_managers[i]->SetSpriteRenderer(m_effekseer_renderers[i]->CreateSpriteRenderer());
				m_effekseer_managers[i]->SetRibbonRenderer(m_effekseer_renderers[i]->CreateRibbonRenderer());
				m_effekseer_managers[i]->SetRingRenderer(m_effekseer_renderers[i]->CreateRingRenderer());
				m_effekseer_managers[i]->SetTrackRenderer(m_effekseer_renderers[i]->CreateTrackRenderer());
				m_effekseer_managers[i]->SetModelRenderer(m_effekseer_renderers[i]->CreateModelRenderer());

				// Effekseer内でのローダー設定
				m_effekseer_managers[i]->SetTextureLoader(m_effekseer_renderers[i]->CreateTextureLoader());
				m_effekseer_managers[i]->SetModelLoader(m_effekseer_renderers[i]->CreateModelLoader());
				m_effekseer_managers[i]->SetMaterialLoader(m_effekseer_renderers[i]->CreateMaterialLoader());
#ifdef AXIS_RH
				// EffekseerをRHで計算する
				m_effekseer_managers[i]->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);
#endif
			}
		}

		// 歪み描画時のコピー用テクスチャ作成
		CreateCopyTexture(width, height);

		return true;
	}

	void Effect::Destroy()
	{
		//// Destroy Effect.
		//const auto itr_end = m_caches.end();
		//for (auto itr = m_caches.begin(); itr != itr_end; ++itr)
		//{
		//	itr->second->Release();
		//}
		//m_caches.clear();
	}

	//void Effect::RemoveUnused()
	//{
	//	const auto itr_end = m_caches.end();
	//	for (auto itr = m_caches.begin(); itr != itr_end;)
	//	{
	//		if (itr->second.use_count() <= 1)
	//		{
	//			itr->second->Release();
	//			itr = m_caches.erase(itr);
	//			continue;
	//		}
	//		++itr;
	//	}
	//}

	void Effect::OnResizeWindow(int width, int height)
	{
		CreateCopyTexture(width, height);
	}

	void Effect::OnFrameUpdate(float unscaled_time, float scaled_time)
	{
		// エフェクトマネージャーの更新
		m_effekseer_managers[static_cast<int>(EffectUpdateType::UnscaledTime)]->Flip();
		m_effekseer_managers[static_cast<int>(EffectUpdateType::ScaledTime)]->Flip();
		m_effekseer_managers[static_cast<int>(EffectUpdateType::UnscaledTime)]->Update(unscaled_time * 60.0f);
		m_effekseer_managers[static_cast<int>(EffectUpdateType::ScaledTime)]->Update(scaled_time * 60.0f);

		// 管理しているエフェクト配列の整頓関数を呼ぶ
		{
			const auto itr_end = m_entity_ref_caches.end();
			auto itr = m_entity_ref_caches.begin();
			while (itr != itr_end)
			{
				auto* p_effect = *itr;

				// 整頓関数を呼ぶ
				const int num_active_handle = p_effect->OrganizeHandles();

				// 実行中のハンドルが存在しないとき、リストから取り除く
				if (num_active_handle <= 0)
				{
					// リストから消去
					itr = m_entity_ref_caches.erase(p_effect->m_entity_ref_cache_iterator);

					// メンバ変数を初期化
					p_effect->m_entity_ref_cache_iterator = itr_end;
					p_effect->m_last_handle_index = 0;

					// ループを抜ける
					continue;
				}

				// イテレータを進める
				++itr;
			}
		}
	}

	void Effect::OnFrameEnd()
	{
		m_effekseer_renderers[static_cast<int>(EffectUpdateType::UnscaledTime)]->ResetStateForDefferedContext();
		m_effekseer_renderers[static_cast<int>(EffectUpdateType::ScaledTime)]->ResetStateForDefferedContext();
	}

	void Effect::SetCameraParam(const MATRIX& view, const MATRIX& projection)
	{
		m_effekseer_renderers[static_cast<int>(EffectUpdateType::UnscaledTime)]->SetCameraMatrix(MatrixToEfMatrix(view));
		m_effekseer_renderers[static_cast<int>(EffectUpdateType::UnscaledTime)]->SetProjectionMatrix(MatrixToEfMatrix(projection));

		m_effekseer_renderers[static_cast<int>(EffectUpdateType::ScaledTime)]->SetCameraMatrix(MatrixToEfMatrix(view));
		m_effekseer_renderers[static_cast<int>(EffectUpdateType::ScaledTime)]->SetProjectionMatrix(MatrixToEfMatrix(projection));
	}

	void Effect::PauseAll()
	{
		m_effekseer_managers[static_cast<int>(EffectUpdateType::UnscaledTime)]->SetPausedToAllEffects(true);
		m_effekseer_managers[static_cast<int>(EffectUpdateType::ScaledTime)]->SetPausedToAllEffects(true);
	}

	void Effect::ResumeAll()
	{
		m_effekseer_managers[static_cast<int>(EffectUpdateType::UnscaledTime)]->SetPausedToAllEffects(false);
		m_effekseer_managers[static_cast<int>(EffectUpdateType::ScaledTime)]->SetPausedToAllEffects(false);
	}

	bool Effect::Load(const std::string& filepath, bool use_unscaled_time)
	{
		const int type_index = static_cast<int>(use_unscaled_time ? EffectUpdateType::UnscaledTime : EffectUpdateType::ScaledTime);
		m_update_type = use_unscaled_time ? EffectUpdateType::UnscaledTime : EffectUpdateType::ScaledTime;

		// 既にロードされているかチェック
		auto itr_find = m_caches[type_index].find(filepath);
		// 見つかった場合
		if (itr_find != m_caches[type_index].end())
		{
			m_effekseer_effect = itr_find->second;
			return true;
		}
		// 見つからない場合
		char16_t utf16_filename[256];
		Effekseer::ConvertUtf8ToUtf16(utf16_filename, 256, filepath.c_str());
		m_effekseer_effect = Effekseer::Effect::Create(m_effekseer_managers[type_index], static_cast<EFK_CHAR*>(utf16_filename));
		// ロード失敗時
		if (m_effekseer_effect == nullptr)
		{
			return false;
		}
		// ロード成功
		m_caches[type_index].emplace(filepath, m_effekseer_effect);
		return true;
	}

	void Effect::Unload(const std::string& filepath)
	{
		m_caches[static_cast<int>(m_update_type)].erase(filepath);
		m_entity_ref_cache_iterator = std::end(m_entity_ref_caches);
		m_effekseer_handles.clear();
		m_last_handle_index = 0;
	}

	void Effect::Render(ID3D11DeviceContext* p_context, const std::shared_ptr<ICamera>& camera)
	{
		const int array_index = static_cast<int>(m_update_type);

		m_effekseer_renderers[array_index]->BeginRendering();
		m_distortion_callbacks[array_index]->SetCopyParameter(p_context, camera.get());
		for (int i = 0; i < m_last_handle_index; ++i)
		{
			// 早期リターン
			if (m_effekseer_handles.at(i) < 0)
				continue;

			// Draw effect
			m_effekseer_managers[array_index]->DrawHandle(m_effekseer_handles.at(i));
		}
		m_effekseer_renderers[array_index]->EndRendering();
	}

	void Effect::Render(ID3D11DeviceContext* p_context, const std::shared_ptr<ICamera>& camera, const VECTOR3& pos, const Quaternion& rot)
	{
		const int array_index = static_cast<int>(m_update_type);

		m_effekseer_renderers[array_index]->BeginRendering();
		m_distortion_callbacks[array_index]->SetCopyParameter(p_context, camera.get());
		for (int i = 0; i < m_last_handle_index; ++i)
		{
			// 早期リターン
			if (m_effekseer_handles.at(i) < 0)
				continue;

			// Set Position and Rotation.
			m_effekseer_managers[array_index]->SetLocation(m_effekseer_handles.at(i), VECTOR3ToEfVector3D(pos));
			VECTOR3 axis;
			float angle;
			rot.ToAxisAngle(axis, angle);
			m_effekseer_managers[array_index]->SetRotation(m_effekseer_handles.at(i), VECTOR3ToEfVector3D(axis), MyMath::AngleToRadian(angle));

			// Draw effect
			m_effekseer_managers[array_index]->DrawHandle(m_effekseer_handles.at(i));
		}
		m_effekseer_renderers[array_index]->EndRendering();
	}

	bool Effect::IsExist() const
	{
		const int array_index = static_cast<int>(m_update_type);
		for (auto& handle : m_effekseer_handles)
		{
			// 1つでも存在しているならtrue
			if (m_effekseer_managers[array_index]->Exists(handle))
				return true;
		}
		return false;
	}

	bool Effect::IsPlaying() const
	{
		const int array_index = static_cast<int>(m_update_type);
		for (auto& handle : m_effekseer_handles)
		{
			// 1つでもPlay状態ならtrue
			if (!m_effekseer_managers[array_index]->GetPaused(handle))
				return true;
		}
		return false;
	}

	void Effect::Play(const VECTOR3& pos, const Quaternion& rot, float start_time)
	{
		if (start_time < 0.0f)
			start_time = 0.0f;

		// 現在の位置と姿勢でエフェクトを生成する
		{
			const int array_index = static_cast<int>(m_update_type);
			VECTOR3 axis;
			float angle;
			rot.ToAxisAngle(axis, angle);

			// ハンドル配列のサイズを超える場合は要素数を拡張する
			{
				const int current_array_size = static_cast<int>(m_effekseer_handles.size());
				if (current_array_size <= m_last_handle_index)
				{
					m_effekseer_handles.resize(current_array_size + ADD_ARRAY_SIZE, INVALID_HANDLE);
				}
			}
			// 現在の配列末尾に生成
			m_effekseer_handles.at(m_last_handle_index)
				= m_effekseer_managers[array_index]->Play(m_effekseer_effect, VECTOR3ToEfVector3D(pos), static_cast<int32_t>(start_time * 60.0f));
			m_effekseer_managers[array_index]->SetRotation(m_effekseer_handles.at(m_last_handle_index), VECTOR3ToEfVector3D(axis), MyMath::AngleToRadian(angle));
			//m_effekseer_managers[array_index]->Flip();

			// 末尾の位置を1つずらす
			++m_last_handle_index;
		}

		// 既に登録されている場合はreturn
		if (m_entity_ref_cache_iterator != m_entity_ref_caches.end())
		{
			return;
		}
		// 整頓関数を呼ぶためのリストに登録する
		m_entity_ref_caches.emplace_back(this);

		// 登録した位置を指すイテレータを取得
		m_entity_ref_cache_iterator = std::end(m_entity_ref_caches);
		std::advance(m_entity_ref_cache_iterator, -1);
	}

	void Effect::Stop()
	{
		const int array_index = static_cast<int>(m_update_type);

		// 管理するすべての実行中エフェクトを停止する
		for (int i = 0; i < m_last_handle_index; ++i)
		{
			// 早期リターン
			{
				// 無効なハンドル
				if (m_effekseer_handles.at(i) < 0)
					continue;
			}
			// エフェクトの停止
			m_effekseer_managers[array_index]->StopEffect(m_effekseer_handles.at(i));
			m_effekseer_handles.at(i) = INVALID_HANDLE;
		}

		// 実行中エンティティリストから消去する
		{
			if (m_entity_ref_cache_iterator == m_entity_ref_caches.end())
				return;

			m_entity_ref_caches.erase(m_entity_ref_cache_iterator);
			m_entity_ref_cache_iterator = m_entity_ref_caches.end();
		}
	}

	void Effect::StopEmit()
	{
		const int array_index = static_cast<int>(m_update_type);

		// 管理するすべての実行中エフェクトの発生を停止する
		for (int i = 0; i < m_last_handle_index; ++i)
		{
			// 早期リターン
			{
				// 無効なハンドル
				if (m_effekseer_handles.at(i) < 0)
					continue;
			}
			// エフェクトの発生の停止
			m_effekseer_managers[array_index]->StopRoot(m_effekseer_handles.at(i));
		}
	}

	void Effect::Pause()
	{
		const int array_index = static_cast<int>(m_update_type);

		// 管理するすべての実行中エフェクトを一時停止する
		for (int i = 0; i < m_last_handle_index; ++i)
		{
			// 早期リターン
			{
				// 無効なハンドル
				if (m_effekseer_handles.at(i) < 0)
					continue;
			}
			// エフェクトの一時停止
			m_effekseer_managers[array_index]->SetPaused(m_effekseer_handles.at(i), true);
		}
	}

	void Effect::Resume()
	{
		const int array_index = static_cast<int>(m_update_type);

		// 管理するすべての実行中エフェクトを再開する
		for (int i = 0; i < m_last_handle_index; ++i)
		{
			// 早期リターン
			{
				// 無効なハンドル
				if (m_effekseer_handles.at(i) < 0)
					continue;
			}
			// エフェクトの再開
			m_effekseer_managers[array_index]->SetPaused(m_effekseer_handles.at(i), false);
		}
	}

	void Effect::SetSpeed(const float speed)
	{
		const int array_index = static_cast<int>(m_update_type);

		// 管理するすべての実行中エフェクトの速さを変更する
		for (int i = 0; i < m_last_handle_index; ++i)
		{
			// 早期リターン
			{
				// 無効なハンドル
				if (m_effekseer_handles.at(i) < 0)
					continue;
			}
			// エフェクトの速さを変更する
			m_effekseer_managers[array_index]->SetSpeed(
				m_effekseer_handles.at(i),
				speed < 0.0f ? 0.0f : speed
			);
		}
	}

	void Effect::SetScale(const float x, const float y, const float z)
	{
		const int array_index = static_cast<int>(m_update_type);

		// 管理するすべての実行中エフェクトのスケールを変更する
		for (int i = 0; i < m_last_handle_index; ++i)
		{
			// 早期リターン
			{
				// 無効なハンドル
				if (m_effekseer_handles.at(i) < 0)
					continue;
			}
			// エフェクトのスケールを変更する
			m_effekseer_managers[array_index]->SetScale(m_effekseer_handles.at(i), x, y, z);
		}
	}

	void Effect::SetTargetPosition(const VECTOR3& pos)
	{
		SetTargetPosition(pos.x, pos.y, pos.z);
	}

	void Effect::SetTargetPosition(const float x, const float y, const float z)
	{
		const int array_index = static_cast<int>(m_update_type);

		// 管理するすべての実行中エフェクトのターゲット位置を変更する
		for (int i = 0; i < m_last_handle_index; ++i)
		{
			// 早期リターン
			{
				// 無効なハンドル
				if (m_effekseer_handles.at(i) < 0)
					continue;
			}
			// エフェクトのターゲット位置を変更する
			m_effekseer_managers[array_index]->SetTargetLocation(m_effekseer_handles.at(i), Effekseer::Vector3D(x, y, z));
		}
	}

	int Effect::OrganizeHandles()
	{
		int num_active_handle = 0;
		const int array_index = static_cast<int>(m_update_type);

		for (int i = 0; i < m_last_handle_index; ++i)
		{
			// アクティブなEffectかどうかチェック
			// アクティブなら次へいく
			if (m_effekseer_managers[array_index]->Exists(m_effekseer_handles.at(i)))
			{
				++num_active_handle;
				continue;
			}
			// 非アクティブならアクティブなHandleと入れ替える
			else
			{
				// 初期化のために無効なハンドル値を代入する
				m_effekseer_handles.at(i) = INVALID_HANDLE;

				// 有効な要素が減ったので範囲を動かす
				--m_last_handle_index;

				// 自身より後ろにアクティブなEffectが存在しなくなるまでループ
				while (i < m_last_handle_index)
				{
					// 末尾のひとつ前の要素がアクティブかチェック
					// アクティブなら要素を入れ替えて範囲を1つずらして次の要素に行く
					if (m_effekseer_managers[array_index]->Exists(m_effekseer_handles.at(m_last_handle_index)) == true)
					{
						// 有効な要素数をインクリメントする
						++num_active_handle;

						// 要素を入れ替える
						m_effekseer_handles.at(i) = m_effekseer_handles.at(m_last_handle_index);
						m_effekseer_handles.at(m_last_handle_index) = INVALID_HANDLE;

						// 次の要素に行く
						break;
					}
					// 非アクティブなら
					// 無効なハンドル値を代入して、範囲を動かす
					else
					{
						// 無効なハンドル値を代入する
						m_effekseer_handles.at(m_last_handle_index) = INVALID_HANDLE;

						// 範囲を動かす
						--m_last_handle_index;
					}
				}// ~while
			}
		}// ~for

		return num_active_handle;
	}

	void Effect::CreateCopyTexture(int width, int height)
	{
		TEX_DESC desc;
		desc.width = width;
		desc.height = height;
		desc.depth = 1;
		desc.array_size = 1;
		desc.mip_levels = 1;
		desc.format = Graphics::g_color_format;
		desc.option = TEXTURE_OPTION::TEXTURE_OPTION_NONE;
		desc.dimension = TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D;
		m_copy_texture.Create(desc, true, false, nullptr);
	}

	bool Effect::DistortionEffectCallback::OnDistorting(EffekseerRenderer::Renderer* renderer)
	{
		if (m_copy_texture.HasTexture())
		{
			camera->CopyColorTarget(context, m_copy_texture);
			reinterpret_cast<EffekseerRendererDX11::Renderer*>(renderer)->SetBackground(m_copy_texture.GetSRV());

			return true;
		}
		return false;
	}

}// namespace TKGEngine