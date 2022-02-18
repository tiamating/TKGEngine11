
#include "IResMesh.h"

#include "../../inc/VertexBuffer.h"
#include "../../inc/IndexBuffer.h"
#include "Systems/inc/AssetSystem.h"
#include "Systems/inc/LogSystem.h"
#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/myfunc_math.h"
#include "Utility/inc/myfunc_file.h"
#include "Utility/inc/bounds.h"

#include <DirectXMath.h>
#include <cfloat>
#include <vector>

namespace TKGEngine
{
	class ResMesh
		: public IResMesh
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		// デフォルトコンストラクタはデシリアライズ時にのみ使用
		ResMesh() = default;
		ResMesh(BUFFER_HEAP_TYPE type);
		virtual ~ResMesh() = default;
		ResMesh(const ResMesh&) = delete;
		ResMesh& operator=(const ResMesh&) = delete;

#ifdef USE_IMGUI
		static std::shared_ptr<IResMesh> CreateFromFBX(
			const std::string& filepath,
			FbxScene* fbx_scene,
			FbxNode* fbx_node,
			FbxMesh* fbx_mesh,
			std::unordered_map<std::string, int>& bone_name_index,
			bool& is_skinned
		);
#endif// USE_IMGUI

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override {}
#endif // USE_IMGUI
		// ~AssetDataBase

		void ActivateVB(ID3D11DeviceContext* p_context, int slot, VERTEX_ELEMENT_TYPE type) override;
		void ActivateIB(ID3D11DeviceContext* p_context) override;

		VertexBuffer* GetVertexBuffer(VERTEX_ELEMENT_TYPE type) override;

		void Clear() override;
		void ReCalculateBounds() override;
		void ReCalculateNormals() override;
		void UploadMeshData() override;

		const std::vector<VECTOR3>* GetPositions() const override;
		const std::vector<VECTOR3>* GetNormals() const override;
		const std::vector<VECTOR2>* GetUVs() const override;
		const Bounds* GetBounds() const override;
		const std::vector<unsigned>* GetIndices() const override;

		void SetPositions(const std::vector<VECTOR3>& pos) override;
		void SetNormals(const std::vector<VECTOR3>& norm) override;
		void SetUVs(const std::vector<VECTOR2>& uv) override;
		void SetBounds(const Bounds& bounds) override;
		void SetIndices(const std::vector<unsigned>& indices) override;

		void SetIndexCount(int index, int count) override;

		int GetVertexCount() const override;
		int GetIndexCount() const override;
		int GetSubsetCount() const override;
		std::pair<int, int> GetSubset(int index) const override;

		VECTOR3 GetGlobalTranslate() const override;
		Quaternion GetGlobalRotate() const override;
		VECTOR3 GetGlobalScale() const override;

		void SetAsyncOnLoad() override;
		void OnLoad() override;

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
					cereal::base_class<IResMesh>(this),
					CEREAL_NVP(m_heap_type),
					// Subset
					CEREAL_NVP(m_subset_count),
					CEREAL_NVP(m_subsets),
					// ~Subset
					// Vertex
					CEREAL_NVP(m_vertex_type_using_flags),
					CEREAL_NVP(m_positions[0]),
					//m_positions[1],
					CEREAL_NVP(m_normals[0]),
					//m_normals[1],
					CEREAL_NVP(m_tangents),
					CEREAL_NVP(m_binormals),
					CEREAL_NVP(m_bones),
					CEREAL_NVP(m_weights),
					CEREAL_NVP(m_colors),
					CEREAL_NVP(m_uv0[0]),
					//m_uv0[1],
					CEREAL_NVP(m_uv1),
					CEREAL_NVP(m_uv2),
					CEREAL_NVP(m_uv3),
					CEREAL_NVP(m_uv4),
					CEREAL_NVP(m_uv5),
					CEREAL_NVP(m_uv6),
					CEREAL_NVP(m_uv7),
					// ~Vertex
					// Index
					CEREAL_NVP(m_indices[0]),
					//m_indices[1],
					// ~Index
					// Bounds
					CEREAL_NVP(m_bounds),
					CEREAL_NVP(m_max_point),
					CEREAL_NVP(m_min_point),
					// ~Bounds
					// Global Transform
					CEREAL_NVP(m_translate),
					CEREAL_NVP(m_rotate),
					CEREAL_NVP(m_scale)
					// ~Global Transform
				);
			}
		}

		struct Bone_Data
			: DirectX::XMUINT4
		{
		private:
			friend class cereal::access;
			template <class Archive>
			void serialize(Archive& archive)
			{
				archive(x, y, z, w);
			}
		};

		// ==============================================
		// private methods
		// ==============================================

#ifdef USE_IMGUI
		void Save(const std::string& filepath);
		void PrepareToFetch(FbxNode* fbx_node, FbxMesh* fbx_mesh);
		void FetchSubset(const FbxNode* fbx_node, FbxMesh* fbx_mesh);
		void FetchVertex(const FbxNode* fbx_node, FbxMesh* fbx_mesh, const std::unordered_map<std::string, int>& bone_name_index, bool& is_skinned);
