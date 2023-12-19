#include "utf8sjis.h"

// UTF8<->SJIS相互変換
static int(*s_pfunc_SJIStoUTF8)(char *pSJIS_Single, int nInSize, char *pUTF8_Buffer) = NULL;
static int(*s_pfunc_UTF8toSJIS)(char *pUTF8_Single, int nInSize, char *pSJIS_Buffer) = NULL;

bool SJIS_IsKanji(unsigned char cData) {
	if (cData<0x81) { return false; }
	if (cData<0xa0) { return true; }
	if (cData<0xe0) { return false; }
	if (cData<0xff) { return true; }
	return false;
}


// UTF8の先頭文字を見て、何バイト文字か返す
int UTF8_OctBytes(unsigned char c) {
	/*
	一次情報としてはUnicodeフォーラムの仕様書か、RFC3629を参照
	0*******: 1オクテット文字
	10******: 複数オクテット文字の後続部分
	110*****: 2オクテット文字の先頭
	1110****: 3オクテット文字の先頭
	11110***: 4オクテット文字の先頭
	111110**: 5オクテット文字の先頭
	1111110*: 6オクテット文字の先頭
	*/
	if ((c & 0x80) == 0x00) { return 1; }	// 0*** ****
	if ((c & 0xe0) == 0xc0) { return 2; }	// 110* ****
	if ((c & 0xf0) == 0xe0) { return 3; }	// 1110 ****
	if ((c & 0xf8) == 0xf0) { return 4; }	// 1111 0***
	if ((c & 0xfc) == 0xf8) { return 5; }	// 1111 10**
	if ((c & 0xfe) == 0xfc) { return 6; }	// 1111 110*

											// 1文字目じゃない
	return 0;
}


void LibTxt_SetFunc_SJIStoUTF8(int(*funcSJIStoUTF8)(char *pSJIS_Single, int nInSize, char *pUTF8_Buffer)) {
	s_pfunc_SJIStoUTF8 = funcSJIStoUTF8;
}

void LibTxt_SetFunc_UTF8toSJIS(int(*funcUTF8toSJIS)(char *pUTF8_Single, int nInSize, char *pSJIS_Buffer)) {
	s_pfunc_UTF8toSJIS = funcUTF8toSJIS;
}

static unsigned long ConvSJIStoUTF8Single(unsigned long lSjis) {
	size_t in_size = 1;
	size_t out_size;
	unsigned long x = lSjis;
	unsigned char szSjis[8] = { 0 };
	if (x < 0x100) {
		// 1byte
		szSjis[0] = (unsigned char)(x & 0xff);
		in_size = 2;
	}
	else if (x < 0x10000) {
		// 2byte
		szSjis[0] = (unsigned char)((x >> 8) & 0xff);
		szSjis[1] = (unsigned char)((x) & 0xff);
		in_size = 3;
	}
	else if (x < 0x1000000) {
		// 3byte
		szSjis[0] = (unsigned char)((x >> 16) & 0xff);
		szSjis[1] = (unsigned char)((x >> 8) & 0xff);
		szSjis[2] = (unsigned char)((x) & 0xff);
		in_size = 4;
	}
	else {
		// 4byte
		szSjis[0] = (unsigned char)((x >> 24) & 0xff);
		szSjis[1] = (unsigned char)((x >> 16) & 0xff);
		szSjis[2] = (unsigned char)((x >> 8) & 0xff);
		szSjis[3] = (unsigned char)((x) & 0xff);
		in_size = 5;
	}

	// Shift-JISからUTF-8へ変換
	unsigned char buffUtf8[16] = "";
	if (s_pfunc_SJIStoUTF8) {
		out_size = (*s_pfunc_SJIStoUTF8)((char*)szSjis, in_size, (char*)buffUtf8);
	}
	else {
		if (in_size == 2) { buffUtf8[0] = szSjis[0]; buffUtf8[1] = '\0'; }

#ifdef ANDROID
#ifdef USE_ICONV
#define	MAX_BUF 1024
		char	inbuf[MAX_BUF + 1] = { 0 };
		char	outbuf[MAX_BUF + 1] = { 0 };
		char	*in = inbuf;
		char	*out = outbuf;
		size_t	in_size = (size_t)MAX_BUF;
		size_t	out_size = (size_t)MAX_BUF;
		iconv_t ic = iconv_open("UTF-8", "SJIS");	// sjis to utf8

		strcpy(in, (char*)szSjis);
		iconv(ic, &in, &in_size, &out, &out_size);
		iconv_close(ic);
		strcpy((char*)buffUtf8, outbuf);
#endif
#endif	// ANDROID
#ifdef WIN32
		// ShiftJISからUTF-16へ変換
		BYTE buffUtf16[16];
		MultiByteToWideChar(932 /*SJIS,  CP_ACP*/, 0, (LPCSTR)szSjis, -1, (LPWSTR)buffUtf16, 16);
		// UTF-16からUTF-8へ変換
		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)buffUtf8, 16, NULL, NULL);
