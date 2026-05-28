#pragma once
#include <KamataEngine.h>

using namespace KamataEngine;

//前方宣言
class player;

/// <summary>
/// 自機カメラ制御
/// </summary>
class CameraController
{
public:

	//矩形
	struct Rect 
	{
		float left = 0.0f;//左端
		float right = 1.0f;//右端
		float bottom = 0.0f;//下端
		float top = 1.0f;//上端
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//setter
	void SetTarget(player* target) { target_ = target; }

	//最初にピッタリ追従するためのリセット関数
	void Reset();

	void SetMovebleArea(Rect area) { movebleArea_ = area; }

	const KamataEngine::Camera& GetViewProjection() const { return camera_; }

private:

	//カメラ
	KamataEngine::Camera camera_;

	//カメラの目標座標
	KamataEngine::Vector3 targetPosition_;

	//プレイヤーの座標を取得するためのポインタ変数
	player* target_ = nullptr;

	//追従対象とカメラの座標の差(オフセット)
	Vector3 targetOffset_ = { 0,0, -15.0f };

	//カメラ移動範囲
	Rect movebleArea_ = { 0,100,0,100 };

	//座標補間割合
	static inline const float kInterpolationRate = 0.1f;


};

