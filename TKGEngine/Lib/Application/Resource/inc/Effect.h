#pragma once

#include "Application/Resource/inc/Texture.h"

#include "Utility/inc/myfunc_vector.h"

#include "../../external/Effekseer/EffekseerRendererDX11.h"

#include <unordered_map>
#include <list>
#include <vector>
#include <string>
#include <memory>

struct ID3D11Device;
struct ID3D11DeviceContext;


namespace TKGEngine
{
	class ICamera;

	class Effect
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Effect();
		virtual ~Effect() = default;
		Effect(const Effect&) = delete;
		Effect& operator=(const Effect& effect);


		static bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);
		static void Destroy();

		//static void RemoveUnused();

		// 歪み描画時のコピーテクスチャのサイズを変更する
		static void OnResizeWindow(int width, int height);

		static void OnFrameUpdate(float unscaled_time, float scaled_time);
		// Deferred Context用フレーム終了処理
		static void OnFrameEnd();
		static void SetCameraParam(const MATRIX& view, const MATRIX& projection);

		static void PauseAll();
		static void ResumeAll();

		bool Load(const std::string& filepath, bool use_unscaled_time);
		void Unload(const std::string& filepath);

		void Render(ID3D11DeviceContext* p_context, const std::shared_ptr<ICamera>& camera);
		void Render(ID3D11DeviceContext* p_context, const std::shared_ptr<ICamera>& camera, const VECTOR3& pos, const Quaternion& rot);

		bool IsExist() const;
		bool IsPlaying() const;

		void Play(const VECTOR3& pos, const Quaternion& rot, float start_time = 0.0f);
		void Stop();
		void StopEmit();
		void Pause();
		void Resume();

		void SetSpeed(const float speed);
		void SetScale(const float x, const float y, const float z);

		void SetTargetPosition(const VECTOR3& pos);
		void SetTargetPosition(const float x, const float y, const float z);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private enum
		// ==============================================
		/// <summary>
		/// エフェクトの更新に使用する時間タイプ
		/// </summary>
		enum class EffectUpdateType
		{
			UnscaledTime = 0,
			ScaledTime = 1,

			Max_EffectUpdateType
		};

		// ==============================================
		// private class
		// ==============================================
		/// <summary>
		/// 歪み描画前に呼ばれるコールバック
		/// </summary>
		class DistortionEffectCallback
			: public EffekseerRenderer::DistortingCallback
		{
		public:
			bool OnDistorting(EffekseerRenderer::Renderer* renderer) override;
			void SetCopyParameter(ID3D11DeviceContext* p_context, ICamera* p_camera)
			{
				context = p_context;
				camera = p_camera;
			}
			
		private:
			ID3D11DeviceContext* context = nullptr;
			ICamera* camera = nullptr;
		};


		// ==============================================
		// private methods
		// ==============================================
		
		/// <summary>
		/// ハンドル配列のactiveなものを先頭側に寄せ、有効な末尾のインデックスを取得する
		/// </summary>
		/// <returns> 有効なハンドル数 </returns>
		int OrganizeHandles();

		/// <summary>
		/// 歪み描画時のコピーテクスチャの作成
		/// </summary>
		static void CreateCopyTexture(int width, int height);

		// ==============================================
		// private variables
		// ==============================================
		// このエンティティが管理するエフェクトのハンドル配列
		std::vector<Effekseer::Handle> m_effekseer_handles{};

		// 管理されているハンドルの末尾のインデックス
		int m_last_handle_index = 0;

		// エンティティ管理リスト内のイテレータ
		std::list<Effect*>::iterator m_entity_ref_cache_iterator;

		// エフェクトデータ参照
		Effekseer::EffectRef m_effekseer_effect = nullptr;

		// 更新をどの経過時間で行うか(実行中の変更不可)
		EffectUpdateType m_update_type = EffectUpdateType::ScaledTime;

		// 歪み描画時のコピーテクスチャ
		static Texture m_copy_texture;
		static DistortionEffectCallback* m_distortion_callbacks[2];
		// Effekseerのマネージャーとレンダラー
		static Effekseer::ManagerRef m_effekseer_managers[static_cast<int>(EffectUpdateType::Max_EffectUpdateType)];
		static EffekseerRendererDX11::RendererRef m_effekseer_renderers[static_cast<int>(EffectUpdateType::Max_EffectUpdateType)];
		// Effect管理用map
		static std::unordered_map<std::string, Effekseer::EffectRef> m_caches[static_cast<int>(EffectUpdateType::Max_EffectUpdateType)];

		// ハンドル配列を更新する必要のあるエンティティを管理するリスト
		static std::list<Effect*> m_entity_ref_caches;
	};


}// namespace TKGEngine