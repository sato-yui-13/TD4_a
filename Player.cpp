#define NOMINMAX
#include "player.h"
#include "MapChipField.h"
#include "KamataEngine.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include <iostream>


void player::Initialize(Model* model, Camera* camera, const Vector3& position)
{
	assert(model);

	model_ = model;
	//引数の内容をメンバ変数に記録
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	worldTransform_.scale_ = { 1.0f, 1.0f, 1.0f };

	worldTransform_.TransferMatrix();
	
	//attack_ = new Attack();
}

void player::Blink()
{

	// ブリンク中
	if (isBlink_) 
	{
		return;
	}

	// クールダウン中
	if (blinkCooldownTimer_ > 0.0f) 
	{
		return;
	}

	// Shift押した瞬間
	if (Input::GetInstance()->TriggerKey(DIK_LSHIFT)) 
	{

		isBlink_ = true;

		// クールダウン開始
		blinkCooldownTimer_ = blinkCooldown_;

		blinkTimer_ = 0.0f;

		blinkStartPos_ = worldTransform_.translation_;
		blinkEndPos_ = blinkStartPos_;

		//n nconst float kBlinkDistance = 8.0f;

		if (Input::GetInstance()->PushKey(DIK_W)) 
		{
			blinkEndPos_.z += kBlinkDistance;
		}

		if (Input::GetInstance()->PushKey(DIK_S)) 
		{
			blinkEndPos_.z -= kBlinkDistance;
		}

		if (Input::GetInstance()->PushKey(DIK_A)) 
		{
			blinkEndPos_.x -= kBlinkDistance;
		}

		if (Input::GetInstance()->PushKey(DIK_D)) 
		{
			blinkEndPos_.x += kBlinkDistance;
		}
	}
}

void player::InputMove()
{
	
	
	
	// ==========================
    // 移動入力（押している間移動）
   // ==========================
	Vector3 moveDirection = { 0, 0, 0 };

	if (Input::GetInstance()->PushKey(DIK_D)) moveDirection.x += 1.0f;
	if (Input::GetInstance()->PushKey(DIK_A)) moveDirection.x -= 1.0f;
	if (Input::GetInstance()->PushKey(DIK_W)) moveDirection.z += 1.0f;
	if (Input::GetInstance()->PushKey(DIK_S)) moveDirection.z -= 1.0f;

	// ==========================
	// 向き変更（押した瞬間だけ）
	// ==========================
	if (Input::GetInstance()->TriggerKey(DIK_D))
	{
		worldTransform_.rotation_.y = 1.57f; // 右
	} else if (Input::GetInstance()->TriggerKey(DIK_A))
	{
		worldTransform_.rotation_.y = -1.57f; // 左
	} else if (Input::GetInstance()->TriggerKey(DIK_W))
	{
		worldTransform_.rotation_.y = 0.0f; // 前
	} else if (Input::GetInstance()->TriggerKey(DIK_S))
	{
		worldTransform_.rotation_.y = 3.14f; // 後ろ
	}

	// ==========================
	// 移動反映
	// ==========================
	float speed = 0.2f; // 好きな速度に調整
	worldTransform_.translation_.x += moveDirection.x * speed;
	worldTransform_.translation_.z += moveDirection.z * speed;
	
	
	// ==========================
	// velocity に反映
	// ==========================

	velocity_.x = moveDirection.x * 0.1f;
	velocity_.z = moveDirection.z * 0.1f;

	// ジャンプ
	if (onGround_) {

		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

			velocity_.y = 0.6f;

			onGround_ = false;
		}
	}

	// 重力
	if (!onGround_) {

		velocity_.y -= 0.03f;

		velocity_.y = std::max(velocity_.y, -1.0f);
	}

	// 移動
	worldTransform_.translation_.y += velocity_.y;

	// 地面高さ
	const float groundY = 1.0f;

	// 着地
	if (worldTransform_.translation_.y <= groundY) {

		worldTransform_.translation_.y = groundY;

		velocity_.y = 0.0f;

		onGround_ = true;
	}

}

// 02_10 10枚目
Vector3 player::GetWorldPosition()
{
	Vector3 worldPos{};
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

void player::CheckMapCollision(CollisionMapInfo& info)
{
	//CheckMapCollisionUp(info);
	//CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
	CheckMapCollisionFront(info);
	CheckMapCollisionBack(info);
}

void player::CheckMapCollisionUp(CollisionMapInfo& info)
{

	// 02_07スライド20枚目 上昇あり?
	if (info.move.y <= 0) 
	{
		return;
	}

	// 02_07 スライド19枚目（下のfor文も）
	std::array<Vector3, kNumCorner> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) 
	{
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	// 02_07 スライド28枚目（下のfor文も）
	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kWall) 
	{
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kWall) 
	{
		hit = true;
	}

	// ブロックにヒット？ 02_07 スライド34枚目
	if (hit) 
	{
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) 
		{
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, +kHeight / 2.0f, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
			info.ceiling = true;
		}
	}
}

