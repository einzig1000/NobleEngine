#pragma once
#include <cstdint>
#include <wrl.h>
#include <d3d12.h>

class FrameCbAllocator
{
public:
	struct Allocation
	{
		void* cpu = nullptr;
		D3D12_GPU_VIRTUAL_ADDRESS gpu = 0;
		size_t size = 0;
	};

	FrameCbAllocator() = default;
	~FrameCbAllocator() = default;

	void Initialize(ID3D12Device* device, size_t capacityBytes, const wchar_t* debugName = L"FrameCbAllocator");
	void Reset();

	Allocation Allocate(size_t sizeBytes);

private:
	static size_t AlignUp(size_t value, size_t alignment)
	{
		return (value + (alignment - 1)) & ~(alignment - 1);
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	uint8_t* mapped_ = nullptr;
	size_t capacity_ = 0;
	size_t offset_ = 0;
};