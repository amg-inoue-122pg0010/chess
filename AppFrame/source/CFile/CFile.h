#pragma once

class CFile
{
public:
	CFile(const char *filename);
	~CFile();
	char *filename() { return _filename; }
	void *data() { return _data; }
	int size() { return _size; }

private:
	char	_filename[320];

	int		_size;
	char	*_data;
};
