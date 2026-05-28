#pragma once
#include "KamataEngine.h"
#include "UpDate.h"
#include "MyMath.h"
using namespace KamataEngine;

class boss
{
public:
	/// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	/// 更新
	void Update();

	/// 描画
	void Draw();

	// 02_10 10枚目 ワールド座標を取得
	Vector3 GetWorldPosition();

	//bossの攻撃の初期化、更新、描画
	void atInitialize(Model* model, Camera* camera, const Vector3& position);
	void atUpdate();
	void atDraw();

	//ボスポジション
	Vector3 bossPosition_ = { 0.0f,1.0f,0.0f };
	//アタックポジション
	Vector3 attackPosition_ = { 0.0f,1.0f,1.0f };

private:
	//上下に動く敵をつくる

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;
	//アップデート
	UpDate* upDate_;

	//位置
	Vector3 positionBoss = {};

	//スピード
	float BossSpeed = 5.5f;

	//跳ね返る位置
	//上の最大sa値
	float min = 30.0f;

	//下の最大値
	float max = 650.0f;

	//移動スピード
	Vector3 moveSpeed_ = { 0.01f,0.01f,0.01f };

	// アタックワールドトランスフォーム
	WorldTransform atWorldTransform_;
	// アタックモデル
	Model* atModel_ = nullptr;
	// アタックカメラ
	Camera* atCamera_ = nullptr;
	// アタック中の有無
	bool isAttacking_ = false;

	//アタックスピード
	float attackSpeed_ = 1.0f / 60.0f;
	//アタックの角度
	float attackAngle_ = 0.0f;
	//アタックの半径
	float attackRadius_ = 1.0f;
	float frameCount_ = 0.0f; // 現在の経過時間（フレーム数）
	float maxFrame_ = 90.0f;  // 1周にかける時間（90フレーム = 1.5秒。数値を小さくすると速くなります）
	bool isDead_ = false;     // 1周したらtrueになるフラグ
	// 移動変数
	float moveTimer_ = 0.0f;     // 時間を計るタイマー
	float moveInterval_ = 180.0f; // 次の目標を決める間隔（180フレーム = 3秒）
	Vector3 bossTargetPosition_; // ★新設：次に目指すランダムな目標座標
	float bossMoveSpeed_ = 0.1f; // ★新設：じわじわ動くスピード（お好みで調整）
	const float kBlockSize = 1.0f; // 1マスのサイズ（環境に合わせて調整）
	int bossState_ = 0;        // ★新設：0=移動中、1=攻撃中
	float attackWaitTimer_ = 0.0f; // ★新設：攻撃が終わったあとの余韻（タイマー）
	int attackPattern_ = 0;       // ★新設：0=回転攻撃、1=直線射撃攻撃
	Vector3 bulletVelocity_;       // ★新設：直線射撃の進む速度ベクトル
	// --- Boss.h の private 変数群に追記 ---
	float attackStartRotationY_ = 0.0f; // ★新設：攻撃開始時のボスの向きを記録する変数

};
