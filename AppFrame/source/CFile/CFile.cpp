#include <stdio.h>
#include <string.h>
#include "CFile.h"
#pragma warning(disable : 4996)

#define	SAFE_DELETE(p)	if(p){delete p;p=NULL;}

CFile::CFile(const char *filename) {
	// �����o�̏�����
	_size = 0;
	_data = NULL;
	strcpy(_filename, filename);

	// �t�@�C�����J���ēǂݍ���
	FILE *fp;
	fp = fopen(filename, "rb");
	if (!fp) {
		// �J���̎��s
		return;
	}
	// �t�@�C���T�C�Y�̊m�F
	fseek(fp, 0, SEEK_END);
	_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	// ���������m�ۂ��ă��[�h
	_data = new char[_size];
	fread(_data, 1, _size, fp);
	// �t�@�C�������
	fclose(fp);
}

CFile::~CFile() {
	SAFE_DELETE(_data);
}
