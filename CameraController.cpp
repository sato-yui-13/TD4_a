#define NOMINMAX
#include "CameraController.h"
#include "MyMath.h"
#include "Player.h"
#include <algorithm>

using namespace MathUtility;

void CameraController::Initialize() 
{ 
	camera_.Initialize();

	// 見下ろしカメラ用オフセット
	targetOffset_ = { 0.0f, 18.0f, -22.0f };

	// 下を見る角度
	camera_.rotation_ = { 0.65f, 0.0f, 0.0f };


	camera_.farZ = 1000.0f;
	camera_.UpdateMatrix();
}

void CameraController::Update()
{
	//追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	//追従対象とオフセットからカメラの座標を計算
	targetPosition_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	targetPosition_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	targetPosition_.z = targetWorldTransform.translation_.z + targetOffset_.z;

	//座標補間によるゆったり追従
	camera_.translation_ = Lerp(camera_.translation_, targetPosition_, kInterpolationRate);

	//------------------------------------------後に修正------------------------------------------/
	
	// 移動範囲制限
	/*camera_.translation_.x = std::max(camera_.translation_.x, movebleArea_.left);
	camera_.translation_.x = std::min(camera_.translation_.x, movebleArea_.right);
	camera_.translation_.y = std::min(camera_.translation_.y, movebleArea_.bottom);
	camera_.translation_.y = std::max(camera_.translation_.y, movebleArea_.top);*/
	//camera_.translation_.z = std::max(camera_.translation_.z, movebleArea_.top);
	//camera_.translation_.z = std::min(camera_.translation_.z, movebleArea_.bottom);

	//行列を更新
	camera_.UpdateMatrix();
}

void CameraController::Reset() 
{
	//追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	//追従対象とオフセットからカメラの座標を計算
	camera_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;

	camera_.rotation_ = { 0.65f, 0.0f, 0.0f };

	camera_.UpdateMatrix();

}