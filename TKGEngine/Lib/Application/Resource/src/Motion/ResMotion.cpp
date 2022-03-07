
#include "IResMotion.h"

#include "Systems/inc/AssetSystem.h"

#include "../../inc/ConstantBuffer.h"

#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/myfunc_file.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <regex>

#include <d3d11.h>
#include <wrl.h>
#include <cassert>

#ifdef USE_IMGUI
#include <fbxsdk.h>
#endif// USE_IMGUI

namespace TKGEngine
{
	class ResMotion
		: public IResMotion
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResMotion() = default;
		virtual ~ResMotion() = default;
		ResMotion(const ResMotion&) = delete;
		ResMotion& operator=(const ResMotion&) = delete;

#ifdef USE_IMGUI
		static std::vector<std::shared_ptr<IResMotion>> CreateFromFBX(
			const std::vector<std::string>& filepathes,
			FbxScene* fbx_scene,
			std::vector<std::string>& motion_names,
			std::unordered_map<std::string, int>& bone_name_index
		);
#endif// USE_IMGUI

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override {}
#endif // USE_IMGUI
		// ~AssetDataBase


		float GetSampleRate() const override;
		float GetMotionLength() const override;

		std::vector<Animations::KeyData>* GetKeyFrame(const int frame) override;
		std::unordered_map<std::string, int>* GetKeyIndex() override;


		// ==============================================
		// public variables
		// ==============================================



	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<IResMotion>(this),
					CEREAL_NVP(m_animation)
				);
			}
			else if (version == 2)
			{
				archive(
					cereal::base_class<IResMotion>(this),
					CEREAL_NVP(m_animation),
					CEREAL_NVP(m_key_index)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		void SetAsyncOnLoad() override;
		void OnLoad() override;

#ifdef USE_IMGUI
		void Save(const std::string& filepath);
#endif// USE_IMGUI


		// use GUI

		// ~use GUI


		// ==============================================
		// private variables
		// ==============================================
		Animations::Animation m_animation;
		std::unordered_map<std::string, int> m_key_index;
	};


	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	ResourceManager<IResMotion> IResMotion::m_caches;
	std::mutex IResMotion::m_cache_mutex;


	////////////////////////////////////////////////////////
	// Local Methods
	////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	inline void FbxAMatrixToMATRIX(const FbxAMatrix& fbx_matrix, MATRIX& matrix)
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				matrix.m[row][column] = static_cast<float>(fbx_matrix[row][column]);
			}
		}
	}
	inline VECTOR3 FbxDouble4ToVECTOR3(const FbxDouble4& fbx_vector)
	{
		return VECTOR3(
			static_cast<float>(fbx_vector[0]),
			static_cast<float>(fbx_vector[1]),
			static_cast<float>(fbx_vector[2])
		);
	}
	inline VECTOR4 FbxDouble4ToVECTOR4(const FbxDouble4& fbx_vector)
	{
		return VECTOR4(
			static_cast<float>(fbx_vector[0]),
			static_cast<float>(fbx_vector[1]),
			static_cast<float>(fbx_vector[2]),
			static_cast<float>(fbx_vector[3])
		);
	}
#endif// USE_IMGUI


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	// IResMotion
	std::shared_ptr<IResMotion> IResMotion::CreateInterface()
	{
		return std::make_shared<ResMotion>();
	}

	std::shared_ptr<IResMotion> IResMotion::Load(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResMotion> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResMotion> res_new = IResMotion::CreateInterface();
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Load Resource
		res_new->m_load_state_mutex.lock();
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnLoad();
		if (!res_new->HasResource())
		{
			res_new->Release();
			res_new->m_load_state_mutex.unlock();
			res_new.reset();
			return std::shared_ptr<IResMotion>();
		}
		res_new->m_load_state_mutex.unlock();
		return res_new;
	}

	std::shared_ptr<IResMotion> IResMotion::LoadAsync(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResMotion> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResMotion> res_new(IResMotion::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResMotion::SetAsyncOnLoad, res_new));

		return res_new;
	}

	void IResMotion::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