#endif // WIN32
	}

	x = 0;
	int c = 0;
	while (buffUtf8[c] != '\0') {
		x = (x << 8) | ((unsigned long)buffUtf8[c++]);
	}

	return x;
}

int ConvSJIStoUTF8(const unsigned char *pszSjis, unsigned char *pszOutUtf8) {
	int n = 0, c = 0;
	unsigned long l, x;

	while (pszSjis[n] != '\0') {

		if (SJIS_IsKanji(pszSjis[n])) {
			// 2byte
			l = (((unsigned long)pszSjis[n]) << 8) | (unsigned long)pszSjis[n + 1];
			n += 2;
		}
		else {
			// 1byte
			l = (unsigned long)pszSjis[n];
			n++;
		}
		x = ConvSJIStoUTF8Single(l);	// SJIS to UTF8

										// データ書き込み
		if (pszOutUtf8) {
			if (x < 0x100) {
				// 1byte
				pszOutUtf8[c++] = (unsigned char)(x & 0xff);
			}
			else if (x < 0x10000) {
				// 2byte
				pszOutUtf8[c++] = (unsigned char)((x >> 8) & 0xff);
				pszOutUtf8[c++] = (unsigned char)((x) & 0xff);
			}
			else if (x < 0x1000000) {
				// 3byte
				pszOutUtf8[c++] = (unsigned char)((x >> 16) & 0xff);
				pszOutUtf8[c++] = (unsigned char)((x >> 8) & 0xff);
				pszOutUtf8[c++] = (unsigned char)((x) & 0xff);
			}
			else {
				// 4byte
				pszOutUtf8[c++] = (unsigned char)((x >> 24) & 0xff);
				pszOutUtf8[c++] = (unsigned char)((x >> 16) & 0xff);
				pszOutUtf8[c++] = (unsigned char)((x >> 8) & 0xff);
				pszOutUtf8[c++] = (unsigned char)((x) & 0xff);
			}
		}
		else {
			if (x < 0x100) { c++; }
			else if (x < 0x10000) { c += 2; }
			else if (x < 0x1000000) { c += 3; }
			else { c += 4; }
		}
	}
	if (pszOutUtf8) { pszOutUtf8[c] = '\0'; }
	c++;	// asciiz用

	return c;	// 必要byte数
}

