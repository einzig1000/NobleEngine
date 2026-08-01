#include "FrameProfiler.h"

FrameProfiler::FrameProfiler(ID3D12Device2* device, ID3D12CommandQueue* commandQueue)
{
    // GPUタイムスタンプの周波数(1秒あたりのtick数)を取得。tick→秒の変換に使う
    HRESULT hr = commandQueue->GetTimestampFrequency(&gpuFrequency_);
    assert(SUCCEEDED(hr));

    // フレームバッファ枚数(kFrameCount)分 × 2発(開始/終了)のタイムスタンプクエリ
    D3D12_QUERY_HEAP_DESC queryHeapDesc{};
    queryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
    queryHeapDesc.Count = Constexprs::kFrameCount * kQueriesPerFrame;
    hr = device->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&queryHeap_));
    assert(SUCCEEDED(hr));

    // クエリ結果の読み出し先(READBACKヒープ)
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = sizeof(uint64_t) * Constexprs::kFrameCount * kQueriesPerFrame;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_READBACK;

    hr = device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
        IID_PPV_ARGS(&readbackBuffer_));
    assert(SUCCEEDED(hr));
    readbackBuffer_->SetName(L"FrameProfiler::readbackBuffer_");

    // READBACKバッファはCPUから読むだけなので永続的にMapしっぱなしでOK
    D3D12_RANGE readRange{ 0, 0 }; // 初回はまだ何も書かれていないのでrange={0,0}
    readbackBuffer_->Map(0, &readRange, &mappedReadback_);
}

FrameProfiler::~FrameProfiler()
{}

void FrameProfiler::OnFrameSynced(UINT frameIndex)
{
    // WaitForGPU(frameIndex)が完了した直後
    const uint64_t* ticks = reinterpret_cast<const uint64_t*>(
        static_cast<const uint8_t*>(mappedReadback_) + sizeof(uint64_t) * kQueriesPerFrame * frameIndex);

    const uint64_t begin = ticks[0];
    const uint64_t end = ticks[1];

    // まだ一度もこのframeIndexで書き込んでいない場合はbegin/endが両方0なのでスキップ
    if (end > begin)
    {
        const double seconds = double(end - begin) / double(gpuFrequency_);
        gpuDeltaTimeMs_ = static_cast<float>(seconds * 1000.0);
    }
}

void FrameProfiler::BeginGpuScope(ID3D12GraphicsCommandList6* cmdList, UINT frameIndex)
{
    cmdList->EndQuery(queryHeap_.Get(), D3D12_QUERY_TYPE_TIMESTAMP, frameIndex * kQueriesPerFrame + 0);
}

void FrameProfiler::EndGpuScope(ID3D12GraphicsCommandList6* cmdList, UINT frameIndex)
{
    cmdList->EndQuery(queryHeap_.Get(), D3D12_QUERY_TYPE_TIMESTAMP, frameIndex * kQueriesPerFrame + 1);

    // クエリ結果をREADBACKバッファへコピー。
    // 実際にCPUから読めるのは、次回このframeIndexが回ってきてWaitForGPUした後(OnFrameSynced)。
    cmdList->ResolveQueryData(
        queryHeap_.Get(), D3D12_QUERY_TYPE_TIMESTAMP,
        frameIndex * kQueriesPerFrame, kQueriesPerFrame,
        readbackBuffer_.Get(), sizeof(uint64_t) * kQueriesPerFrame * frameIndex);
}

void FrameProfiler::BeginCpuScope()
{
    cpuBeginTime_ = std::chrono::steady_clock::now();
}

void FrameProfiler::EndCpuScope()
{
    const auto now = std::chrono::steady_clock::now();
    cpuDeltaTimeMs_ = std::chrono::duration<float, std::milli>(now - cpuBeginTime_).count();
}