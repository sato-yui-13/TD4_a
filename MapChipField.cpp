#include "MapChipField.h"
#include <cassert>
#include <map>
#include <fstream>
#include <sstream>
#include <string>

using namespace KamataEngine;

//マップチップテーブル
namespace
{
	std::map<std::string, MapChipType> mapChipTable =
	{
		{"0",MapChipType::kBlank},
		{"1",MapChipType::kBlock},
		{"2", MapChipType::kWall},
	};
}

void MapChipField::ResetMapChipData()
{
	//マップチップデータをリセット
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data)
	{
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath)
{
	//マップチップデータをリセット
	ResetMapChipData();

	//ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	//マップチップCSV
	std::stringstream mapChipCsv;
	//ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();
	//ファイルを閉じる
	file.close();

	//CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical; i++)
	{
		std::string line;
		getline(mapChipCsv, line);

		//1行目の文字列をストリームに変換して解析しやすくなる
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; j++)
		{
			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word))
			{
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

//縦横のインデックスを指定してその位置のマップチップ種別を取得する関数。
MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex)
{
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex)
	{
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex)
	{
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];

}

//マップチップのワールド座標を取得する
KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex)
{
	return Vector3(
		kBlockWidth * xIndex,
		0.0f,
		kBlockWidth * yIndex
	);
}

KamataEngine::Vector3 MapChipField::GetCenterPosition()
{
	float centerX = kNumBlockHorizontal * kBlockWidth * 0.5f;
	float centerZ = kNumBlockVirtical * kBlockWidth * 0.5f;

	return { centerX, 0.0f, centerZ };

}

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position)
{
	IndexSet indexSet = {};

	indexSet.xIndex = static_cast<uint32_t>(
		(position.x + kBlockWidth / 2.0f) / kBlockWidth
		);

	indexSet.yIndex = static_cast<uint32_t>(
		(position.z + kBlockWidth / 2.0f) / kBlockWidth
		);

	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex)
{
	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect{};
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.z - kBlockWidth / 2.0f;
	rect.top = center.z + kBlockWidth / 2.0f;

	return rect;
}

