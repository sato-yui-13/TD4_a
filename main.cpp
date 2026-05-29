#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"

using namespace KamataEngine;

GameScene* gameScene = nullptr;


//シーン切り替え
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
	kClear,
	kOver,
};
// 現在シーン（型）
Scene scene = Scene::kTitle;
//ゲームシーンのインスタンス生成
// 02_12 29枚目
void ChangeScene() {
	
	
	switch (scene) {

	case Scene::kTitle:
		if (Input::GetInstance()->PushKey(DIK_L)) {
			scene = Scene::kGame;

			if (!gameScene) {
				gameScene = new GameScene;
				gameScene->Initialize();
			}
		}
		break;

	case Scene::kGame:
			if (gameScene->clearFlag == 1) {
				scene = Scene::kClear;
				gameScene = new GameScene;
				gameScene->Initialize();

			}
		
		break;

	case Scene::kClear:

	

		if (Input::GetInstance()->PushKey(DIK_L)) {
			scene = Scene::kTitle;
		}

		break;



	case Scene::kOver:
		break;
	}
}

void UpDataScene() {
	//
	switch (scene) {
	case Scene::kTitle:
		//		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kClear:
		break;
	case Scene::kOver:
		break;
	}
}
//
// 02_12 32枚目
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		//		titleScene->Draw();
		break;
	case Scene::kGame:
			gameScene->Draw();
		break;
	case Scene::kClear:
		//
		break;
	case Scene::kOver:
		//
		break;
	}
}


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{

	KamataEngine::Initialize();

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();


	//ゲームシーンの初期化
	//gameScene->Initialize();


	// メインループ
	while (true)
	{
		// エンジンの更新
		if (KamataEngine::Update())
		{
			break;
		}
		ChangeScene();
		UpDataScene();
		DrawScene();

		// 描画更新
		dxCommon->PreDraw();

		//ゲームシーンの描画
		//gameScene->Draw();
		DrawScene();     // ★これも足す

		// 描画終了
		dxCommon->PostDraw();
	}

	//ゲームシーンの解放
	delete gameScene;
	//delete titleScene;
	//nullptrの代入
	gameScene = nullptr;

	KamataEngine::Finalize();

	return 0;
}
