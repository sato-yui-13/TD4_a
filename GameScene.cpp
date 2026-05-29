#include "GameScene.h"
#include <iostream>

using namespace KamataEngine;

GameScene::~GameScene()
{
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine)
		{
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();


	//--------------------3Dモデルデータの解放----------------------//
	delete player_;
	delete modelPlayer_;
	delete boss_;
	delete modelBlock_;
	delete mapChipField_;
	delete debugCamera_;
}

void GameScene::Initialize()
{

	//ワールドトランスフォーム
	worldTransform_.Initialize();

	// カメラの初期化
	camera_.Initialize();
	camera_.farZ = 1000.0f;

	//デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks_csv/blocks.csv");

	player_ = new player();
	boss_ = new boss();
	//カメラコントローラーの生成
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);

	CameraController::Rect cameraArea = { 12.0f,100 - 12.0f,6.0f,6.0f };
	cameraController_->SetMovebleArea(cameraArea);


	Vector3 center = mapChipField_->GetCenterPosition();

	//カメラの位置
	camera_.translation_ =
	{
		center.x,
		20.0f,
		center.z - 30.0f
	};
	camera_.rotation_ = { 0.9f, 0.0f, 0.0f };

	//------------------3Dモデルデータの生成-----------------//
	modelBlock_ = Model::CreateFromOBJ("block");/*ブロック*/
	modelPlayer_ = Model::CreateFromOBJ("player", true);/*自機*/
	modelBoss_ = Model::CreateFromOBJ("boss", true);/*ボス*/

	GenerateBlocks();

	
	player_->SetMapChipField(mapChipField_);

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 5);

	Vector3 BossPosition = mapChipField_->GetMapChipPositionByIndex(50, 10);

	playerPosition.y += 1.0f;

	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	boss_->Initialize(modelBoss_, &camera_, BossPosition);

	cameraController_->Reset();

	//ゲームタイトルから開始
	phase_ = Phase::kPlay;

}

void GameScene::Update()
{

	switch (phase_)
	{
	case Phase::kPlay:

		//自機の更新
		player_->Update();
		Vector3 playerPos = player_->GetWorldPosition();
		//敵の更新
		boss_->Update();
		Vector3 bossPos = boss_->GetWorldPosition();




		// ここで当たり判定
		if (IsCollision(player_->GetAABB(), boss_->GetAABB()))
		{
			std::cout << "プレイヤーとボスが衝突！\n";
		}









		//ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
		{
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine)
			{
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の生成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}

#ifdef _DEBUG

		if (Input::GetInstance()->TriggerKey(DIK_0))
		{
			isDebugCameraActive_ = !isDebugCameraActive_;
		}

#endif // DEBUG

		//カメラの処理
		if (isDebugCameraActive_)
		{
			//デバックカメラの更新
			debugCamera_->Update();
			//デバックカメラのビュー行列
			camera_.matView = debugCamera_->GetCamera().matView;
			//デバックカメラのプロジェクション行列
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			//ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} 
		else
		{
			//ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();

			camera_.matView = cameraController_->GetViewProjection().matView;
			camera_.matProjection = cameraController_->GetViewProjection().matProjection;

			//ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		}
		cameraController_->Update();

		for (auto& line : worldTransformBlocks_)
		{
			for (auto& block : line)
			{
				if (!block) continue;

				block->matWorld_ = MakeAffineMatrix(
					block->scale_,
					block->rotation_,
					block->translation_
				);

				block->TransferMatrix();
			}
		}

		break;
	case Phase::kDeath:

		//敵の更新
		boss_->Update();
		/*Vector3 bossPos = boss_->GetWorldPosition();*/

		//ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
		{
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine)
			{
				if (!worldTransformBlock)
					continue;
				// アフィン変換行列の生成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}

#ifdef _DEBUG

		if (Input::GetInstance()->TriggerKey(DIK_0))
		{
			isDebugCameraActive_ = !isDebugCameraActive_;
		}

#endif // DEBUG

		//カメラの処理
		if (isDebugCameraActive_)
		{
			//デバックカメラの更新
			debugCamera_->Update();
			//デバックカメラのビュー行列
			camera_.matView = debugCamera_->GetCamera().matView;
			//デバックカメラのプロジェクション行列
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			//ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} 
		else
		{
			//ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();

			camera_.matView = cameraController_->GetViewProjection().matView;
			camera_.matProjection = cameraController_->GetViewProjection().matProjection;

			//ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		}

		break;
	case Phase::kClear:
		break;
	}
}

void GameScene::Draw()
{

	
	Model::PreDraw();

	player_->Draw();
	boss_->Draw();


	//ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform*& worldTransformBlock : worldTransformBlockLine)
		{
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	Model::PostDraw();

}

void GameScene::GenerateBlocks()
{
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	worldTransformBlocks_.resize(numBlockVirtical);

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i)
	{
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i)
	{
		for (uint32_t j = 0; j < numBlockHorizontal; ++j)
		{
			// マップチップ種類を取得
			MapChipType type =
				mapChipField_->GetMapChipTypeByIndex(j, i);

			// 空白なら作らない
			if (type == MapChipType::kBlank)
			{
				continue;
			}

			WorldTransform* worldTransform =
				new WorldTransform();

			worldTransform->Initialize();

			worldTransformBlocks_[i][j] = worldTransform;

			worldTransformBlocks_[i][j]->translation_ =
				mapChipField_->GetMapChipPositionByIndex(j, i);

			// 床
			if (type == MapChipType::kBlock)
			{
				worldTransformBlocks_[i][j]->scale_ =
				{
					1.0f,
					0.1f,
					1.0f
				};

				worldTransformBlocks_[i][j]->translation_.y = -0.05f;
			}

			// 壁
			if (type == MapChipType::kWall)
			{
				worldTransformBlocks_[i][j]->scale_ =
				{
					1.0f,
					6.0f,
					1.0f
				};

				worldTransformBlocks_[i][j]->translation_.y = 3.0f;
			}
		}
	}
}

void GameScene::ChangePhase()
{

	switch (phase_)
	{
	case GameScene::Phase::kPlay:

		break;
	case GameScene::Phase::kDeath:
		break;
	case GameScene::Phase::kClear:
		break;
	}

}