#ifdef USE_IMGUI
	void IResMotion::CreateBinaryFromFBX(
		const std::vector<std::string>& filepathes,
		FbxScene* fbx_scene,
		std::vector<std::string>& motion_names,
		std::unordered_map<std::string, int>& bone_name_index
	)
	{
		const int motion_cnt = static_cast<int>(motion_names.size());

		std::lock_guard<std::mutex> lock(m_cache_mutex);
		auto&& s_ptr_list = ResMotion::CreateFromFBX(filepathes, fbx_scene, motion_names, bone_name_index);
		for (int i = 0; i < motion_cnt; ++i)
		{
			if (!s_ptr_list.at(i))
				return;

			if (m_caches.Search(filepathes.at(i)))
			{
				m_caches.RemoveCache(filepathes.at(i));
			}
			m_caches.Set(filepathes.at(i), s_ptr_list.at(i));
		}
	}

	std::vector<std::shared_ptr<IResMotion>> ResMotion::CreateFromFBX(
		const std::vector<std::string>& filepathes,
		FbxScene* fbx_scene,
		std::vector<std::string>& motion_names,
		std::unordered_map<std::string, int>& bone_name_index
	)
	{
		using namespace fbxsdk;

		const int motion_cnt = static_cast<int>(motion_names.size());
		//int current_motion_num = 0;

		std::vector<std::shared_ptr<ResMotion>> p_motions(motion_cnt);
		for (auto&& p_motion : p_motions)
		{
			p_motion = std::make_shared<ResMotion>();
		}

		// 全てのアニメーション名を取得
		FbxArray<FbxString*> fbx_animation_stack_names;
		fbx_scene->FillAnimStackNameArray(fbx_animation_stack_names);

		const int fbx_animation_cnt = fbx_animation_stack_names.Size();
		for (int fbx_animation_idx = 0; fbx_animation_idx < fbx_animation_cnt; ++fbx_animation_idx)
		{
			FbxString* fbx_animation_name = fbx_animation_stack_names.GetAt(fbx_animation_idx);
			int filename_idx = 0;	// 引数の要素番号
			{
				std::string name_tester = fbx_animation_name->Buffer();
				{
					bool find_name = false;
					for (auto&& name : motion_names)
					{
						if (name == name_tester)
						{
							find_name = true;
							break;
						}
						++filename_idx;
					}
					// ロードするアニメーション名が見つからない場合は次のループへ
					if (!find_name)
					{
						continue;
					}
				}
			}

			auto& animation = p_motions.at(filename_idx)->m_animation;

			// アニメーションデータのサンプリング設定
			const FbxTime::EMode fbx_time_mode = fbx_scene->GetGlobalSettings().GetTimeMode();
			FbxTime fbx_frame_time;
			fbx_frame_time.SetTime(0, 0, 0, 1, 0, fbx_time_mode);

			const float sampling_rate = static_cast<float>(fbx_frame_time.GetFrameRate(fbx_time_mode));
			animation.sampling_rate = sampling_rate;
			const float sampling_time = 1.0f / sampling_rate;

			FbxAnimStack* fbx_anim_stack = fbx_scene->FindMember<FbxAnimStack>(fbx_animation_name->Buffer());

			// 再生するアニメーションを指定する
			fbx_scene->SetCurrentAnimationStack(fbx_anim_stack);

			// アニメーションの再生開始時間と再生終了時間を取得する
			const FbxTakeInfo* fbx_take_info = fbx_scene->GetTakeInfo(fbx_animation_name->Buffer());
			// StartTimeが負の場合にTポーズが再生されてしまうので0以上にクランプする
			FbxTime fbx_start_time = fbx_take_info->mLocalTimeSpan.GetStart();
			if (fbx_start_time.Get() < 0)
			{
				fbx_start_time = 0;
			}
			FbxTime fbx_end_time = fbx_take_info->mLocalTimeSpan.GetStop();

			// TODO : 抽出するデータは60FPS基準でサンプリングする
			FbxTime fbx_sampling_step;
			fbx_sampling_step.SetTime(0, 0, 1, 0, 0, fbx_time_mode);
			fbx_sampling_step = static_cast<FbxLongLong>(fbx_sampling_step.Get() * static_cast<double>(sampling_time));

			int start_frame = static_cast<int>(fbx_start_time.Get() / fbx_sampling_step.Get());
			int end_frame = static_cast<int>(fbx_end_time.Get() / fbx_sampling_step.Get());
			const int frame_count = static_cast<int>((fbx_end_time.Get() - fbx_start_time.Get()) / fbx_sampling_step.Get());

			// アニメーションの対象となるノードを列挙する
			std::vector<FbxNode*> fbx_nodes;
			// ルートノードから再帰的に検索
			{
				FbxNode* root_node = fbx_scene->GetRootNode();
				static std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
					if (!node)
					{
						return;
					}
					fbx_nodes.emplace_back(node);
					const int child_cnt = node->GetChildCount();
					for (int i = 0; i < child_cnt; ++i)
					{
						traverse(node->GetChild(i));
					}
				};
				traverse(root_node);
			}

			// アニメーションデータを抽出する
			animation.seconds_length = frame_count * sampling_time;
			animation.keyframes.resize(frame_count + 1);

			float seconds = 0.0f;
			Animations::KeyFrame* keyframe = animation.keyframes.data();
			const size_t fbx_node_cnt = fbx_nodes.size();
			FbxTime fbx_current_time = fbx_start_time;
			for (FbxTime fbx_time = fbx_start_time; fbx_time <= fbx_end_time; fbx_time += fbx_sampling_step, ++keyframe)
			{
				// キーフレーム毎の姿勢データを取り出す
				keyframe->seconds = seconds;
				keyframe->keys.resize(fbx_node_cnt);
				for (size_t fbx_node_idx = 0; fbx_node_idx < fbx_node_cnt; ++fbx_node_idx)
				{
					//KeyData& key_data = keyframe->keys.at(fbx_node_idx);
					FbxNode* fbx_node = fbx_nodes.at(fbx_node_idx);
					if (!fbx_node)
					{
						// nodeがnullptrならemplace_backされない
						assert(0);
					}
					else
					{
						// 指定時間のローカル行列からスケール値、回転値、移動値を取り出す。
						const FbxAMatrix& fbxLocalTransform = fbx_node->EvaluateLocalTransform(fbx_time);

						keyframe->keys.at(fbx_node_idx).scale = FbxDouble4ToVECTOR3(fbxLocalTransform.GetS());		// scaleにunitは影響しない
						keyframe->keys.at(fbx_node_idx).rotate = FbxDouble4ToVECTOR4(fbxLocalTransform.GetQ());
						keyframe->keys.at(fbx_node_idx).translate = FbxDouble4ToVECTOR3(fbxLocalTransform.GetT());

						// スペースをアンダーバーに置き換える
						const std::string replaced_node_name = std::regex_replace(fbx_node->GetName(), std::regex(" "), "_");
						// アニメーション配列の要素とノードの名前を結びつける
						p_motions.at(filename_idx)->m_key_index.emplace(replaced_node_name, fbx_node_idx);
					}
				}
				seconds += sampling_time;
			}
		}

		// 後始末
		for (int i = 0; i < fbx_animation_cnt; ++i)
		{
			delete fbx_animation_stack_names[i];
		}

		// ファイル書き出し
		for (int i = 0; i < motion_cnt; ++i)
		{
			// ファイル名から不正な文字を取り除く
			std::string motion_path = filepathes.at(i);
			while (true)
			{
				const unsigned find = motion_path.find_last_of("|:;*?\",<> ");
				if (find == std::string::npos)
				{
					break;
				}
				else
				{
					if (find <= 0)
					{
						assert(0 && "invalid string.");
					}
					motion_path = motion_path.substr(0, find) + motion_path.substr(find + 1);
				}
			}

			// AssetDataBaseの情報のセット
			p_motions.at(i)->SetName(MyFunc::SplitFileName(motion_path));
			p_motions.at(i)->m_has_resource = true;
			p_motions.at(i)->Save(motion_path);
		}

		std::vector<std::shared_ptr<IResMotion>> ret_vec(motion_cnt);
		for (int i = 0; i < motion_cnt; ++i)
		{
			ret_vec.at(i) = p_motions.at(i);
		}
		return ret_vec;
	}
	void ResMotion::Save(const std::string& filepath)
	{
		// Binary
		{
			std::ofstream ofs(filepath.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed open file. ResMotion::Save()");
				return;
			}
			cereal::BinaryOutputArchive ar(ofs);
			ar(*this);
		}
#ifdef SAVE_JSON
		// Json
		{
			std::string path = filepath + "J";
			std::ofstream ofs(path.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed open file. ResMesh::Save()");
				return;
			}
			cereal::JSONOutputArchive ar(ofs);
			ar(*this);
		}
#endif//#ifdef SAVE_JSON
	}
