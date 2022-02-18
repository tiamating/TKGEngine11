#pragma once

#include <Windows.h>

struct ID3D11Query;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace TKGEngine::Graphics
{
	class IQuery
	{
	public:
		IQuery() = default;
		virtual ~IQuery() = default;
		IQuery(const IQuery&) = delete;
		IQuery& operator=(const IQuery&) = delete;

		virtual bool Init(ID3D11Device* p_device) = 0;
		virtual void Release() = 0;

		/// <summary>
		/// Return query not used last frame.
		/// </summary>
		virtual ID3D11Query* GetQuery() const = 0;
	};

	class IQueryDisjoint
		: public IQuery
	{
	public:
		IQueryDisjoint() = default;
		virtual ~IQueryDisjoint() = default;
		IQueryDisjoint(const IQueryDisjoint&) = delete;
		IQueryDisjoint& operator=(const IQueryDisjoint&) = delete;

		static IQueryDisjoint* CreateInterface();

		virtual void Begin(ID3D11DeviceContext* p_ic) = 0;
		virtual void End(ID3D11DeviceContext* p_ic) = 0;

		/// <summary>
		/// Wait for query data to be available 
		/// and 
		/// Check whether timestamps were disjoint during the last frame
		/// </summary>
		/// <param name="p_ic">Immediate Context pointer</param>
		/// <returns>true : timestamp is joint. false : timestamp is not joint</returns>
		virtual bool WaitAndCheckJoint(ID3D11DeviceContext* p_ic) = 0;
		virtual void WaitForIdle(ID3D11DeviceContext* p_ic) = 0;
		virtual UINT64 GetFrequency() const = 0;
	};

	class IQueryTimeStamp
		: public IQuery
	{
	public:
		IQueryTimeStamp() = default;
		virtual ~IQueryTimeStamp() = default;
		IQueryTimeStamp(const IQueryTimeStamp&) = delete;
		IQueryTimeStamp& operator=(const IQueryTimeStamp&) = delete;

		static IQueryTimeStamp* CreateInterface();

		virtual void End(ID3D11DeviceContext* p_ic) = 0;
		virtual void GetData(ID3D11DeviceContext* p_ic) = 0;

		virtual UINT64 GetTimeStamp() const = 0;
	};
}// namespace TKGEngine::Graphics