static unsigned long ConvUTF8toSJISSingle(unsigned long lUtf8) {
	size_t in_size = 1;
	size_t out_size;
	unsigned long x = lUtf8;
	unsigned char szUtf8[8] = { 0 };
	if (x < 0x100) {
		// 1byte
		szUtf8[0] = (unsigned char)(x & 0xff);
		in_size = 2;
	}
	else if (x < 0x10000) {
		// 2byte
		szUtf8[0] = (unsigned char)((x >> 8) & 0xff);
		szUtf8[1] = (unsigned char)((x) & 0xff);
		in_size = 3;
	}
	else if (x < 0x1000000) {
		// 3byte
		szUtf8[0] = (unsigned char)((x >> 16) & 0xff);
		szUtf8[1] = (unsigned char)((x >> 8) & 0xff);
		szUtf8[2] = (unsigned char)((x) & 0xff);
		in_size = 4;
	}
	else {
		// 4byte
		szUtf8[0] = (unsigned char)((x >> 24) & 0xff);
		szUtf8[1] = (unsigned char)((x >> 16) & 0xff);
		szUtf8[2] = (unsigned char)((x >> 8) & 0xff);
		szUtf8[3] = (unsigned char)((x) & 0xff);
		in_size = 5;
	}

	// UTF-8からShift-JISへ変換
	unsigned char buffSjis[16] = "";
	if (s_pfunc_SJIStoUTF8) {
		out_size = (*s_pfunc_UTF8toSJIS)((char*)szUtf8, in_size, (char*)buffSjis);
	}
	else {
		if (in_size == 2) { buffSjis[0] = szUtf8[0]; buffSjis[1] = '\0'; }

#ifdef ANDROID
#ifdef USE_ICONV
#define	MAX_BUF 1024
		char	inbuf[MAX_BUF + 1] = { 0 };
		char	outbuf[MAX_BUF + 1] = { 0 };
		char	*in = inbuf;
		char	*out = outbuf;
		size_t	in_size = (size_t)MAX_BUF;
		size_t	out_size = (size_t)MAX_BUF;
		iconv_t ic = iconv_open("SJIS", "UTF-8");	// utf8 to sjis

		strcpy(in, (char*)szUtf8);
		iconv(ic, &in, &in_size, &out, &out_size);
		iconv_close(ic);
		strcpy((char*)buffSjis, outbuf);
#endif
#endif	// ANDROID
#ifdef WIN32
		// UTF-8からUTF-16へ変換
		BYTE buffUtf16[16];
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)szUtf8, -1, (LPWSTR)buffUtf16, 16);
		// UTF-16からShift-JISへ変換
		WideCharToMultiByte(932 /*SJIS,  CP_ACP*/, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)buffSjis, 16, NULL, NULL);
#endif // WIN32
	}

	x = 0;
	int c = 0;
	while (buffSjis[c] != '\0') {
		x = (x << 8) | ((unsigned long)buffSjis[c++]);
	}

	return x;
}

int ConvUTF8toSJIS(const unsigned char *pszUtf8, unsigned char *pszOutSjis) {
	int n = 0, c = 0, len, i;
	unsigned long l, x;

	while (pszUtf8[n] != '\0') {
		len = UTF8_OctBytes(pszUtf8[n]);
		l = 0;
		for (i = 0; i < len; i++) {
			l = (l << 8) | pszUtf8[n++];
		}
		x = ConvUTF8toSJISSingle(l);	// UTF8 to SJIS

										// データ書き込み
		if (pszOutSjis) {
			if (x < 0x100) {
				// 1byte
				pszOutSjis[c++] = (unsigned char)(x & 0xff);
			}
			else {
				// 2byte
				pszOutSjis[c++] = (unsigned char)((x >> 8) & 0xff);
				pszOutSjis[c++] = (unsigned char)((x) & 0xff);
			}
		}
		else {
			if (x < 0x100) { c++; }
			else { c += 2; }
		}
	}
	if (pszOutSjis) { pszOutSjis[c] = '\0'; }
	c++;	// asciiz用

	return c;	// 必要byte数
}


std::string ConvUTF8toSJIS(std::string strUtf8) {
	int n = ConvUTF8toSJIS((const unsigned char *)strUtf8.c_str(), NULL);
	unsigned char *p = new unsigned char[n];
	ConvUTF8toSJIS((const unsigned char *)strUtf8.c_str(), p);
	std::string s =(char*)p;
	delete p;
	return s;
}

std::string ConvSJIStoUTF8(std::string strSjis) {
	int n = ConvSJIStoUTF8((const unsigned char *)strSjis.c_str(), NULL);
	unsigned char *p = new unsigned char[n];
	ConvSJIStoUTF8((const unsigned char *)strSjis.c_str(), p);
	std::string s = (char*)p;
	delete p;
	return s;
}