void player::CheckMapCollisionDown(CollisionMapInfo& info)
{
	info;

	// 02_08 スライド7枚目 下降あり？
	if (info.move.y >= 0) 
	{
		return;
	}

	// 02_08 スライド7枚目（下のfor文も）
	std::array<Vector3, kNumCorner> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) 
	{
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	// 02_08 スライド8枚目(右下、左下の判定まで)
	MapChipType mapChipType;

	// フラグ初期化
	bool hit = false;

	// 左下の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kWall) 
	{
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kWall) 
	{
		hit = true;
	}

	// 02_08スライド11枚目 ブロックにヒット？
	if (hit) 
	{
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(0, -kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (kHeight / 2.0f));
		// 地面に当たったことを記録する
		info.landing = true;
	}

}


void player::UpdateOnGround(const CollisionMapInfo& info)
{
	info;

	if (onGround_) 
	{
		// 02_08スライド18枚目 ジャンプ開始
		if (velocity_.y > 0.0f) 
		{
			onGround_ = false;
		} 
		else 
		{
			// 落下判定
			// 落下なら空中状態に切り替え

			// 02_08スライド19枚目(このelseブロック全部)
			std::array<Vector3, kNumCorner> positionsNew{};

			for (uint32_t i = 0; i < positionsNew.size(); ++i) 
			{
				positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
			}

			bool hit = false;

			MapChipType mapChipType;

			// 左下点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) 
			{
				hit = true;
			}

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom] + Vector3(0, -kGroundSearchHeight, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) 
			{
				hit = true;
			}

			// 落下開始
			if (!hit) 
			{
				onGround_ = false;
			}
		}
	} 
	else 
	{

		// 02_08スライド16枚目 地面に接触している場合の処理
		if (info.landing) 
		{
			// 着地状態に切り替える（落下を止める）
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロに
			velocity_.y = 0.0f;
		}
	}

}

void player::UpdateOnWall(const CollisionMapInfo& info)
{
	if (info.hitWall) 
	{
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void player::CheckMapCollisionRight(CollisionMapInfo& info)
{
	if (info.move.x <= 0) 
	{
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) 
	{
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右側の当たり判定
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kWall) 
	{
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kWall) 
	{
		hit = true;
	}

	// ブロックにヒット？
	if (hit) 
	{
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) 
		{
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(+kWidth / 2.0f, 0, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			info.hitWall = true;
		}
	}

}

void player::SetWorldPosition(const Vector3& pos)
{
	worldTransform_.translation_ = pos;
}

void player::CheckMapCollisionLeft(CollisionMapInfo& info)
{

	if (info.move.x >= 0) 
	{
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) 
	{
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右側の当たり判定
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kWall) 
	{
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kWall) 
	{
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(-kWidth / 2.0f, 0, 0));

		if (indexSetNow.xIndex != indexSet.xIndex) 
		{
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + Vector3(-kWidth / 2.0f, 0, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth / 2.0f + kBlank));
			info.hitWall = true;
		}
	}


}

void player::CheckMapCollisionFront(CollisionMapInfo& info)
{
	if (info.move.z <= 0) {
		return;
	}

	Vector3 newPos = worldTransform_.translation_ + info.move;

	Vector3 checkPositions[] = {
		newPos + Vector3(+kWidth / 2.0f, 0, +kDepth / 2.0f),
		newPos + Vector3(-kWidth / 2.0f, 0, +kDepth / 2.0f),
	};

	bool hit = false;
	MapChipField::IndexSet indexSet{};

	for (Vector3 pos : checkPositions) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kWall) {
			hit = true;
			break;
		}
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		info.move.z = std::max(
			0.0f,
			rect.bottom - worldTransform_.translation_.z - (kDepth / 2.0f + kBlank)
		);

		info.hitWall = true;
	}
}

void player::CheckMapCollisionBack(CollisionMapInfo& info)
{
	if (info.move.z >= 0) {
		return;
	}

	Vector3 newPos = worldTransform_.translation_ + info.move;

	Vector3 checkPositions[] = {
		newPos + Vector3(+kWidth / 2.0f, 0, -kDepth / 2.0f),
		newPos + Vector3(-kWidth / 2.0f, 0, -kDepth / 2.0f),
	};

	bool hit = false;
	MapChipField::IndexSet indexSet{};

	for (Vector3 pos : checkPositions) {
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(pos);
		if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kWall) {
			hit = true;
			break;
		}
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		info.move.z = std::min(
			0.0f,
			rect.top - worldTransform_.translation_.z + (kDepth / 2.0f + kBlank)
		);

		info.hitWall = true;
	}
}

