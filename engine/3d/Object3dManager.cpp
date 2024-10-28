#include "Object3dManager.h"

Object3dManager* Object3dManager::instance = nullptr;
std::once_flag Object3dManager::initInstanceFlag;

Object3dManager* Object3dManager::GetInstance()
{
	std::call_once(initInstanceFlag, []() {
		instance = new Object3dManager();
		});
	return instance;
}

void Object3dManager::Initialize(DirectXManager* directXManager)
{
	assert(directXManager);
	dxManager_ = directXManager;

	CreateRootSignature();
	inputLayoutDesc_ = CreateInputElementDesc();
	CreateBlendState();
	CreateRasterizerState();
	LoadShader();
	CreatePipelineState();
}

void Object3dManager::DrawSet()
{
	dxManager_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	dxManager_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());			// PSOを設定
	dxManager_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3dManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;														// 0から始まる
	descriptorRange[0].NumDescriptors = 1;															// 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;									// SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// Offsetを自動計算

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。PixelShaderのMaterialとVertezShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[13] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;			// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;			// レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;			// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;		// VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;							// レジスタ番号0を使う

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;			// DescriptorTableで使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;					// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);		// Tableで利用する数
	// カメラ
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;						// DescriptorTableで使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;	
	// 平行光源
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;						// DescriptorTableで使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 2;	// レジスタ番号2を使う
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;						// DescriptorTableで使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[5].Descriptor.ShaderRegister = 3;	// レジスタ番号2を使う
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;						// DescriptorTableで使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[6].Descriptor.ShaderRegister = 4;	// レジスタ番号2を使う
	// 点光源
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;						// DescriptorTableで使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[7].Descriptor.ShaderRegister = 5;	// レジスタ番号2を使う
	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;						// DescriptorTableで使う
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[8].Descriptor.ShaderRegister = 6;	// レジスタ番号2を使う
	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;						// DescriptorTableで使う
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[9].Descriptor.ShaderRegister = 7;	// レジスタ番号2を使う
	// スポットライト
	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;						// DescriptorTableで使う
	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[10].Descriptor.ShaderRegister = 8;	// レジスタ番号2を使う
	rootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;						// DescriptorTableで使う
	rootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[11].Descriptor.ShaderRegister = 9;	// レジスタ番号2を使う
	rootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;						// DescriptorTableで使う
	rootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;						// PixelShaderで使う
	rootParameters[12].Descriptor.ShaderRegister = 10;	// レジスタ番号2を使う

	descriptionRootSignature.pParameters = rootParameters;						// ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);			// 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする

	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリをもとに生成

	hr = dxManager_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

D3D12_INPUT_LAYOUT_DESC Object3dManager::CreateInputElementDesc()
{
	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	return inputLayoutDesc;
}

void Object3dManager::CreateBlendState()
{
	// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = TRUE;
	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

void Object3dManager::CreateRasterizerState()
{
	// 裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
}

void Object3dManager::LoadShader()
{
	// Shaderをコンパイルする
	vertexShaderBlob_ = dxManager_->CompileShader(L"./resource/shaders/Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxManager_->CompileShader(L"./resource/shaders/Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

void Object3dManager::CreatePipelineState()
{
	HRESULT hr;

	// PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();			// RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;			// InputLayout
	//graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;			// InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };			// vertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };			// PixelShade
	graphicsPipelineStateDesc.BlendState = blendDesc_;					// BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;			// RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 実際に生成
	//Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	hr = dxManager_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}
