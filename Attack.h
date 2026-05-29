#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "UpDate.h"
#include "MyMath.h"
#include "boss.h"
#include "player.h"

class Attack
{
public:
	/// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);
	/// 更新
	void Update();
	/// 描画
	void Draw();
	//
	void SetPlayerPosition(const Vector3& pos);
	Vector3 playerPosition_;
	//攻撃
	void Action();

	//弾が出てるときだけ動く
	int isAction = false;

	
	//プレイヤーの攻撃の当たり判定
	//bool IsCollision(const AABB& a, const AABB& b);
	//攻撃した時の当たり判定
	AABB GetAABB() const;

	// これが IsAttacking()。外から状態を確認するための関数
	bool IsAttacking() const { return isAttacking_; }

private:

	// ワールド変換データ
	WorldTransform worldTransform_;

	//マップチップフィールド
	MapChipField* mapChipField_;


	// モデル
	Model* model_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	//アップデート
	UpDate* upDate_;

	//速度
	Vector3 velocity_ = {};
	//============= 攻撃変数 ============\\
	//位置
	Vector3 positionAction = {};

	//スピード
	static inline const float ActionSpeed = 0.5f;

	//ワールド
	WorldTransform worldTransformAction;

	//モデル
	Model* modelAction = nullptr;

	//フラグで攻撃のイラスト表示
	bool isAttacking_ = false;
	//攻撃のCD
	float attackTimer_ = 0.0f;
	const float attackDuration_ = 0.2f; // 攻撃が出てる時間

	//スピード
	int actionSpeed = 1;

	//AA
	Vector3 size_ = { 1.0f, 1.0f, 1.0f };
	static inline const float kWidth = 2.0f;







	//近距離
	//連続(AS上げる)
	//単発()

	//遠距離
	//連続
	//単発(詠唱)
};

