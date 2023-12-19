#include <stdio.h>
#include <string.h>
#include "CFile.h"
#pragma warning(disable : 4996)

#define	SAFE_DELETE(p)	if(p){delete p;p=NULL;}

CFile::CFile(const char *filename) {
	// メンバの初期化
	_size = 0;
	_data = NULL;
	strcpy(_filename, filename);

	// ファイルを開いて読み込む
	FILE *fp;
	fp = fopen(filename, "rb");
	if (!fp) {
		// 開くの失敗
		return;
	}
	// ファイルサイズの確認
	fseek(fp, 0, SEEK_END);
	_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	// メモリを確保してロード
	_data = new char[_size];
	fread(_data, 1, _size, fp);
	// ファイルを閉じる
	fclose(fp);
}

CFile::~CFile() {
	SAFE_DELETE(_data);
}
