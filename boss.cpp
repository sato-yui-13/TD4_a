#include "boss.h"
#include <cassert>
#include <numbers>
#include <cstdio>
#include <random>
#include <cmath>

using namespace KamataEngine;

void boss::Initialize(Model* model, Camera* camera, const Vector3& position)
{
    assert(model);

    model_ = model;
    camera_ = camera;

    worldTransform_.Initialize();
    worldTransform_.translation_ = position;
    // 向き
    worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	atWorldTransform_.Initialize();
	atWorldTransform_.translation_ = position;
	atWorldTransform_.rotation_ = { 0.0f, 0.0f, 0.0f };
	atWorldTransform_.scale_ = { 1.0f, 1.0f, 1.0f };

	bossPosition_ = position;
	bossTargetPosition_ = position;
	attackPosition_ = position;


    // ★ 生成位置を中心に上下移動する範囲を設定
    float moveRange = 2.0f;
    min = position.y - moveRange;
    max = position.y + moveRange;

    worldTransform_.TransferMatrix();
}

Vector3 boss::GetWorldPosition()
{
    Vector3 worldPos{};
    worldPos.x = worldTransform_.matWorld_.m[3][0];
    worldPos.y = worldTransform_.matWorld_.m[3][1];
    worldPos.z = worldTransform_.matWorld_.m[3][2];
    return worldPos;
}

void WorldTransformUpdate(WorldTransform& worldTransform) 
{

	Matrix4x4 affin_mat = MakeAffineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);

	worldTransform.matWorld_ = affin_mat;

	// 定数バッファに転送する
	worldTransform.TransferMatrix();
}
AABB boss::GetAABB() const
{
	// ボスの中心座標
	Vector3 center = worldTransform_.translation_;

	// ボスの当たり判定サイズ（プレイヤーより大きめに設定）
	Vector3 halfSize = { 1.0f, 2.0f, 1.0f };

	AABB aabb;
	aabb.min = { center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z };
	aabb.max = { center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z };

	return aabb;
}

void boss::SetWorldPosition(const Vector3& pos)
{
	worldTransform_.translation_ = pos;
}

