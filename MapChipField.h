#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include <cstdint>
#include <string>
#include <vector>

using namespace KamataEngine;

enum class MapChipType
{
    kBlank,//空白
    kBlock,//ブロック
    kWall,//壁
};
//マップチップデータ
struct MapChipData
{
    std::vector<std::vector<MapChipType>> data;
};

class MapChipField
{
public:

    struct IndexSet
    {
        uint32_t xIndex;
        uint32_t yIndex;
    };

    //範囲矩形
    struct Rect
    {
        float left;   //左端
        float right;  //右端
        float bottom; //下
        float top;    //上
    };


    //リセット
    void ResetMapChipData();
    //読み込み
    void LoadMapChipCsv(const std::string& filePath);

    //マップチップ種別の取得
    MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
    //マップチップ座標の取得
    KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

    uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }
    uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

    // ステージ中央を取得
    KamataEngine::Vector3 GetCenterPosition();

    // 02_07 スライド22枚目
    IndexSet GetMapChipIndexSetByPosition(const Vector3& position);
    // 02_07 スライド33枚目
    Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);



private:

    MapChipData mapChipData_;

    //1ブロックのサイズ
    static inline const float kBlockWidth = 1.0f;
    static inline const float kBlockHeight = 1.0f;
    //ブロックの個数
    static inline const uint32_t kNumBlockVirtical = 100;
    static inline const uint32_t kNumBlockHorizontal = 100;

};

