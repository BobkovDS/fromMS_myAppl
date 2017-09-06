#include "FrameResource.h"



FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT VerticesCount, UINT IndicesCount)
{
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdListAllocator.GetAddressOf()));

	passCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	perObjectCB = std::make_unique<UploadBuffer<ObjectContants>>(device, objectCount, true);

	vertices = std::make_unique<UploadBuffer<Vertex>>(device, VerticesCount, false);
	indices = std::make_unique<UploadBuffer<UINT32>>(device, IndicesCount, false);
}


FrameResource::~FrameResource()
{
}
