
#include "IResAvatar.h"

#include "../../inc/Avatar_Defined.h"
#include "../../inc/ConstantBuffer.h"

#include "Systems/inc/AssetSystem.h"
#include "Systems/inc/AsyncFence.h"
#include "Systems/inc/StateManager.h"
#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/myfunc_file.h"

#include "Application/Objects/Components/inc/CTransform.h"

#include <string>
#include <vector>
#include <unordered_map>

#include <d3d11.h>
#include <wrl.h>
#include <cassert>
#include <memory>

namespace TKGEngine
{
	class ResAvatar
		: public IResAvatar
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResAvatar() = default;
		virtual ~ResAvatar() = default;
		ResAvatar(const ResAvatar&) = delete;
		ResAvatar& operator=(const ResAvatar&) = delete;

#ifdef USE_IMGUI
		static std::shared_ptr<IResAvatar> CreateFromFBX(
			const std::string& filepath,
			FbxScene* fbx_scene,
			std::vector<FbxMesh*>& fbx_meshes,
			std::unordered_map<std::string, int>& bone_name_index,
			bool is_save);
#endif// USE_IMGUI

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI
		// ~AssetDataBase
		
		int GetBoneCount() const override;
		int GetBoneIndex(const std::string& bone_name) override;
		int GetEnabledBoneCount() const override;
		int GetRootCount() const override;
		const char* GetRootName(int index) override;
		int GetRootIndex(int index) override;
		const Node* GetNode(int index) override;
		const Node* GetNode(const std::string& bone_name) override;

		const std::unordered_map<std::string, int>* GetNodeIndex() const override;
		const std::vector<std::string>* GetAlignedBoneNames() const override;

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<IResAvatar>(this),
					CEREAL_NVP(m_node_index),
					CEREAL_NVP(m_nodes),
					CEREAL_NVP(m_aligned_bone_names),
					CEREAL_NVP(m_root_names)
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

		// ==============================================
		// private variables
		// ==============================================
		// ボーン検索用要素番号リスト
		std::unordered_map<std::string, int> m_node_index;
		// ノードリスト
		std::vector<Node> m_nodes;
		// スキンが参照しているボーンをアニメーション行列の順に直したリスト
		std::vector<std::string> m_aligned_bone_names;
		// ルートノード名リスト
		std::vector<std::string> m_root_names;
	};


	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	ResourceManager<IResAvatar> IResAvatar::m_caches;
	std::mutex IResAvatar::m_cache_mutex;


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
	// IResAvatar
	std::shared_ptr<IResAvatar> IResAvatar::CreateInterface()
	{
		return std::make_shared<ResAvatar>();
	}

	std::shared_ptr<IResAvatar> IResAvatar::Load(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResAvatar> res_find = m_caches.Search(filename);
		if (res_find != nullptr)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResAvatar> res_new = IResAvatar::CreateInterface();
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
			return std::shared_ptr<IResAvatar>();
		}
		res_new->m_load_state_mutex.unlock();
		return res_new;
	}

	std::shared_ptr<IResAvatar> IResAvatar::LoadAsync(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResAvatar> res_find = m_caches.Search(filename);
		if (res_find != nullptr)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResAvatar> res_new(IResAvatar::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResAvatar::SetAsyncOnLoad, res_new));

		return res_new;
	}

	void IResAvatar::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

