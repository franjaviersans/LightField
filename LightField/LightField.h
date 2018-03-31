#ifndef LIGHTFIELD_H
#define LIGHTFIELD_H

#ifdef WIN32
#pragma warning( disable : 4290 )
#pragma warning(disable:4996)
#endif


#include "Definitions.h"
#include "TextureManager.h"
#include <string>
#include <filesystem>
#include <vector>

using namespace std::experimental::filesystem::v1;

using std::string;
using std::vector;

struct imageInfo{
	string path;
	int indexx, indexy;
	double posx, posy;
};

class LightField
{
private:
	string mPath;
	vector<imageInfo> mImageInfo;
	int mWidth, mHeight;

public:
	LightField() : mPath("") {};
	~LightField();

	void setPath(string p) { mPath = p; }
	bool checkDirectory();
	bool readLightField();
	void useLightField();
};

#endif // GLSLPROGRAM_H