#include "Attack.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include "KamataEngine.h"
#include <iostream>


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
	if (Input::GetInstance()->TriggerKey(DIK_J))
	{
		isAttacking_ = true;
		attackTimer_ = attackDuration_;
	}

	//攻撃の時間
	if (isAttacking_)
	{
		attackTimer_ -= 1.0f / 60.0f;

		if (attackTimer_ <= 0.0f)
		{
			isAttacking_ = false;
		}
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




void Attack::Update() {
	//単発の攻撃
	Action();



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
