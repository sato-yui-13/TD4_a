#include "GameScene.h"
#include <iostream>
#include <algorithm>
#undef min


using namespace KamataEngine;

Vector3 GetOverlap(const AABB& a, const AABB& b)
{
	Vector3 overlap;

	overlap.x = std::min(a.max.x - b.min.x, b.max.x - a.min.x);
	overlap.y = std::min(a.max.y - b.min.y, b.max.y - a.min.y);
	overlap.z = std::min(a.max.z - b.min.z, b.max.z - a.min.z);

	return overlap;
}
void ResolveCollision(Vector3& playerPos, Vector3& bossPos, const AABB& playerBox, const AABB& bossBox)
{
	Vector3 overlap;

	overlap.x = std::min(playerBox.max.x - bossBox.min.x, bossBox.max.x - playerBox.min.x);
	overlap.y = std::min(playerBox.max.y - bossBox.min.y, bossBox.max.y - playerBox.min.y);
	overlap.z = std::min(playerBox.max.z - bossBox.min.z, bossBox.max.z - playerBox.min.z);

	if (overlap.x <= overlap.y && overlap.x <= overlap.z)
	{
		float push = overlap.x / 2.0f;

		if (playerPos.x < bossPos.x)
		{
			playerPos.x -= push;
			bossPos.x += push;
		} else
		{
			playerPos.x += push;
			bossPos.x -= push;
		}
	} else if (overlap.y <= overlap.x && overlap.y <= overlap.z)
	{
		float push = overlap.y / 2.0f;

		if (playerPos.y < bossPos.y)
		{
			playerPos.y -= push;
			bossPos.y += push;
		} else
		{
			playerPos.y += push;
			bossPos.y -= push;
		}
	} else
	{
		float push = overlap.z / 2.0f;

		if (playerPos.z < bossPos.z)
		{
			playerPos.z -= push;
			bossPos.z += push;
		} else
		{
			playerPos.z += push;
			bossPos.z -= push;
		}
	}
}


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
	delete boss_at_model_;
	delete attack_;
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
	modelattack_ = Model::CreateFromOBJ("action", true);
	//modelSkydome_ = Model::CreateFromOBJ("skyDome", true);/*天球*/

	// サウンドデータの読み込み
	/*BGMHandle = Audio::GetInstance()->LoadWave("sound/1123.mp3");
	voiceHandle = Audio::GetInstance()->PlayWave(BGMHandle, true, 0.1f);*/

	// skydome生成
	//skydome_ = new Skydome();
	// 初期化
	//skydome_->Initialize(modelSkydome_, &camera_);

	GenerateBlocks();

	player_->SetMapChipField(mapChipField_);

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 5);
	Vector3 BossPosition = mapChipField_->GetMapChipPositionByIndex(50, 10);

	playerPosition.y += 1.0f;
	BossPosition.y += 1.0f;

	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	boss_->Initialize(modelBoss_, &camera_, BossPosition);

	boss_at_model_ = Model::CreateFromOBJ("bullet");
	boss_->atInitialize(boss_at_model_, &camera_, boss_->attackPosition_);

	cameraController_->Reset();

	attack_ = new Attack();
	attack_->Initialize(modelattack_, &camera_, playerPosition);

	//ゲームタイトルから開始
	phase_ = Phase::kPlay;

}

void GameScene::Update()
{

	switch (phase_)
	{
	case Phase::kPlay:

		//skydome更新
		//skydome_->Update();

		//自機の更新
		player_->Update();
		Vector3 playerPos = player_->GetWorldPosition();
		//敵の更新
		boss_->Update();
		Vector3 bossPos = boss_->GetWorldPosition();
		boss_->atUpdate();
		boss_->SetPlayer(player_);
		// ここで当たり判定
		//AABB取得
		AABB playerBox = player_->GetAABB();
		AABB bossBox = boss_->GetAABB();

		//衝突してたら押し戻し
		if (IsCollision(playerBox, bossBox))
		{
			ResolveCollision(playerPos, bossPos, playerBox, bossBox);

			// ★ここ超重要：位置をオブジェクトに反映
			player_->SetWorldPosition(playerPos);
			boss_->SetWorldPosition(bossPos);
		}


		attack_->Update(); // 攻撃の時間管理と入力処理

		// 攻撃中ならボスとの判定
		if (attack_->IsAttacking())
		{
			// プレイヤー位置
			//Vector3 playerPos = player_->GetWorldPosition();

			// プレイヤーの向き
			float angle = player_->GetWorldTransform().rotation_.y;
			Vector3 forward = { sinf(angle), 0.0f, cosf(angle) };

			// 攻撃範囲
			AABB attackBox = player_->GetAABB();
			float attackRange = 1.5f;

			attackBox.min.x += forward.x * attackRange;
			attackBox.max.x += forward.x * attackRange;
			attackBox.min.z += forward.z * attackRange;
			attackBox.max.z += forward.z * attackRange;

			// ボスとの当たり判定
			if (IsCollision(attackBox, boss_->GetAABB()))
			{
				std::cout << "ヒット！\n";
				boss_->TakeDamage(2);
			}
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
		} else
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

		//ボス倒したらクリア画面に行く
		if (boss_->IsDead())
		{
			phase_ = Phase::kClear;

			clearFlag = true;

			//phase_ = Phase::kClear;   // 内部フラグは残す
			///scene_ = Scene::kClear;    // ← これで画面も切り替わる
			std::cout << "CLEAR!!\n";
		}

		break;
	case Phase::kDeath:

		//敵の更新
		boss_->Update();

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
		} else
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
	boss_->atDraw();
	// 天球描画
	//skydome_->Draw();

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
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(j, i);

			// 空白なら作らない
			if (type == MapChipType::kBlank)
			{
				continue;
			}

			WorldTransform* worldTransform = new WorldTransform();

			worldTransform->Initialize();

			worldTransformBlocks_[i][j] = worldTransform;
			worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);

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
