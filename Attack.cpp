#include "Attack.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include "KamataEngine.h"


void Attack::Initialize(Model* model, Camera* camera, const Vector3& position) {

	assert(model);

	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// 向き
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	modelAction = Model::CreateFromOBJ("action", true);


	worldTransform_.TransferMatrix();
	//攻撃
	worldTransformAction.Initialize();

	worldTransformAction.TransferMatrix();

	mapChipField_ = new MapChipField();

}
void Attack::SetPlayerPosition(const Vector3& pos) {
	playerPosition_ = pos;
}


//攻撃
void Attack::Action()
{
	//AAの攻撃にしよ
	//playerの位置に
	// Jキーで攻撃
	if (Input::GetInstance()->TriggerKey(DIK_J)) {
		// プレイヤー位置から発射
		positionAction = playerPosition_; // ←ここ重要
		isAction = true;

	}
	if (isAction) {

		positionAction.x += actionSpeed;
	}

	if (!CheckMapChipCollisionRight()) {

		// 右側がブロックに当たった
		//isAction = false;
		positionAction.x += actionSpeed;
		worldTransform_.translation_.y += 1.0f;
	}

	//Transformに反映
	worldTransformAction.translation_ = positionAction;
	worldTransformAction.TransferMatrix();

}


AABB Attack::GetAABB() const
{
	Vector3 p = worldTransform_.translation_;

	AABB aabb;

	aabb.min = {
		p.x - size_.x,
		p.y - size_.y,
		p.z - size_.z
	};

	aabb.max = {
		p.x + size_.x,
		p.y + size_.y,
		p.z + size_.z
	};

	return aabb;
}

bool Attack::CheckMapChipCollisionRight()
{ // 右側の座標
	Vector3 rightPos = worldTransform_.translation_;

	rightPos.x += kWidth / 2.0f;

	MapChipField::IndexSet indexSet;

	indexSet =
		mapChipField_->GetMapChipIndexSetByPosition(rightPos);

	// 追加
	assert(indexSet.xIndex >= 0);
	assert(indexSet.yIndex >= 0);

	MapChipType mapChipType =
		mapChipField_->GetMapChipTypeByIndex(
			indexSet.xIndex,
			indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {

		if (isAction) {

			return true;
		}
	}

	return false;
}



void Attack::Update() {
	//単発の攻撃
	Action();

	CheckMapChipCollisionRight();

	// 行列更新
	worldTransform_.TransferMatrix();
	worldTransformAction.TransferMatrix();
	//弾
	upDate_->WorldTransformUpData(worldTransformAction);

}

void Attack::Draw() {

	//Jキーを押したら表示　xの値で消す
	if (isAction) {
		modelAction->Draw(worldTransformAction, *camera_);
	}

}
