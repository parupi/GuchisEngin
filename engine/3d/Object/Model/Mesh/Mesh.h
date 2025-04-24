#pragma once

class SrvManager;
class DirectXManager;

class Mesh {
public:
	// 初期化処理
	void Initialize();
	// 

private:

	// 頂点データの生成
	void CreateVertexResource();
	// Indexデータの生成
	void CreateIndexResource();
};