#include "LightField.h"
#include <stdio.h>

#include "./include/stb_image.h"


LightField::~LightField() {
	mImageInfo.clear();
}

bool LightField::checkDirectory()
{
	path p(mPath);
	

	mWidth = 0; 
	mHeight = 0;
	
	for (auto p : directory_iterator(p))
	{

		string fileName = p.path().filename().string();

		if (fileName.find(".png") != std::string::npos) {
			int row, col;
			double camx, camy;
			char ext[64];
			int ret = sscanf(fileName.c_str(),
				"out_%d_%d_%lf_%lf%s", &row, &col, &camx, &camy, ext);

			if (ret == 5) {

				printf("File name parsed: %s\n", fileName.c_str());
				if (row >= 16 || col >= 16) continue;

				imageInfo info;
				info.path = p.path().string();
				info.indexx = col;
				info.indexy = row;
				info.posx = camx;
				info.posy = camy;

				mImageInfo.push_back(info);


				mWidth = std::max(mWidth, col + 1);
				mHeight = std::max(mHeight, row + 1);
			}
			else {
				printf("[ERROR] File name is invalid!!\n");
				printf("        It must be like \"out_%%d_%%d_%%lf_%%lf.png\"\n");
				scanf("%s", ext);
				exit(1);
			}
		}
	}

	return true;
}

bool LightField::readLightField()
{

	bool firstTime = true;
	

	for (auto v : mImageInfo) {

		printf("Loading image %s\n", v.path.c_str());
		//image width and height
		int width(0), height(0);
		//OpenGL's image ID to map to

		int comp(0);

		stbi_set_flip_vertically_on_load(true);

		//pointer to the image data
		unsigned char* bits = stbi_load(v.path.c_str(), &width, &height, &comp, 0);

		//if this somehow one of these failed (they shouldn't), return failure
		if ((bits == 0) || (width == 0) || (height == 0))
			return false;

		
		if (firstTime) {
			
			TextureManager::Inst().CreateTexture3D(LIGHT_FILED_TEXTURE, width, height, mWidth * mHeight,
					GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR);
			TextureManager::Inst().BindTexture(LIGHT_FILED_TEXTURE);

			firstTime = false;
		}

		printf("%d %d %d\n", v.indexy , v.indexx, v.indexy * mWidth + v.indexx);
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, v.indexy * mWidth + v.indexx, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, bits);
		
		//Free FreeImage's copy of the data
		stbi_image_free(bits);
	}

	return true;
}

void LightField::useLightField()
{

}