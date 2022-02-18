#pragma once
#include "Systems/inc/TKGEngine_Defined.h"

#include "Application/Resource/inc/Asset_Defined.h"

#include <string>
#include <vector>
#include <unordered_map>

#include <memory>

namespace TKGEngine::Animations
{
	struct KeyData;
}// namespace TKGEngine::Animations

namespace TKGEngine
{
	class IResMotion;

	class Motion
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Motion(const Motion&) = delete;

		Motion() = default;
		virtual ~Motion() = default;
		Motion& operator=(const Motion&);

		static void RemoveUnused();

		void Load(const std::string& filename);
		void LoadAsync(const std::string& filename);

		void Release();

		float GetSampleRate() const;
		float GetMotionLength() const;


		bool IsLoaded() const;
		bool HasMotion() const;
		const char* GetName() const;
		const char* GetFilepath() const;

		unsigned GetHash() const;

		std::vector<Animations::KeyData>* GetKeyFrame(const int frame) const;
		std::unordered_map<std::string, int>* GetKeyIndex() const;
		

		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================


		// ==============================================
		// private variables
		// ==============================================
		std::shared_ptr<IResMotion> m_res_motion = nullptr;
	};


}	// namespace TKGEngine