#endif// USE_IMGUI

		void CreateBuffers();
		void ClearVerticesAndIndices();

		VECTOR3 CalculateNormal(const VECTOR3& p0, const VECTOR3& p1, const VECTOR3& p2);
		void CalculateTangentAndBinormalLH(
			const VECTOR3& p0, const VECTOR2& uv0,
			const VECTOR3& p1, const VECTOR2& uv1,
			const VECTOR3& p2, const VECTOR2& uv2,
			VECTOR3& tangent,
			VECTOR3& binormal);


		// ==============================================
		// private variables
		// ==============================================
		BUFFER_HEAP_TYPE m_heap_type = BUFFER_HEAP_TYPE::BUFFER_HEAP_IMMUTABLE;

		// Subset Data
		int m_subset_count = 0;
		std::vector<Subset> m_subsets;
		// ~Subset Data

		// Vertex Buffer Data
		// 要素数2の配列になっているものはメッシュの動的生成に対応
		int m_vertex_type_using_flags = 0;
		VertexBuffer m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD7) + 1];
		std::vector<VECTOR3> m_positions[2];
		std::vector<VECTOR3> m_normals[2];
		std::vector<VECTOR3> m_tangents;
		std::vector<VECTOR3> m_binormals;
		std::vector</*DirectX::XMUINT4*/Bone_Data> m_bones;
		std::vector<VECTOR4> m_weights;
		std::vector<VECTOR4> m_colors;
		std::vector<VECTOR2> m_uv0[2];
		std::vector<VECTOR2> m_uv1;
		std::vector<VECTOR2> m_uv2;
		std::vector<VECTOR2> m_uv3;
		std::vector<VECTOR2> m_uv4;
		std::vector<VECTOR2> m_uv5;
		std::vector<VECTOR2> m_uv6;
		std::vector<VECTOR2> m_uv7;

		bool m_is_changed_position = false;
		int m_current_position_idx = 0;
		bool m_is_changed_normal = false;
		int m_current_normal_idx = 0;
		bool m_is_changed_uv = false;
		int m_current_uv_idx = 0;
		// ~Vertex Buffer Data

		// Index Buffer Data
		IndexBuffer m_IB;
		std::vector<unsigned> m_indices[2];
		bool m_is_changed_index = false;
		int m_current_index_idx = 0;
		// ~Index Buffer Data

		// Bounds Data
		Bounds m_bounds;
		VECTOR3 m_max_point = VECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		VECTOR3 m_min_point = VECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
		// ~Bounds Data

		// Global Transform
		VECTOR3 m_translate = VECTOR3::Zero;
		Quaternion m_rotate = Quaternion::Identity;
		VECTOR3 m_scale = VECTOR3::One;
		// ~Global Transform
	};


	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	ResourceManager<IResMesh> IResMesh::m_caches;
	std::mutex IResMesh::m_cache_mutex;

	static constexpr const char* DEFAULT_NAME = "default";


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
	inline VECTOR2 FbxDouble2ToVECTOR2(const FbxDouble2& fbx_vector)
	{
		return VECTOR2(
			static_cast<float>(fbx_vector[0]),
			static_cast<float>(fbx_vector[1])
		);
	}
	inline VECTOR3 FbxDouble3ToVECTOR3(const FbxDouble3& fbx_vector)
	{
		return VECTOR3(
			static_cast<float>(fbx_vector[0]),
			static_cast<float>(fbx_vector[1]),
			static_cast<float>(fbx_vector[2])
		);
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
	inline VECTOR4 FbxColorToVECTOR4(const FbxColor& fbx_color)
	{
		return VECTOR4(
			static_cast<float>(fbx_color[0]),
			static_cast<float>(fbx_color[1]),
			static_cast<float>(fbx_color[2]),
			static_cast<float>(fbx_color[3])
		);
	}
#endif// USE_IMGUI


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	// IResMesh
	IResMesh* IResMesh::CreateInterface(BUFFER_HEAP_TYPE type)
	{
		return new ResMesh(type);
	}

	std::shared_ptr<IResMesh> IResMesh::Load(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResMesh> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResMesh> res_new(IResMesh::CreateInterface(BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT));
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
			return std::shared_ptr<IResMesh>();
		}
		res_new->m_load_state_mutex.unlock();
		return res_new;
	}

	std::shared_ptr<IResMesh> IResMesh::LoadAsync(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResMesh> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResMesh> res_new(IResMesh::CreateInterface(BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT));
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResMesh::SetAsyncOnLoad, res_new));

		return res_new;
	}

#ifdef USE_IMGUI
	void IResMesh::CreateBinaryFromFBX(
		const std::string& filepath,
		FbxScene* fbx_scene,
		FbxNode* fbx_node,
		FbxMesh* fbx_mesh,
		std::unordered_map<std::string, int>& bone_name_index,
		bool& is_skinned
	)
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		auto&& s_ptr = ResMesh::CreateFromFBX(filepath, fbx_scene, fbx_node, fbx_mesh, bone_name_index, is_skinned);
		if (!s_ptr)
			return;
		// すでに同名ファイルが存在するなら、新しいリソースとして登録する
		if (m_caches.Search(filepath))
		{
			m_caches.RemoveCache(filepath);
		}
		m_caches.Set(filepath, s_ptr);
	}
#endif// USE_IMGUI

	std::shared_ptr<IResMesh> IResMesh::Create()
	{
		static int idx = 0;

		std::shared_ptr<IResMesh> s_ptr(IResMesh::CreateInterface(BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT));
		s_ptr->m_has_resource = true;
		s_ptr->SetName(std::string(DEFAULT_NAME + std::to_string(idx++)));
		return s_ptr;
	}

	void IResMesh::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

	void IResMesh::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}
	// ~IResMesh


	// ResMesh
	ResMesh::ResMesh(BUFFER_HEAP_TYPE type)
		: m_heap_type(type)
	{
		m_subsets.emplace_back(Subset());
		m_subset_count = 1;
	}

