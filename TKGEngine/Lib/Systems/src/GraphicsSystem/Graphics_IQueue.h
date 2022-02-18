#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace TKGEngine::Graphics
{
	class ICommandList;

	class IQueue
	{
	public:
		static IQueue* CreateInterface();

		IQueue() = default;
		virtual ~IQueue() = default;
		IQueue(const IQueue&) = delete;
		IQueue& operator=(const IQueue&) = delete;

		virtual bool Init(
			ID3D11Device* p_device, 
			int num_passes/* number of using pass */, 
			int num_threads/* number of using thread */) = 0;
		virtual void Release() = 0;

		// Execute QueryDisjoint::Begin -> FinishCommandList -> ExecuteCommandList -> QueryTS::End -> QueryDisjoint::End
		virtual void Execute(ID3D11DeviceContext* p_ic, int thread_idx, int pass_idx) = 0;

		virtual ICommandList* GetCommandList(int thread_idx, int pass_idx) const = 0;
	};

}	// namespace TKGEngine::Graphics