void boss::Update()
{

	// ─── 【デバッグ用】ターゲットの座標を左右に往復移動させる ───
	static float testTimer_ = 0.0f;
	testTimer_ += 0.03f;
	float playerX = 4.5f + std::sinf(testTimer_) * 3.0f;
	float playerZ = 4.5f;

	// =======================================================
		// パターン0：【移動中】の処理
		// =======================================================
	if (bossState_ == 0) {

		moveTimer_ += 1.0f;
		if (moveTimer_ >= moveInterval_) {
			moveTimer_ = 0.0f;

			std::random_device seed_gen;
			std::mt19937 engine(seed_gen());
			std::uniform_int_distribution<int> dist(2, 8);

			bossTargetPosition_.x = static_cast<float>(dist(engine)) * kBlockSize;
			bossTargetPosition_.z = static_cast<float>(dist(engine)) * kBlockSize;
		}

		// 目的地までの残りの距離（ベクトル）
		float toTargetX = bossTargetPosition_.x - bossPosition_.x;
		float toTargetZ = bossTargetPosition_.z - bossPosition_.z;

		// 完全に到着したかを判定するための直線距離
		float distance = std::sqrtf(toTargetX * toTargetX + toTargetZ * toTargetZ);

		// ─── ★【ここをステップ移動に修正】★ ───
		// 目的地から「0.05（5cm）」以上離れている間は、残りの距離の 8% ずつ進む
		if (distance > 0.05f) {
			// 0.08f (8%) の数値を大きくするとより鋭いダッシュになり、小さくするとマイルドになります
			float easeSpeed = 0.08f;
			bossPosition_.x += toTargetX * easeSpeed;
			bossPosition_.z += toTargetZ * easeSpeed;
		} else {
			// ── 目的地にほぼ到着した瞬間 ──
			// ズレをなくすためにピタッと座標を合わせる
			bossPosition_.x = bossTargetPosition_.x;
			bossPosition_.z = bossTargetPosition_.z;

			bossState_ = 1;      // 攻撃中モードへ
			isAttacking_ = true; // 攻撃中フラグON
			frameCount_ = 0.0f;  // タイマーリセット
			attackAngle_ = 0.0f;

			attackPosition_ = bossPosition_;

			// 到着した瞬間（発射時）のボスの向きを記録
			attackStartRotationY_ = worldTransform_.rotation_.y;

			// ── プレイヤーとの直線距離を計算 ──
			float toPlayerX = playerX - bossPosition_.x;
			float toPlayerZ = playerZ - bossPosition_.z;
			float distToPlayer = std::sqrtf(toPlayerX * toPlayerX + toPlayerZ * toPlayerZ);

			// ── 距離に応じて攻撃パターンを切り替える ──
			if (distToPlayer <= 2.0f) {
				attackPattern_ = 0; // 2マス以内なら回転攻撃
			} else {
				attackPattern_ = 1; // 2マスより離れているなら直線射撃

				// 射撃の速度ベクトルを計算
				float shootSpeed = 0.2f;
				bulletVelocity_.x = std::sinf(attackStartRotationY_) * shootSpeed;
				bulletVelocity_.z = std::cosf(attackStartRotationY_) * shootSpeed;
			}
		}

		attackPosition_ = bossPosition_;
	}
	// =======================================================
	// パターン1：【攻撃中（その場で停止）】の処理
	// =======================================================
	else if (bossState_ == 1) {

		if (isAttacking_) {

			// ─── 【攻撃パターン0：回転攻撃】 ───
			if (attackPattern_ == 0) {
				frameCount_ += 1.0f;
				float t = frameCount_ / maxFrame_;

				if (t >= 1.0f) {
					isAttacking_ = false;
					attackWaitTimer_ = 0.0f;
				} else {
					float easedT = t * t;

					// 円を描くための基本角度 (0 〜 -360度)
					float circleAngle = -(easedT * (3.141592f * 2.0f));

					// ★【修正】毎フレーム動く worldTransform_ ではなく、
					// 発射時にロックした attackStartRotationY_ を基準にする！
					float startAngle = attackStartRotationY_ + 3.141592f;

					// 起点に円の角度を足す
					attackAngle_ = startAngle + circleAngle;

					// 回転の中心はボスの位置
					attackPosition_.x = bossPosition_.x + std::sinf(attackAngle_) * attackRadius_;
					attackPosition_.z = bossPosition_.z + std::cosf(attackAngle_) * attackRadius_;
				}
			}
			// ─── 【攻撃パターン1：直線射撃攻撃】 ───
			else if (attackPattern_ == 1) {
				// 速度ベクトル（これもロック時の角度で計算済み）を足し続ける
				attackPosition_.x += bulletVelocity_.x;
				attackPosition_.z += bulletVelocity_.z;

				frameCount_ += 1.0f;
				if (frameCount_ >= 60.0f) {
					isAttacking_ = false;
					attackWaitTimer_ = 0.0f;
				}
			}
		} else {
			// ─── 待機時間（1.5秒に延長中） ───
			attackPosition_ = bossPosition_;

			attackWaitTimer_ += 1.0f;
			if (attackWaitTimer_ >= 90.0f) {
				bossState_ = 0;
				moveTimer_ = 0.0f;

				std::random_device seed_gen;
				std::mt19937 engine(seed_gen());
				std::uniform_int_distribution<int> dist(2, 8);
				bossTargetPosition_.x = static_cast<float>(dist(engine)) * kBlockSize;
				bossTargetPosition_.z = static_cast<float>(dist(engine)) * kBlockSize;
			}
		}
	}

	// ─── 常にターゲット（プレイヤー）の方向を向かせる（ボス本体のみが滑らかに動く） ───
	float directionX = playerX - bossPosition_.x;
	float directionZ = playerZ - bossPosition_.z;
	worldTransform_.rotation_.y = std::atan2f(directionX, directionZ);

	// =======================================================
	// ③ 確定した座標をトランスフォームに適用して行列更新
	// =======================================================
	worldTransform_.translation_.x = bossPosition_.x;
	worldTransform_.translation_.z = bossPosition_.z;
	WorldTransformUpdate(worldTransform_);

	// 攻撃（at変数）の更新（ボス本体の rotation_ から完全に独立しました！）
	atWorldTransform_.translation_ = attackPosition_;
	WorldTransformUpdate(atWorldTransform_);


    //// 行列更新
    //worldTransform_.TransferMatrix();
}

void boss::Draw()
{

	float alpha = 0.5f;
	alpha -= 0.1f / 60.0f;
	model_->SetAlpha(alpha);

    model_->Draw(worldTransform_, *camera_);
}