#pragma once
#include "KamataEngine.h"
#include "MapChipField.h"
#include "UpDate.h"
#include "MyMath.h"
#include "Attack.h"

class player
{
public:

	// 左右
	enum class LRDirection 
	{
		kRight,
		kLeft,
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="camera">カメラ</param>
	/// <param name="position"></param>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//ボスとのボックス当たり判定
	AABB GetAABB() const;
	// 02_10 10枚目 ワールド座標を取得
	Vector3 GetWorldPosition();

	// 02_07 スライド4枚目
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	//ha
	void SetPosition(const Vector3& pos);
	Vector3 GetPosition() const;


	struct CollisionMapInfo 
	{
		bool ceiling = false;
		bool landing = false;
		bool hitWall = false;
		Vector3 move;
	};

	// 02_07 スライド13枚目
	void CheckMapCollision(CollisionMapInfo& info);

	// 02_07 スライド14枚目
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	void CheckMapCollisionFront(CollisionMapInfo& info);
	void CheckMapCollisionBack(CollisionMapInfo& info);

	// 02_08 スライド27枚目 壁接触している場合の処理
	void UpdateOnWall(const CollisionMapInfo& info);

	// 02_08スライド14枚目 設置状態の切り替え処理
	void UpdateOnGround(const CollisionMapInfo& info);

	// 角 02_07スライド16枚目
	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };
	// 02_07 スライド17枚目
	Vector3 CornerPosition(const Vector3& center, Corner corner);

	//攻撃
	void Action();

	// getter
	const KamataEngine::WorldTransform& GetWorldTransform() const
	{
		return worldTransform_;
	}

	bool IsDead() const { return isDead_; }
	void SetWorldPosition(const Vector3& pos);
private:

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	//カメラ
	KamataEngine::Camera* camera_ = nullptr;

	//速度
	Vector3 velocity_ = {};

	//アップデート
	UpDate* upDate_;

	//ブリンク
	void Blink();

	// 02_07スライド10枚目 移動入力
	void InputMove();

	// 02_07 マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;


	// 02_05 顔の向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// 02_05 旋回タイマー
	float turnTimer_ = 0.0f;

	// 02_05 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;

	// 02_05 旋回時間 <秒>
	static inline const float kTimeTurn = 0.3f;

	// 02_05 最高速度
	static inline const float kLimitRunSpeed = 0.3f;
	// 02_05 非入力時の摩擦係数
	static inline const float kAttenuation = 0.05f;
	// 02_05 着地フラグ
	bool onGround_ = true;

	// 02_05 ジャンプ定数もろもろ
	static inline const float kJumpAcceleration = 6.15f;;
	static inline const float kGravityAcceleration = 1.98f;
	static inline const float kLimitFallSpeed = 0.5f;

	// 02_08スライド16枚目 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.0f;
	// 02_08スライド21枚目 微小な数値
	static inline const float kGroundSearchHeight = 0.06f;

	// 02_07 キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	// 02_07スライド34枚目
	static inline const float kBlank = 0.005f;
	// 02_08スライド27枚目 着地時の速度減衰率
	static inline const float kAttenuationWall = 0.2f;

	//　Attack* attack_;

	bool isBlink_ = false;

	float blinkTimer_ = 0.0f;
	float blinkDuration_ = 0.2f;

	Vector3 blinkStartPos_;
	Vector3 blinkEndPos_;

	//ブリンクCD
	float blinkCooldownTimer_ = 0.0f;
	const float blinkCooldown_ = 0.5f;

	//デスフラグ
	bool isDead_ = false;

	static inline const float kDepth = 1.0f;
	const float kBlinkDistance = 8.0f;

	Vector3 forward_ = { 0.0f, 0.0f, 1.0f };
	float blinkStartRotY_ = 0.0f;
	float blinkTargetRotY_ = 0.0f;
	//const float kBlinkDistance = 8.0f;

};