#ifdef USE_IMGUI
	void IResAvatar::CreateBinaryFromFBX(
		const std::string& filepath,
		FbxScene* fbx_scene,
		std::vector<FbxMesh*>& fbx_meshes,
		std::unordered_map<std::string, int>& bone_name_index,
		bool is_save)
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		auto&& s_ptr = ResAvatar::CreateFromFBX(filepath, fbx_scene, fbx_meshes, bone_name_index, is_save);
		if (s_ptr == nullptr)
			return;
		if (m_caches.Search(filepath) != nullptr)
		{
			m_caches.RemoveCache(filepath);
		}
		m_caches.Set(filepath, s_ptr);
	}

	std::shared_ptr<IResAvatar> ResAvatar::CreateFromFBX(
		const std::string& filepath,
		FbxScene* fbx_scene,
		std::vector<FbxMesh*>& fbx_meshes,
		std::unordered_map<std::string, int>& bone_name_index,
		bool is_save)
	{
		std::shared_ptr<ResAvatar> p_avatar = std::make_shared<ResAvatar>();

		// Node探索
		FbxNode* root_node = fbx_scene->GetRootNode();
		std::vector<FbxNode*> root_bones;
		int bone_cnt = 0;
		std::vector<std::string> names;
		// Bone検索
		{
			static std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
				if (!node)
					return;
				const int child_cnt = node->GetChildCount();
				std::string name_ = node->GetName();
				names.emplace_back(name_);
				// NodeAttributeがボーンのものを探索する
				FbxNodeAttribute* node_attribute = node->GetNodeAttribute();
				if (node_attribute)
				{
					auto att = node_attribute->GetAttributeType();
					switch (node_attribute->GetAttributeType())
					{
					case FbxNodeAttribute::eNull:
						for (int i = 0; i < child_cnt; ++i)
						{
							FbxNode* child = node->GetChild(i);
							if (child &&
								child->GetNodeAttribute() &&
								child->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton
								)
							{
								root_bones.emplace_back(node);
								p_avatar->m_root_names.emplace_back(node->GetName());

								p_avatar->m_node_index.emplace(node->GetName(), bone_cnt);

								++bone_cnt;
								break;
							}
						}
						break;

					case FbxNodeAttribute::eSkeleton:
						// Parent Nodeがないか、属性がnullptrならルートボーン
						FbxNode* parent = node->GetParent();
						if (!parent ||
							!parent->GetNodeAttribute() //||
							//(parent->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton &&
							//	parent->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eNull)
							)
						{
							root_bones.emplace_back(node);
							p_avatar->m_root_names.emplace_back(node->GetName());
						}

						// Boneの名前と要素番号を紐づける
						p_avatar->m_node_index.emplace(node->GetName(), bone_cnt);

						++bone_cnt;
						break;
					}
				}
				// Traverse Children
				//const int child_cnt = node->GetChildCount();
				for (int i = 0; i < child_cnt; ++i)
				{
					traverse(node->GetChild(i));
				}
			};
			traverse(root_node);
		}

		std::unordered_map<std::string, FbxAMatrix> node_geometric_matries;

		// Root Node検索
		{
			int node_idx = 0;
			p_avatar->m_nodes.resize(bone_cnt);

			for (auto&& root : root_bones)
			{
				static std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
					if (!node)
						return;

					const int child_cnt = node->GetChildCount();

					// Nodeの情報を取得する
					{
						const auto itr = p_avatar->m_node_index.find(node->GetName());
						if (itr == p_avatar->m_node_index.end())
						{
							return;
						}
						p_avatar->m_nodes.at(itr->second).bone_name = node->GetName();
						for (int i = 0; i < child_cnt; ++i)
						{
							FbxNode* child = node->GetChild(i);
							if (!child ||
								!child->GetNodeAttribute() ||
								child->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton
								)
							{
								continue;
							}
							std::string name_buf = node->GetChild(i)->GetName();
							p_avatar->m_nodes.at(itr->second).children_idx.emplace_back(p_avatar->m_node_index.at(node->GetChild(i)->GetName()));
						}
						FbxNode* parent = node->GetParent();
						if (parent &&
							parent->GetNodeAttribute() && (
								parent->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton ||
								parent->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eNull)
							)
						{
							// 親ノードがeNullで自分もeNullは存在しないとする
							if (node->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eNull)
							{
								p_avatar->m_nodes.at(itr->second).parent_idx = p_avatar->m_node_index.at(parent->GetName());
							}
						}
						FbxAMatrix& local_transform = node->EvaluateLocalTransform();
						p_avatar->m_nodes.at(itr->second).init_scale = FbxDouble4ToVECTOR3(local_transform.GetS());
						p_avatar->m_nodes.at(itr->second).init_rotate = FbxDouble4ToVECTOR4(local_transform.GetQ());
						p_avatar->m_nodes.at(itr->second).init_translate = FbxDouble4ToVECTOR3(local_transform.GetT());

						// Geometric matrix
						node_geometric_matries.emplace(
							node->GetName(),
							FbxAMatrix(
								node->GetGeometricTranslation(FbxNode::eSourcePivot),
								node->GetGeometricRotation(FbxNode::eSourcePivot),
								node->GetGeometricScaling(FbxNode::eSourcePivot))
						);
					}

					// Traverse Children
					for (int i = 0; i < child_cnt; ++i)
					{
						traverse(node->GetChild(i));
					}
				};
				traverse(root);
			}
		}

		int current_bone_idx = 0;

		// 各ボーンのInverseTransformとAnimationIndexを取得して、名前でセットしていく
		{
			for (auto&& fbx_mesh : fbx_meshes)
			{
				// Fetch data need to skinning.
				int deformer_cnt = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
				for (int deformer_idx = 0; deformer_idx < deformer_cnt; ++deformer_idx)
				{
					// Skinの取得
					FbxSkin* fbx_skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_idx, FbxDeformer::eSkin));
					int cluster_cnt = fbx_skin->GetClusterCount();
					// Skin内のボーン分データ探索
					for (int cluster_idx = 0; cluster_idx < cluster_cnt; ++cluster_idx)
					{
						FbxCluster* fbx_cluster = fbx_skin->GetCluster(cluster_idx);
						std::string bone_name = fbx_cluster->GetLink()->GetName();

						auto& bone_index = p_avatar->m_nodes.at(p_avatar->m_node_index.at(bone_name)).bone_idx;
						if (bone_index >= 0)
							continue;
						bone_index = current_bone_idx;
						bone_name_index.emplace(bone_name, current_bone_idx);
						++current_bone_idx;

						// Calculate Bone Transform Inverse Matrix
						{
							// メッシュ空間からワールド空間への変換行列
							FbxAMatrix fbx_mesh_space_transform;
							fbx_cluster->GetTransformMatrix(fbx_mesh_space_transform);

							// ボーン空間からワールド空間への変換行列
							FbxAMatrix fbx_bone_space_transform;
							fbx_cluster->GetTransformLinkMatrix(fbx_bone_space_transform);

							// ボーン逆行列を計算する
							FbxAMatrix fbxInverse_transform = fbx_bone_space_transform.Inverse() * fbx_mesh_space_transform * node_geometric_matries.at(fbx_cluster->GetLink()->GetName());

							MATRIX inverse_transform;
							FbxAMatrixToMATRIX(fbxInverse_transform, inverse_transform);
							
							auto& avatar_node = p_avatar->m_nodes.at(p_avatar->m_node_index.at(fbx_cluster->GetLink()->GetName()));
							avatar_node.inverse_transform = inverse_transform;
						}
					}
				}
			}
		}

		p_avatar->m_aligned_bone_names.resize(/*p_avatar->m_nodes.size()*/current_bone_idx);
		{
			for (auto&& node : p_avatar->m_nodes)
			{
				if (node.bone_idx >= 0)
				{
					p_avatar->m_aligned_bone_names.at(node.bone_idx) = node.bone_name;
				}
			}
		}

		//bone_names = p_avatar->m_aligned_bone_names;

		// File書き出し
		if (is_save)
		{
			// ファイル名から不正な文字を取り除く
			std::string avatar_path = filepath;
			while (true)
			{
				unsigned find = avatar_path.find_last_of(":;*?,<>\"| ");
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
					avatar_path = avatar_path.substr(0, find) + avatar_path.substr(find + 1);
				}
			}
			// AssetDataBaseの情報のセット
			p_avatar->SetName(MyFunc::SplitFileName(avatar_path));
			p_avatar->m_has_resource = true;
			p_avatar->Save(avatar_path);
		}

		return std::static_pointer_cast<IResAvatar>(p_avatar);
	}
	void ResAvatar::Save(const std::string& filepath)
	{
		// Binary
		{
			std::ofstream ofs(filepath.c_str(), std::ios::out | std::ios::binary);
			if (ofs.is_open() == false)
			{
				assert(0 && "failed open file. ResAvatar::Save()");
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
			if (ofs.is_open() == false)
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

	void IResAvatar::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}
	// ~IResAvatar

	// ResAvatar
#ifdef USE_IMGUI
	void ResAvatar::OnGUI()
	{
		// TODO : GUI対応




	}
#endif // USE_IMGUI

	int ResAvatar::GetBoneCount() const
	{
		return static_cast<int>(m_nodes.size());
	}

	int ResAvatar::GetBoneIndex(const std::string& bone_name)
	{
		auto itr = m_node_index.find(bone_name);
		if (itr == m_node_index.end())
		{
			assert(0);
			return -1;
		}
		return itr->second;
	}

	int ResAvatar::GetEnabledBoneCount() const
	{
		return static_cast<int>(m_aligned_bone_names.size());
	}

	int ResAvatar::GetRootCount() const
	{
		return static_cast<int>(m_root_names.size());
	}

	const char* ResAvatar::GetRootName(int index)
	{
		return m_root_names.at(index).c_str();
	}

	int ResAvatar::GetRootIndex(int index)
	{
		return m_node_index.at(m_root_names.at(index));
	}

	const Node* ResAvatar::GetNode(int index)
	{
		return &m_nodes.at(index);
	}

	const Node* ResAvatar::GetNode(const std::string& bone_name)
	{
		return &m_nodes.at(m_node_index.at(bone_name));
	}

	const std::unordered_map<std::string, int>* ResAvatar::GetNodeIndex() const
	{
		return &m_node_index;
	}

	const std::vector<std::string>* ResAvatar::GetAlignedBoneNames() const
	{
		return &m_aligned_bone_names;
	}

	void ResAvatar::SetAsyncOnLoad()
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

	void ResAvatar::OnLoad()
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



	// ~ResAvatar


}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::ResAvatar);
CEREAL_CLASS_VERSION(TKGEngine::ResAvatar, 1);
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::IResAvatar, TKGEngine::ResAvatar)