#pragma once
#include	<unordered_map>
#include <string>
// リソース管理サーバ
// すべて静的メンバで構成する

struct MV1Model {
	std::string _filename;
	int _handle;
};


class ResourceServer {
public:
	static	void	Init();
	static	void	Release();

	static	void	ClearGraph();
	static	int		LoadGraph(const TCHAR* FileName);
	static int		LoadDivGraph(const TCHAR* FileName, int AllNum,
		int XNum, int YNum,
		int XSize, int YSize, int* HandleBuf);

	static	int		LoadSoundMem(const TCHAR* FileName);

	//MV1モデルを一括読み込みする
	static int MV1LoadModel(const TCHAR* FileName);
	using MV1ModelMap = std::unordered_map<std::string, MV1Model>;
	static void LoadModels(const MV1ModelMap& mv1modelmap);
	static int GetHandles(const std::string& key);
	static MV1ModelMap _mv1Models;

	

private:
	static std::unordered_map<std::string, int>	_mapGraph;
	typedef struct {
		int		AllNum;
		int*	handle;
	} DIVGRAPH;
	static std::unordered_map<std::string, DIVGRAPH>	_mapDivGraph;

	static std::unordered_map<std::string, int>	_mapSound;
	static std::unordered_map<std::string, int> _mapMV1Model;
	static std::unordered_map<std::string, int> _mapIntData;
};