Vector3 player::CornerPosition(const Vector3& center, Corner corner)
{
	Vector3 offsetTable[] = {
			{+kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kRightBottom
			{-kWidth / 2.0f, -kHeight / 2.0f, 0}, //  kLeftBottom
			{+kWidth / 2.0f, +kHeight / 2.0f, 0}, //  kRightTop
			{-kWidth / 2.0f, +kHeight / 2.0f, 0}  //  kLeftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void player::TakeDamage(int damage)
{
	hp_ -= damage;

	if (hp_ < 0)
	{
		hp_ = 0;
	}

	std::cout << "プレイヤーHP: " << hp_ << std::endl;
}

AABB player::GetAABB() const
{
	// プレイヤーの中心座標
	Vector3 center = worldTransform_.translation_;

	// 当たり判定の半サイズをその場で指定
	Vector3 halfSize = { 0.5f, 1.0f, 0.5f };

	AABB aabb;
	aabb.min = { center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z };
	aabb.max = { center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z };

	return aabb;
}
void player::SetPosition(const Vector3& pos)
{
	worldTransform_.translation_ = pos;
}

Vector3 player::GetPosition() const
{
	return worldTransform_.translation_;
}

void player::Update()
{
	// 移動入力(02_07 スライド10枚目)
	InputMove();
	Blink();

	// 衝突情報を初期化(02_07 スライド13枚目)
	CollisionMapInfo collisionMapInfo = {};
	collisionMapInfo.move = velocity_;
	collisionMapInfo.landing = false;
	collisionMapInfo.hitWall = false;

	// マップ衝突チェック(02_07 スライド13枚目)
	CheckMapCollision(collisionMapInfo);

	// 移動(02_07 スライド36枚目)
	worldTransform_.translation_ += collisionMapInfo.move;

	// 天井接触による落下開始(02_07 スライド38枚目)
	if (collisionMapInfo.ceiling) 
	{
		velocity_.y = 0;
	}

	// 02_08 スライド27枚目 壁接触している場合の処理
	UpdateOnWall(collisionMapInfo);

	// 接地判定
	UpdateOnGround(collisionMapInfo);


	//移動
	bool landing = false;

	// 下降あり？
	if (velocity_.y < 0) 
	{
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) 
		{
			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {

		// 上方向へ動いたら空中へ
		if (velocity_.y > 0.0f) {

			onGround_ = false;
		}

	} else {

		// 着地
		if (landing) {

			// 地面にぴったり合わせる
			worldTransform_.translation_.y += 0.1f;


			// 落下停止
			velocity_.y = 0.0f;

			// 横移動減衰
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.z *= (1.0f - kAttenuation);

			onGround_ = true;
		}
	}

	// 旋回制御
	if (turnTimer_ > 0.0f) 
	{
		// タイマーを進める
		turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

		float destinationRotationYTable[] = { std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> *3.0f / 2.0f };

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}

	// ワールド行列更新（アフィン変換～DirectXに転送）
	upDate_->WorldTransformUpData(worldTransform_);

	if (blinkCooldownTimer_ > 0.0f) 
	{

		blinkCooldownTimer_ -= 1.0f / 60.0f;

		if (blinkCooldownTimer_ < 0.0f) 
		{
			blinkCooldownTimer_ = 0.0f;
		}
	}
	if (isBlink_) 
	{

		blinkTimer_ += 1.0f / 60.0f;

		float t = blinkTimer_ / blinkDuration_;

		if (t >= 1.0f) 
		{
			t = 1.0f;
			isBlink_ = false;
		}

		// イージング
		float easedT = EaseOut(0.0f, 1.0f, t);

		// 座標補間
		worldTransform_.translation_.x =
			blinkStartPos_.x +
			(blinkEndPos_.x - blinkStartPos_.x) * easedT;

		worldTransform_.translation_.y =
			blinkStartPos_.y +
			(blinkEndPos_.y - blinkStartPos_.y) * easedT;

		worldTransform_.translation_.z =
			blinkStartPos_.z +
			(blinkEndPos_.z - blinkStartPos_.z) * easedT;

		// 回転
		worldTransform_.rotation_.y += 0.5f;
	}
}

void player::Draw()
{
	model_->Draw(worldTransform_, *camera_);
}