#ifdef USE_IMGUI
	std::shared_ptr<IResMesh> ResMesh::CreateFromFBX(
		const std::string& filepath,
		FbxScene* fbx_scene,
		FbxNode* fbx_node,
		FbxMesh* fbx_mesh,
		std::unordered_map<std::string, int>& bone_name_index,
		bool& is_skinned
	)
	{
		// ファイル名から不正な文字を取り除く
		std::string mesh_path = filepath;
		while (true)
		{
			const unsigned find = mesh_path.find_last_of(":;*?,<>\"| ");
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
				mesh_path = mesh_path.substr(0, find) + mesh_path.substr(find + 1);
			}
		}

		const std::shared_ptr<ResMesh> p_mesh = std::make_shared<ResMesh>(BUFFER_HEAP_TYPE::BUFFER_HEAP_IMMUTABLE);
		// AssetDataBaseの情報のセット
		p_mesh->SetName(MyFunc::SplitFileName(mesh_path));
		p_mesh->SetFilePath(mesh_path);
		p_mesh->m_has_resource = true;
		// 頂点、サブセット情報の取得
		p_mesh->PrepareToFetch(fbx_node, fbx_mesh);
		p_mesh->FetchSubset(fbx_node, fbx_mesh);
		p_mesh->FetchVertex(fbx_node, fbx_mesh, bone_name_index, is_skinned);
		// Saveする
		p_mesh->Save(mesh_path);
		// バッファの作成と使用済み配列の削除
		p_mesh->CreateBuffers();
		p_mesh->ClearVerticesAndIndices();

		return std::static_pointer_cast<IResMesh>(p_mesh);
	}
	void ResMesh::PrepareToFetch(FbxNode* fbx_node, FbxMesh* fbx_mesh)
	{
		// Subset
		const int material_cnt = fbx_node->GetMaterialCount();
		{
			m_subset_count = material_cnt > 0 ? material_cnt : 1;
			m_subsets.resize(m_subset_count);
		}
		// Vertex
		const int vertex_cnt = fbx_mesh->GetPolygonVertexCount();
		{
			// Position
			m_positions[0].resize(vertex_cnt);
			// Normal
			m_normals[0].resize(vertex_cnt);
			// Tangent
			m_tangents.resize(vertex_cnt);
			// Binormal
			m_binormals.resize(vertex_cnt);

			if (fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) > 0)
			{
				// BoneIdx
				m_bones.resize(vertex_cnt);
				// BoneWeight
				m_weights.resize(vertex_cnt);
			}

			if (fbx_mesh->GetElementUVCount() > 0)
			{
				// UV
				m_uv0[0].resize(vertex_cnt);
				m_uv1.resize(vertex_cnt);
				m_uv2.resize(vertex_cnt);
				m_uv3.resize(vertex_cnt);
				m_uv4.resize(vertex_cnt);
				m_uv5.resize(vertex_cnt);
				m_uv6.resize(vertex_cnt);
				m_uv7.resize(vertex_cnt);
			}

			if (fbx_mesh->GetElementVertexColorCount() > 0)
			{
				// Color
				m_colors.resize(vertex_cnt);
			}
		}
		// Index
		const int polygon_vertex_cnt = fbx_mesh->GetPolygonVertexCount();
		{
			m_indices->resize(polygon_vertex_cnt);
		}

		// Global Transform
		{
			const FbxAMatrix& local_transform = fbx_node->EvaluateLocalTransform();
			m_scale = FbxDouble4ToVECTOR3(local_transform.GetS());
			m_rotate = FbxDouble4ToVECTOR4(local_transform.GetQ());
			m_translate = FbxDouble4ToVECTOR3(local_transform.GetT());
		}
	}
	void ResMesh::FetchSubset(const FbxNode* fbx_node, FbxMesh* fbx_mesh)
	{
		const int material_cnt = fbx_node->GetMaterialCount();
		const int polygon_cnt = fbx_mesh->GetPolygonCount();

		if (material_cnt > 0)
		{
			for (int polygon_idx = 0; polygon_idx < polygon_cnt; ++polygon_idx)
			{
				const int material_idx = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_idx);
				m_subsets.at(material_idx).index_count += 3;
			}

			int offset = 0;
			for (auto&& subset : m_subsets)
			{
				subset.start_index = offset;
				offset += subset.index_count;

				subset.index_count = 0;
			}
		}
		//else
		//{
		//	m_subsets.at(0).start_index = 0;
		//	m_subsets.at(0).index_count = polygon_cnt * 3;
		//}
	}
	void ResMesh::FetchVertex(const FbxNode* fbx_node, FbxMesh* fbx_mesh, const std::unordered_map<std::string, int>& bone_name_index, bool& is_skinned)
	{
		struct BoneInfluence
		{
			int use_count = 0;
			int indices[MAX_BONE_INFLUENCE] = { 0 };
			float weights[MAX_BONE_INFLUENCE] = { 1.0f };

			void Add(int index, float weight)
			{
				if (use_count < MAX_BONE_INFLUENCE)
				{
					indices[use_count] = index;
					weights[use_count] = weight;
					++use_count;
				}
			}
		};
		// Fetch bone influence.
		const int control_point_cnt = fbx_mesh->GetControlPointsCount();
		const int polygon_cnt = fbx_mesh->GetPolygonCount();
		const int material_cnt = fbx_node->GetMaterialCount();

		// Bone
		bool b_has_bone = false;
		std::vector<BoneInfluence> bone_influences;
		{
			//FbxAMatrix geometric_transform(
			//	fbx_node->GetGeometricTranslation(FbxNode::eSourcePivot),
			//	fbx_node->GetGeometricRotation(FbxNode::eSourcePivot),
			//	fbx_node->GetGeometricScaling(FbxNode::eSourcePivot)
			//);

			// Fetch data need to skinning.
			const int deformer_cnt = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
			if (deformer_cnt > 0)
			{
				b_has_bone = true;
				is_skinned = true;
				bone_influences.resize(control_point_cnt);
			}
			for (int deformer_idx = 0; deformer_idx < deformer_cnt; ++deformer_idx)
			{
				FbxSkin* fbx_skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_idx, FbxDeformer::eSkin));
				const int cluster_cnt = fbx_skin->GetClusterCount();
				for (int cluster_idx = 0; cluster_idx < cluster_cnt; ++cluster_idx)
				{
					const FbxCluster* fbx_cluster = fbx_skin->GetCluster(cluster_idx);
					const int bone_idx_from_avatar = bone_name_index.at(fbx_skin->GetCluster(cluster_idx)->GetLink()->GetName());
					// Fetch Bone Influence
					{
						const int cluster_control_point_indices_count = fbx_cluster->GetControlPointIndicesCount();
						const int* control_point_indices = fbx_cluster->GetControlPointIndices();
						const double* control_point_weights = fbx_cluster->GetControlPointWeights();

						for (int i = 0; i < cluster_control_point_indices_count; ++i)
						{
							BoneInfluence& influence = bone_influences.at(control_point_indices[i]);
							influence.Add(/*cluster_idx*/bone_idx_from_avatar, static_cast<float>(control_point_weights[i]));
						}
					}
				}
			}
		}

		// Has Normal
		bool has_normal = false;
		if (fbx_mesh->GetElementNormalCount() > 0)
		{
			has_normal = true;
		}

		// UVセット名
		FbxStringList UV_set_names;
		fbx_mesh->GetUVSetNames(UV_set_names);

		// Has TexCoord
		int num_texcoord = fbx_mesh->GetElementUVCount();
		if (num_texcoord > 8)
		{
			num_texcoord = 8;
		}
		//if ( > 0)
		//{
		//	has_texcoord = true;
		//}

		// Has Color
		bool has_color = false;
		enum class ColorType
		{
			None,
			PolygonVertex_IndexToDirect,
			ControlPoint_Direct
		};
		ColorType color_type = ColorType::None;
		const int color_layer_count = fbx_mesh->GetElementVertexColorCount();
		FbxGeometryElementVertexColor* color = nullptr;
		const FbxLayerElementArrayTemplate<int>* color_index = nullptr;
		const FbxLayerElementArrayTemplate<FbxColor>* color_array = nullptr;
		//int color_index_count = 0;
		if (color_layer_count > 0)
		{
			has_color = true;

			// TODO : 頂点カラーが複数ある場合には対応していない
			color = fbx_mesh->GetElementVertexColor(0);

			const auto mapping_mode = color->GetMappingMode();
			const auto reference_mode = color->GetReferenceMode();

			if (mapping_mode == FbxGeometryElement::eByPolygonVertex && reference_mode == FbxGeometryElement::EReferenceMode::eIndexToDirect)
			{
				color_type = ColorType::PolygonVertex_IndexToDirect;
			}
			else if (mapping_mode == FbxGeometryElement::eByControlPoint && reference_mode == FbxGeometryElement::EReferenceMode::eDirect)
			{
				color_type = ColorType::ControlPoint_Direct;
			}
			else
			{
				// 対応するmodeでなければ取り出さない
				has_color = false;
			}

			if (color_type == ColorType::ControlPoint_Direct)
			{
				color_array = &color->GetDirectArray();
			}
			else if (color_type == ColorType::PolygonVertex_IndexToDirect)
			{
				color_index = &color->GetIndexArray();
			}
			//color_index_count = color_index->GetCount();
		}

		// 頂点情報の取り出し
		const FbxVector4* control_points = fbx_mesh->GetControlPoints();
		const int control_point_count = fbx_mesh->GetControlPointsCount();
		int vertex_cnt = 0;

		for (int polygon_idx = 0; polygon_idx < polygon_cnt; ++polygon_idx)
		{
			int material_idx = 0;
			if (material_cnt > 0)
			{
				material_idx = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_idx);
			}

			Subset& subset = m_subsets.at(material_idx);
			const int index_offset = subset.start_index + subset.index_count;

			bool has_uv = false;
			VECTOR3 pos_buf[3];
			VECTOR2 uv_buf[3];
			const int current_polygon_vertex = vertex_cnt;
			// ポリゴンごとに取り出し
			for (int vertex_idx = 0; vertex_idx < 3; ++vertex_idx)
			{
				const int control_point_idx = fbx_mesh->GetPolygonVertex(polygon_idx, vertex_idx);
				// Position
				{
					m_vertex_type_using_flags |= (1 << static_cast<int>(VERTEX_ELEMENT_TYPE::POSITION));

					m_positions[0].at(vertex_cnt)
						= pos_buf[vertex_idx]
						= FbxDouble4ToVECTOR3(control_points[control_point_idx]);
				}

				if (b_has_bone)
				{
					// Bone Influence
					{
						m_vertex_type_using_flags |= (1 << static_cast<int>(VERTEX_ELEMENT_TYPE::BONES));
						m_vertex_type_using_flags |= (1 << static_cast<int>(VERTEX_ELEMENT_TYPE::WEIGHTS));

						const BoneInfluence& bone_influence = bone_influences.at(control_point_idx);

						m_bones.at(vertex_cnt).x = bone_influence.indices[0];
						m_bones.at(vertex_cnt).y = bone_influence.indices[1];
						m_bones.at(vertex_cnt).z = bone_influence.indices[2];
						m_bones.at(vertex_cnt).w = bone_influence.indices[3];

						m_weights.at(vertex_cnt).x = bone_influence.weights[0];
						m_weights.at(vertex_cnt).y = bone_influence.weights[1];
						m_weights.at(vertex_cnt).z = bone_influence.weights[2];
						m_weights.at(vertex_cnt).w = 1.0f - (m_weights.at(vertex_cnt).x + m_weights.at(vertex_cnt).y + m_weights.at(vertex_cnt).z);
						//m_weights.at(i).w = bone_influences.weights[3];
					}
				}

				// Normal
				if (has_normal)
				{
					m_vertex_type_using_flags |= (1 << static_cast<int>(VERTEX_ELEMENT_TYPE::NORMAL));

					FbxVector4 normal;
					fbx_mesh->GetPolygonVertexNormal(polygon_idx, vertex_idx, normal);
					m_normals[0].at(vertex_cnt) = FbxDouble4ToVECTOR3(normal).Normalized();
				}
				else
				{
					m_normals[0].at(vertex_cnt) = VECTOR3::Zero;
				}

				// Texcoord
				if (num_texcoord > 0)
				{
					for (int i = 0; i < num_texcoord; ++i)
					{
						m_vertex_type_using_flags |= (1 << (static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD0) + i));

						bool unmapped_uv = false;
						FbxVector2 uv;
						fbx_mesh->GetPolygonVertexUV(polygon_idx, vertex_idx, UV_set_names[i], uv, unmapped_uv);
						uv[1] = 1.0f - uv[1];

						switch (i)
						{
							case 0:
								m_uv0[0].at(vertex_cnt)
									= uv_buf[vertex_idx]
									= FbxDouble2ToVECTOR2(uv);
								break;
							case 1:
								m_uv1.at(vertex_cnt) = FbxDouble2ToVECTOR2(uv);
								break;
							case 2:
								m_uv2.at(vertex_cnt) = FbxDouble2ToVECTOR2(uv);
								break;
							case 3:
								m_uv3.at(vertex_cnt) = FbxDouble2ToVECTOR2(uv);
								break;
							case 4:
								m_uv4.at(vertex_cnt) = FbxDouble2ToVECTOR2(uv);
								break;
							case 5:
								m_uv5.at(vertex_cnt) = FbxDouble2ToVECTOR2(uv);
								break;
							case 6:
								m_uv6.at(vertex_cnt) = FbxDouble2ToVECTOR2(uv);
								break;
							case 7:
								m_uv7.at(vertex_cnt) = FbxDouble2ToVECTOR2(uv);
								break;
						}
					}

					//bool unmapped_uv = false;
					//FbxVector2 uv;
					//fbx_mesh->GetPolygonVertexUV(polygon_idx, LH_vertex_idx, UV_set_names[0], uv, unmapped_uv);
					//uv[1] = 1.0f - uv[1];
					//m_uv0[0].at(vertex_cnt)
					//	= uv_buf[vertex_idx]
					//	= FbxDouble2ToVECTOR2(uv);
					has_uv = true;
				}

				// Color
				if (has_color)
				{
					m_vertex_type_using_flags |= (1 << static_cast<int>(VERTEX_ELEMENT_TYPE::COLOR));

					if (color_type == ColorType::ControlPoint_Direct)
					{
						m_colors.at(vertex_cnt) = FbxColorToVECTOR4(color_array->GetAt(control_point_idx));
					}
					else if (color_type == ColorType::PolygonVertex_IndexToDirect)
					{
						m_colors.at(vertex_cnt) = FbxColorToVECTOR4(color->GetDirectArray().GetAt(color_index->GetAt(vertex_cnt)));
					}
				}

				// TODO : Indexを正しくロードするようにして、同一頂点を複数持たないようにする.
				m_indices[0].at(index_offset + vertex_idx) = vertex_cnt;
				++vertex_cnt;
			}
			subset.index_count += 3;

			// UVを持っている場合TangentとBinormalを計算する
			if (has_uv)
			{
				m_vertex_type_using_flags |= (1 << static_cast<int>(VERTEX_ELEMENT_TYPE::TANGENT));
				//m_vertex_type_using_flags |= (1 << static_cast<int>(VERTEX_ELEMENT_TYPE::BINORMAL));

				VECTOR3 tangent, binormal;
				CalculateTangentAndBinormalLH(
					pos_buf[0], uv_buf[0],
					pos_buf[1], uv_buf[1],
					pos_buf[2], uv_buf[2],
					tangent, binormal);
				for (int i = 0; i < 3; ++i)
				{
					m_tangents.at(current_polygon_vertex + i) = tangent;
					m_binormals.at(current_polygon_vertex + i) = binormal;
				}
			}
		}

		// AABBの計算
		ReCalculateBounds();
	}

	void ResMesh::Save(const std::string& filepath)
	{
		// Binary
		{
			std::ofstream ofs(filepath.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed open file. ResMesh::Save()");
				return;
			}
			cereal::BinaryOutputArchive ar(ofs);
			ar(*this);
		}

#ifdef SAVE_JSON
		// Json
		{
			const std::string path = filepath + "J";
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

	void ResMesh::ActivateVB(ID3D11DeviceContext* p_context, int slot, VERTEX_ELEMENT_TYPE type)
	{
		if (m_vertex_type_using_flags & (1 << static_cast<int>(type)))
		{
			m_VBs[static_cast<int>(type)].Set(p_context, slot);
		}
	}
	void ResMesh::ActivateIB(ID3D11DeviceContext* p_context)
	{
		m_IB.Set(p_context);
	}

	VertexBuffer* ResMesh::GetVertexBuffer(VERTEX_ELEMENT_TYPE type)
	{
		return &m_VBs[static_cast<int>(type)];
	}

	void ResMesh::Clear()
	{
		if (m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT)
			return;

		m_vertex_type_using_flags = 0;
		for (auto&& vb : m_VBs)
		{
			vb.Release();
		}
		for (auto&& v : m_positions)
		{
			v.clear();
			v.shrink_to_fit();
		}
		for (auto&& v : m_normals)
		{
			v.clear();
			v.shrink_to_fit();
		}
		for (auto&& v : m_uv0)
		{
			v.clear();
			v.shrink_to_fit();
		}
		m_uv1.clear();
		m_uv1.shrink_to_fit();
		m_uv2.clear();
		m_uv2.shrink_to_fit();
		m_uv3.clear();
		m_uv3.shrink_to_fit();
		m_uv4.clear();
		m_uv4.shrink_to_fit();
		m_uv5.clear();
		m_uv5.shrink_to_fit();
		m_uv6.clear();
		m_uv6.shrink_to_fit();
		m_uv7.clear();
		m_uv7.shrink_to_fit();

		m_current_position_idx = 0;
		m_current_normal_idx = 0;
		m_current_uv_idx = 0;

		m_IB.Release();
		for (auto&& v : m_indices)
		{
			v.clear();
			v.shrink_to_fit();
		}

		m_bounds.Reset();
		m_max_point = VECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		m_min_point = VECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	void ResMesh::ReCalculateBounds()
	{
		VECTOR3 max = VECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		VECTOR3 min = VECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);

		for (auto&& p : m_positions[m_current_position_idx])
		{
			// Max
			{
				// x
				max.x = MyMath::Max(max.x, p.x);
				// y
				max.y = MyMath::Max(max.y, p.y);
				// z
				max.z = MyMath::Max(max.z, p.z);
			}
			// Min
			{
				// x
				min.x = MyMath::Min(min.x, p.x);
				// y
				min.y = MyMath::Min(min.y, p.y);
				// z
				min.z = MyMath::Min(min.z, p.z);
			}
		}
		m_max_point = max;
		m_min_point = min;
		m_bounds.CreateFromPoints(max, min);
	}

	void ResMesh::ReCalculateNormals()
	{
		const auto& v_idx = m_indices[m_current_index_idx];
		const auto& v_pos = m_positions[m_current_position_idx];
		auto& v_norm = m_normals[m_current_normal_idx];

		const int idx_cnt = v_idx.size();
		const int pos_cnt = v_pos.size();
		const int triangle_cnt = idx_cnt / 3;

		for (auto&& n : m_normals)
		{
			n.resize(pos_cnt);
			n.shrink_to_fit();
		}

		assert(idx_cnt != 0);
		assert(triangle_cnt * 3 == idx_cnt);
		// ポリゴンごとに法線の計算
		for (int i = 0; i < triangle_cnt; ++i)
		{
			const VECTOR3 p0 = v_pos.at(v_idx.at(i * 3 + 0));
			const VECTOR3 p1 = v_pos.at(v_idx.at(i * 3 + 1));
			const VECTOR3 p2 = v_pos.at(v_idx.at(i * 3 + 2));

			const VECTOR3 v01 = p1 - p0;
			const VECTOR3 v02 = p2 - p0;

			VECTOR3 cross = VECTOR3::Cross(v01, v02);
			const VECTOR3 norm_cross = cross.Normalized();
			v_norm.at(v_idx.at(i * 3 + 0)) = v_norm.at(v_idx.at(i * 3 + 1)) = v_norm.at(v_idx.at(i * 3 + 2)) = norm_cross;
		}
	}

	void ResMesh::UploadMeshData()
	{
		// Position
		if (m_is_changed_position)
		{
			const size_t size = m_positions[m_current_position_idx].size() * sizeof(VECTOR3);
			memcpy(m_positions[1 - m_current_position_idx].data(), m_positions[m_current_position_idx].data(), size);

			m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::POSITION)].UpdateSubresource(m_positions[m_current_position_idx].data());
			m_is_changed_position = false;
			m_current_position_idx = 1 - m_current_position_idx;
		}
		// Normal
		if (m_is_changed_normal)
		{
			const size_t size = m_normals[m_current_normal_idx].size() * sizeof(VECTOR3);
			memcpy(m_normals[1 - m_current_normal_idx].data(), m_normals[m_current_normal_idx].data(), size);

			m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::NORMAL)].UpdateSubresource(m_normals[m_current_normal_idx].data());
			m_is_changed_normal = false;
			m_current_normal_idx = 1 - m_current_normal_idx;
		}
		// UV
		if (m_is_changed_uv)
		{
			const size_t size = m_uv0[m_current_uv_idx].size() * sizeof(VECTOR2);
			memcpy(m_uv0[1 - m_current_uv_idx].data(), m_uv0[m_current_uv_idx].data(), size);

			m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD0)].UpdateSubresource(m_uv0[m_current_uv_idx].data());
			m_is_changed_uv = false;
			m_current_uv_idx = 1 - m_current_uv_idx;
		}
		// Index
		if (m_is_changed_index)
		{
			const size_t size = m_indices[m_current_index_idx].size() * sizeof(unsigned);
			memcpy(m_indices[1 - m_current_index_idx].data(), m_indices[m_current_index_idx].data(), size);

			m_IB.UpdateSubreasource(m_indices->data());
			m_is_changed_index = false;
			m_current_index_idx = 1 - m_current_index_idx;
		}
	}

	const std::vector<VECTOR3>* ResMesh::GetPositions() const
	{
		return &(m_positions[m_current_position_idx]);
	}

	const std::vector<VECTOR3>* ResMesh::GetNormals() const
	{
		return &(m_normals[m_current_normal_idx]);
	}

	const std::vector<VECTOR2>* ResMesh::GetUVs() const
	{
		return &(m_uv0[m_current_uv_idx]);
	}

	const Bounds* ResMesh::GetBounds() const
	{
		return &m_bounds;
	}

	const std::vector<unsigned>* ResMesh::GetIndices() const
	{
		return &(m_indices[m_current_index_idx]);
	}

	void ResMesh::SetPositions(const std::vector<VECTOR3>& pos)
	{
		const size_t size = pos.size();
		// [0], [1]のバッファのリサイズ
		m_positions[m_current_position_idx].resize(size);
		m_positions[m_current_position_idx].shrink_to_fit();
		m_positions[1 - m_current_position_idx].resize(size);
		m_positions[1 - m_current_position_idx].shrink_to_fit();
		m_is_changed_position = true;
		for (size_t i = 0; i < size; ++i)
		{
			m_positions[m_current_position_idx].at(i) = pos.at(i);
		}

		m_vertex_type_using_flags |= (1 << static_cast<int>(VERTEX_ELEMENT_TYPE::POSITION));
		m_positions[m_current_position_idx] = pos;
		m_is_changed_position = true;

		if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::POSITION)].IsCreated() == false)
		{
			const int elem_cnt = pos.size();
			m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::POSITION)].Create(
				m_positions[m_current_position_idx].data(),
				sizeof(VECTOR3),
				elem_cnt,
				VERTEX_ELEMENT_TYPE::POSITION,
				m_heap_type);
		}
	}

	void ResMesh::SetNormals(const std::vector<VECTOR3>& norm)
	{
		const size_t size = norm.size();
		m_normals[m_current_normal_idx].resize(size);
		m_normals[m_current_normal_idx].shrink_to_fit();
		m_normals[1 - m_current_normal_idx].resize(size);
		m_normals[1 - m_current_normal_idx].shrink_to_fit();
		m_is_changed_normal = true;
		for (size_t i = 0; i < size; ++i)
		{
			m_normals[m_current_normal_idx].at(i) = norm.at(i);
		}

		m_vertex_type_using_flags |= (1 << static_cast<int>(VERTEX_ELEMENT_TYPE::NORMAL));
		m_normals[m_current_normal_idx] = norm;
		m_is_changed_normal = true;

		if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::NORMAL)].IsCreated() == false)
		{
			const int elem_cnt = norm.size();
			m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::NORMAL)].Create(
				m_normals[m_current_normal_idx].data(),
				sizeof(VECTOR3),
				elem_cnt,
				VERTEX_ELEMENT_TYPE::NORMAL,
				m_heap_type);
		}
	}

	void ResMesh::SetUVs(const std::vector<VECTOR2>& uv)
	{
		const size_t size = uv.size();
		m_uv0[m_current_index_idx].resize(size);
		m_uv0[m_current_index_idx].shrink_to_fit();
		m_uv0[1 - m_current_index_idx].resize(size);
		m_uv0[1 - m_current_index_idx].shrink_to_fit();
		m_is_changed_uv = true;
		for (size_t i = 0; i < size; ++i)
		{
			m_uv0[m_current_uv_idx].at(i) = uv.at(i);
		}

		m_vertex_type_using_flags |= (1 << static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD0));
		m_uv0[m_current_uv_idx] = uv;
		m_is_changed_uv = true;

		if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD0)].IsCreated() == false)
		{
			const int elem_cnt = uv.size();
			m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD0)].Create(
				m_uv0[m_current_uv_idx].data(),
				sizeof(VECTOR2),
				elem_cnt,
				VERTEX_ELEMENT_TYPE::TEXCOORD0,
				m_heap_type);
		}
	}

	void ResMesh::SetBounds(const Bounds& bounds)
	{
		m_bounds = bounds;
	}

	void ResMesh::SetIndices(const std::vector<unsigned>& indices)
	{
		const size_t size = indices.size();
		m_indices[m_current_index_idx].resize(size);
		m_indices[1 - m_current_index_idx].resize(size);
		m_is_changed_index = true;
		for (size_t i = 0; i < size; ++i)
		{
			m_indices[m_current_index_idx].at(i) = indices.at(i);
		}

		if (m_IB.IsCreated() == false)
		{
			m_IB.Create(
				m_indices[m_current_index_idx].data(),
				m_indices[m_current_index_idx].size(),
				m_heap_type
			);
		}
	}

	void ResMesh::SetIndexCount(int index, int count)
	{
		// 範囲外チェック
		if (index >= m_subset_count || index < 0)
		{
			return;
		}

		m_subsets.at(index).index_count = count;
	}

	int ResMesh::GetVertexCount() const
	{
		return static_cast<int>(m_positions[m_current_position_idx].size());
	}

	int ResMesh::GetIndexCount() const
	{
		return static_cast<int>(m_indices[m_current_index_idx].size());
	}

	int ResMesh::GetSubsetCount() const
	{
		assert(m_subset_count > 0);

		return m_subset_count;
	}

	std::pair<int, int> ResMesh::GetSubset(int index) const
	{
		// 範囲外チェック
		if (index >= m_subset_count || index < 0)
			return std::pair<int, int>(0, 0);

		const auto& subset = m_subsets.at(index);
		return std::pair<int, int>(subset.start_index, subset.index_count);
	}

	VECTOR3 ResMesh::GetGlobalTranslate() const
	{
		return m_translate;
	}

	Quaternion ResMesh::GetGlobalRotate() const
	{
		return m_rotate;
	}

	VECTOR3 ResMesh::GetGlobalScale() const
	{
		return m_scale;
	}

	void ResMesh::SetAsyncOnLoad()
	{
		// Start loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load Mesh
		bool b_result = false;
		{
			std::ifstream ifs(this->GetFilePath(), std::ios::in | std::ios::binary);

			if (ifs.is_open() == false)
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
			if (b_result == false)
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;

				// 頂点バッファの作成
				CreateBuffers();
				// ロードした情報はバッファ作成のみに使用するため、作成後はクリアする
				ClearVerticesAndIndices();
			}
		}
	}

	void ResMesh::OnLoad()
	{
		// Ready loading
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load Mesh
		bool b_result = false;
		{
			std::ifstream ifs(this->GetFilePath(), std::ios::in | std::ios::binary);

			if (ifs.is_open() == false)
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
			if (b_result == false)
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;

				// 頂点バッファの作成
				CreateBuffers();
				// ロードした情報はバッファ作成のみに使用するため、作成後はクリアする
				ClearVerticesAndIndices();
			}
		}
	}

	void ResMesh::CreateBuffers()
	{
		// VB
		for (int i = 0; i <= static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD7); ++i)
		{
			if ((m_vertex_type_using_flags & (1 << i)) == 0)
			{
				continue;
			}

			const VERTEX_ELEMENT_TYPE type = static_cast<VERTEX_ELEMENT_TYPE>(i);
			switch (type)
			{
				case VERTEX_ELEMENT_TYPE::POSITION:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::POSITION);
					if (m_VBs[element_type].Create(
						m_positions[0].data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_positions[0].size(),
						VERTEX_ELEMENT_TYPE::POSITION,
						m_heap_type,
						true,
						false) == false)
					{
						assert(0 && "failed create VB::POSITION ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::NORMAL:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::NORMAL);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::NORMAL)].Create(
						m_normals[0].data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_normals[0].size(),
						VERTEX_ELEMENT_TYPE::NORMAL,
						m_heap_type,
						true,
						false) == false)
					{
						assert(0 && "failed create VB::NORMAL ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::TANGENT:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::TANGENT);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TANGENT)].Create(
						m_tangents.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_tangents.size(),
						VERTEX_ELEMENT_TYPE::TANGENT,
						m_heap_type,
						true,
						false) == false)
					{
						assert(0 && "failed create VB::TANGENT ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::BONES:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::BONES);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::BONES)].Create(
						m_bones.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_bones.size(),
						VERTEX_ELEMENT_TYPE::BONES,
						m_heap_type,
						true,
						false) == false)
					{
						assert(0 && "failed create VB::BONES ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::WEIGHTS:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::WEIGHTS);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::WEIGHTS)].Create(
						m_weights.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_weights.size(),
						VERTEX_ELEMENT_TYPE::WEIGHTS,
						m_heap_type,
						true,
						false) == false)
					{
						assert(0 && "failed create VB::WEIGHTS ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::COLOR:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::COLOR);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::COLOR)].Create(
						m_colors.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_colors.size(),
						VERTEX_ELEMENT_TYPE::COLOR,
						m_heap_type) == false)
					{
						assert(0 && "failed create VB::COLOR ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::TEXCOORD0:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD0);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD0)].Create(
						m_uv0[0].data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_uv0[0].size(),
						VERTEX_ELEMENT_TYPE::TEXCOORD0,
						m_heap_type) == false)
					{
						assert(0 && "failed create VB::TEXCOORD0 ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::TEXCOORD1:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD1);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD1)].Create(
						m_uv1.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_uv1.size(),
						VERTEX_ELEMENT_TYPE::TEXCOORD1,
						m_heap_type) == false)
					{
						assert(0 && "failed create VB::TEXCOORD1 ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::TEXCOORD2:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD2);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD2)].Create(
						m_uv2.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_uv2.size(),
						VERTEX_ELEMENT_TYPE::TEXCOORD2,
						m_heap_type) == false)
					{
						assert(0 && "failed create VB::TEXCOORD2 ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::TEXCOORD3:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD3);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD3)].Create(
						m_uv3.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_uv3.size(),
						VERTEX_ELEMENT_TYPE::TEXCOORD3,
						m_heap_type) == false)
					{
						assert(0 && "failed create VB::TEXCOORD3 ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::TEXCOORD4:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD4);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD4)].Create(
						m_uv4.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_uv4.size(),
						VERTEX_ELEMENT_TYPE::TEXCOORD4,
						m_heap_type) == false)
					{
						assert(0 && "failed create VB::TEXCOORD4 ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::TEXCOORD5:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD5);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD5)].Create(
						m_uv5.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_uv5.size(),
						VERTEX_ELEMENT_TYPE::TEXCOORD5,
						m_heap_type) == false)
					{
						assert(0 && "failed create VB::TEXCOORD5 ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::TEXCOORD6:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD6);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD6)].Create(
						m_uv6.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_uv6.size(),
						VERTEX_ELEMENT_TYPE::TEXCOORD6,
						m_heap_type) == false)
					{
						assert(0 && "failed create VB::TEXCOORD6 ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				case VERTEX_ELEMENT_TYPE::TEXCOORD7:
				{
					constexpr int element_type = static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD7);
					if (m_VBs[static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD7)].Create(
						m_uv7.data(),
						VERTEX_ELEMENT::ELEMENT_SIZE[element_type],
						m_uv7.size(),
						VERTEX_ELEMENT_TYPE::TEXCOORD7,
						m_heap_type) == false)
					{
						assert(0 && "failed create VB::TEXCOORD7 ResMesh::CreateBuffers()");
						return;
					}
				}
				break;

				default:
					assert(0 && "not yet implemented.");
					return;
			}
		}

		// IB
		if (m_IB.Create(m_indices[0].data(), m_indices[0].size(), m_heap_type) == false)
		{
			assert(0 && "failed create IB ResMesh::CreateBuffers()");
			return;
		}

	}

	void ResMesh::ClearVerticesAndIndices()
	{
		m_tangents.clear();
		m_tangents.shrink_to_fit();
		m_binormals.clear();
		m_binormals.shrink_to_fit();
		m_bones.clear();
		m_bones.shrink_to_fit();
		m_weights.clear();
		m_weights.shrink_to_fit();
		m_colors.clear();
		m_colors.shrink_to_fit();
		m_uv1.clear();
		m_uv1.shrink_to_fit();
		m_uv2.clear();
		m_uv2.shrink_to_fit();
		m_uv3.clear();
		m_uv3.shrink_to_fit();
		m_uv4.clear();
		m_uv4.shrink_to_fit();
		m_uv5.clear();
		m_uv5.shrink_to_fit();
		m_uv6.clear();
		m_uv6.shrink_to_fit();
		m_uv7.clear();
		m_uv7.shrink_to_fit();
	}

	VECTOR3 ResMesh::CalculateNormal(const VECTOR3& p0, const VECTOR3& p1, const VECTOR3& p2)
	{
		VECTOR3 v_01 = p1 - p0;
		VECTOR3 v_02 = p2 - p0;

		return VECTOR3();
	}

	void ResMesh::CalculateTangentAndBinormalLH(
		const VECTOR3& p0, const VECTOR2& uv0,
		const VECTOR3& p1, const VECTOR2& uv1,
		const VECTOR3& p2, const VECTOR2& uv2,
		VECTOR3& tangent,
		VECTOR3& binormal)
	{
		// http://www.opengl-tutorial.org/jp/intermediate-tutorials/tutorial-13-normal-mapping/
		const VECTOR3 delta_pos1 = p1 - p0;
		const VECTOR3 delta_pos2 = p2 - p0;

		const VECTOR2 delta_uv1 = uv1 - uv0;
		const VECTOR2 delta_uv2 = uv2 - uv0;

		const float uv_cross = delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x;
		if (MyMath::Approximately(uv_cross, 0.0f))
		{
			// ポリゴンかUV上のポリゴンが縮退している
			tangent = VECTOR3::Zero;
			binormal = VECTOR3::Zero;
			return;
		}
		const float inv_uv_cross = 1.0f / uv_cross;
		tangent = (delta_pos1 * delta_uv2.y - delta_pos2 * delta_uv1.y) * inv_uv_cross;
		binormal = (delta_pos2 * delta_uv1.x - delta_pos1 * delta_uv2.x) * inv_uv_cross;

		tangent = tangent.Normalized();
		binormal = binormal.Normalized();

		// http://marupeke296.com/DXPS_No12_CalcTangentVectorSpace.html

		//// 5頂点 -> 3頂点にする
		//VECTOR3 cp0[3] = {
		//	VECTOR3(p0.x,uv0.x,uv0.y),
		//	VECTOR3(p0.y,uv0.x,uv0.y),
		//	VECTOR3(p0.z,uv0.x,uv0.y)
		//};
		//VECTOR3 cp1[3] = {
		//	VECTOR3(p1.x,uv1.x,uv1.y),
		//	VECTOR3(p1.y,uv1.x,uv1.y),
		//	VECTOR3(p1.z,uv1.x,uv1.y)
		//};
		//VECTOR3 cp2[3] = {
		//	VECTOR3(p2.x,uv2.x,uv2.y),
		//	VECTOR3(p2.y,uv2.x,uv2.y),
		//	VECTOR3(p2.z,uv2.x,uv2.y)
		//};

		//// 3点の平面からUV軸座標を計算
		//float U[3], V[3];
		//for (int i = 0; i < 3; ++i)
		//{
		//	VECTOR3 v1 = cp1[i] - cp0[i];
		//	v1 = v1.Normalized();
		//	VECTOR3 v2 = cp2[i] - cp0[i];
		//	v2 = v2.Normalized();
		//	// ABCはポリゴンの法線
		//	VECTOR3 ABC = VECTOR3::Cross(v1, v2);
		//	ABC = ABC.Normalized();

		//	if (MyMath::Approximately(ABC.x, 0.0f) == true)
		//	{
		//		// ポリゴンかUV上のポリゴンが縮退している
		//		assert(0);
		//		tangent = VECTOR3::Zero;
		//		binormal = VECTOR3::Zero;
		//		return;
		//	}
		//	U[i] = -ABC.y / ABC.x;
		//	V[i] = -ABC.z / ABC.x;
		//}
		//tangent.x = U[0];
		//tangent.y = U[1];
		//tangent.z = U[2];

		//binormal.x = V[0];
		//binormal.y = V[1];
		//binormal.z = V[2];

		//tangent = tangent.Normalized();
		//binormal = binormal.Normalized();
	}

	// ~ResMesh

}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::ResMesh);
CEREAL_CLASS_VERSION(TKGEngine::ResMesh, 1);
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::IResMesh, TKGEngine::ResMesh)
