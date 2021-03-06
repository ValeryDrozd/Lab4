﻿#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
using namespace std;
#pragma pack(2)
typedef struct {
	int8_t id[2];            // Завжди дві літери 'B' і 'M'
	int32_t filesize;        // Розмір файла в байтах
	int16_t reserved[2];     // 0, 0
	int32_t headersize;      // 54L для 24-бітних зображень
	int32_t infoSize;        // 40L для 24-бітних зображень
	int32_t width;           // ширина зображення в пікселях
	int32_t depth;           // висота зображення в пікселях
	int16_t biPlanes;        // 1 (для 24-бітних зображень)
	int16_t bits;            // 24 (для 24-бітних зображень)
	int32_t biCompression;   // 0L
	int32_t biSizeImage;     // Можна поставити в 0L для зображень без компрессії (наш варіант)
	int32_t biXPelsPerMeter; // Рекомендована кількість пікселів на метр, можна 0L
	int32_t biYPelsPerMeter; // Те саме, по висоті
	int32_t biClrUsed;       // Для індексованих зображень, можна поставити 0L
	int32_t biClrImportant;  // Те саме
} BMPHEAD;

#pragma pack(2)
typedef struct {
	uint8_t redComponent;
	uint8_t greenComponent;
	uint8_t blueComponent;
} PIXELDATA;

class Image {
	BMPHEAD head;
	PIXELDATA** pic;
public:
	void read(char* filename) {
		FILE* in = fopen(filename, "rb");
		if (!in) { cout << "Wrong input file\n"; exit(1); }
		fread(&this->head, sizeof(BMPHEAD), 1, in);
		this->pic = new PIXELDATA * [this->head.depth];
		for (int i = 0; i < this->head.depth; i++) {
			this->pic[i] = new PIXELDATA[this->head.width];
		}

		int inPadding = (4 - (this->head.width * sizeof(PIXELDATA)) % 4) % 4;
		for (int i = 0; i < this->head.depth; i++) {
			for (int j = 0; j < this->head.width; j++) {
				fread(&this->pic[i][j], sizeof(PIXELDATA), 1, in);
			}
			for (int j = 1; j <= inPadding; j++)
				getc(in);
		}
		fclose(in);
	}

	void interol(double n, Image& newPic) {
		/*reconfiguring old image*/
		if (n <= 0) {
			cout << "Wrong coefficient to scale\n";
		}
		int oldDepth = this->head.depth;
		int newDepth = this->head.depth = int(oldDepth * n);
		int oldWidth = this->head.width;
		int newWidth = this->head.width = int(oldWidth * n);
		int outPadding = (4 - (this->head.width * sizeof(PIXELDATA)) % 4) % 4;
		newPic.head = this->head;
		newPic.head.biSizeImage = ((sizeof(PIXELDATA) * newWidth) + outPadding) * abs(newDepth);
		newPic.head.filesize = head.biSizeImage + sizeof(BMPHEAD);

		PIXELDATA p;
		double widthRatio = (double)oldWidth / (double)newWidth;
		double depthRatio = (double)oldDepth / (double)newDepth;
		newPic.pic = new PIXELDATA * [newDepth];
		for (int i = 0; i < newDepth; i++) {
			newPic.pic[i] = new PIXELDATA[newWidth];
		}

		for (int i = 0; i < newDepth; i++) {
			float tmp = ((float)(i) / (float)(newDepth - 1) * (oldDepth - 1));
			int h = (int)floor(tmp);
			if (h <= 0) {
				h = 0;
			}
			else
				if (h >= oldDepth - 1) {
					h = oldDepth - 2;
				}

			if (h < 0)
			{
				h = 0;
			}
			if (oldDepth == 1)
			{
				tmp = 0;
			}
			float u = tmp - h;
			for (int j = 0; j < newWidth; j++) {
				tmp = (float)(j) / (float)(newWidth - 1) * (oldWidth - 1);
				int w = (int)floor(tmp);
				if (w < 0) {
					w = 0;
				}
				else
					if (w >= oldWidth - 1) {
						w = oldWidth - 2;
					}

				if (w < 0) {
					w = 0;
				}
				float t = tmp - w;
				if (oldWidth == 1) {
					t = 0;
				}
				float d1 = (1 - t) * (1 - u);
				float d2 = t * (1 - u);
				float d3 = t * u;
				float d4 = (1 - t) * u;
				PIXELDATA toAdd;
				PIXELDATA p1;
				PIXELDATA p2;
				PIXELDATA p3;
				PIXELDATA p4;
				if (oldWidth == 1 && oldDepth == 1)toAdd = this->pic[0][0];
				else
					if (oldDepth == 1) {
						p1 = this->pic[h][w];
						p2 = this->pic[h][w + 1];
						toAdd.blueComponent = p1.blueComponent * d1 + p2.blueComponent * d2;
						toAdd.greenComponent = p1.greenComponent * d1 + p2.greenComponent * d2;
						toAdd.redComponent = p1.redComponent * d1 + p2.redComponent * d2;
					}
					else
						if (oldWidth == 1) {
							p1 = this->pic[h][w];
							p4 = this->pic[h + 1][w];
							toAdd.blueComponent = p1.blueComponent * d1 + p4.blueComponent * d4;
							toAdd.greenComponent = p1.greenComponent * d1 + p4.greenComponent * d4;
							toAdd.redComponent = p1.redComponent * d1 + p4.redComponent * d4;
						}
						else
						{
							p1 = this->pic[h][w];
							p2 = this->pic[h][w + 1];
							p3 = this->pic[h + 1][w + 1];
							p4 = this->pic[h + 1][w];
							toAdd.blueComponent = p1.blueComponent * d1 + p2.blueComponent * d2 + p3.blueComponent * d3 + p4.blueComponent * d4;
							toAdd.greenComponent = p1.greenComponent * d1 + p2.greenComponent * d2 + p3.greenComponent * d3 + p4.greenComponent * d4;
							toAdd.redComponent = p1.redComponent * d1 + p2.redComponent * d2 + p3.redComponent * d3 + p4.redComponent * d4;
						}
				newPic.pic[i][j] = toAdd;
			}
		}
	}

	void write(char* filename) {
		FILE* out = fopen(filename, "wb");
		int outPadding = (4 - (this->head.width * sizeof(PIXELDATA)) % 4) % 4;
		if (!out) { cout << "Something wrong with output file"; exit(2); }
		fwrite(&this->head, sizeof(BMPHEAD), 1, out);
		for (int i = 0; i < this->head.depth; i++) {
			for (int j = 0; j < this->head.width; j++) {
				fwrite(&this->pic[i][j], sizeof(PIXELDATA), 1, out);
			}
			for (int j = 0; j < outPadding; j++)
			{
				fputc(0x00, out);
			}
		}
		fclose(out);
	}

};






int main(int argc,char* argv[]) {
	if (argc != 4) { cout << "Wrong input!"; exit(4); }
	Image base;
	Image toCopy;
	base.read(argv[1]);
	base.interol(atoi(argv[3]), toCopy);
	toCopy.write(argv[2]);
	cout << "Resizing " << argv[1] << " " << argv[3] << " times... Done!\n";
	cout << "Rezult is written to " << argv[2];
}