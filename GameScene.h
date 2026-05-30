#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include "boss.h"
#include "Attack.h"
#include "CameraController.h"
//#include "Skydome.h"
#include <vector>

//ゲームシーン
class GameScene
{
public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
	//デストラクタ
	~GameScene();

	//表示ブロック
	void GenerateBlocks();

	//ゲームのフェーズ
	enum class Phase
	{
		kPlay,
		kDeath,
		kClear
	};

	//クリアフラグ
	int clearFlag = false;

	//フェーズの切り替え
	void ChangePhase();

private:

	//ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;

	//カメラ
	KamataEngine::Camera camera_;

	//デバックカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	//ブロック用のワールドトランスフォーム
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	//マップチップフィールド
	MapChipField* mapChipField_;

	player* player_;

	boss* boss_;

	Attack* attack_;

	//Skydome* skydome_ = nullptr;

	CameraController* cameraController_ = nullptr;

	//--------------------3Dモデル----------------------//
	Model* model_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelBoss_ = nullptr;
	//アタックモデル
	KamataEngine::Model* boss_at_model_ = nullptr;
	KamataEngine::Model* modelattack_ = nullptr;
	Model* modelSkydome_ = nullptr;

	//デバックカメラ有効
	bool isDebugCameraActive_ = false;

	//ゲームの現在のフェーズ
	Phase phase_;

};