#endif// USE_IMGUI

	void IResMotion::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}
	// ~IResMotion

	// ResMotion
	float ResMotion::GetSampleRate() const
	{
		return m_animation.sampling_rate;
	}

	float ResMotion::GetMotionLength() const
	{
		return m_animation.seconds_length;
	}

	std::vector<Animations::KeyData>* ResMotion::GetKeyFrame(const int frame)
	{
		return &m_animation.keyframes.at(frame).keys;
	}

	std::unordered_map<std::string, int>* ResMotion::GetKeyIndex()
	{
		return &m_key_index;
	}

	void ResMotion::SetAsyncOnLoad()
	{
		// Start loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load Motion
		bool b_result = false;
		{
			std::ifstream ifs(this->GetFilePath(), std::ios::in | std::ios::binary);

			if (!ifs.is_open())
			{
				b_result = false;
			}
			else
			{
				cereal::BinaryInputArchive ar(ifs);

				ar(*this);
				b_result = true;
			}
		}

		// Finish loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (b_result)
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
		}
	}

	void ResMotion::OnLoad()
	{
		// Ready loading
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load Motion
		bool b_result = false;
		{
			std::ifstream ifs(this->GetFilePath(), std::ios::in | std::ios::binary);

			if (!ifs.is_open())
			{
				b_result = false;
			}
			else
			{
				cereal::BinaryInputArchive ar(ifs);

				ar(*this);
				b_result = true;
			}
		}

		// Finish loading
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (b_result)
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
		}
	}

	// ~ResMotion
}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::ResMotion)
CEREAL_CLASS_VERSION(TKGEngine::ResMotion, 2)
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::IResMotion, TKGEngine::ResMotion)