#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
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

int main() {

	char filename[256];
	cout << "Enter input picture name\n";
	gets_s(filename);
	FILE* in = fopen(filename, "rb");
	if (!in) { cout << "Something wrong with input file"; exit(1); }
	cout << "Enter output picture name\n";
	gets_s(filename);
	FILE* out = fopen(filename, "wb");
	if (!out) { cout << "Something wrong with output file"; exit(2); }
	/*Opening input and putput file*/
	double n;
	cout << "Enter coefficient to resize: ";
	cin >> n;
	BMPHEAD head;
	fread(&head, sizeof(BMPHEAD), 1, in);
	/*reconfiguring old image*/
	int oldDepth = head.depth;
	int newDepth = head.depth = int(oldDepth * n);
	int oldWidth = head.width;
	int newWidth = head.width = int(oldWidth * n);
	int inPadding = (4 - (oldWidth * sizeof(PIXELDATA)) % 4) % 4;
	int outPadding = (4 - (newWidth * sizeof(PIXELDATA)) % 4) % 4;
	head.biSizeImage = ((sizeof(PIXELDATA) * newWidth) + outPadding) * abs(newDepth);
	head.filesize = head.biSizeImage + sizeof(BMPHEAD);
	fwrite(&head, sizeof(BMPHEAD), 1, out);
	PIXELDATA p;
	PIXELDATA* line = new PIXELDATA[oldWidth * sizeof(PIXELDATA)];// determine ratio
	double widthRatio = (double)oldWidth / (double)newWidth;
	double depthRatio = (double)oldDepth / (double)newDepth;
	PIXELDATA** oldPic = new PIXELDATA * [oldDepth];
	PIXELDATA** newPic = new PIXELDATA * [newDepth];
	for (int i = 0; i < oldDepth; i++) {
		oldPic[i] = new PIXELDATA[oldWidth];
	}
	for (int i = 0; i < newDepth; i++) {
		newPic[i] = new PIXELDATA[newWidth];
	}
	for (int i = 0; i < oldDepth; i++) {
		for (int j = 0; j < oldWidth; j++) {
			fread(&oldPic[i][j], sizeof(PIXELDATA), 1, in);
		}
		for(int j=1;j<=inPadding;j++)
			getc(in);
	}
	for (int i = 0; i < newDepth; i++) {
		float tmp = ((float)(i) / (float)(newDepth - 1) * (oldDepth - 1));
		int h = (int)floor(tmp);
		if (h <= 0) {
			h = 0;
		}
		else {
			if (h >= oldDepth - 1) {
				h = oldDepth - 2;
			}
		}
		float u = tmp - h;
		for (int j = 0; j < newWidth; j++) {
			tmp = (float)(j) / (float)(newWidth - 1) * (oldWidth - 1);
			int w = (int)floor(tmp);
			if (w < 0) {
				w = 0;
			}
			else {
				if (w >= oldWidth - 1) {
					w = oldWidth - 2;
				}
			}
			float t = tmp - w;
			float d1 = (1 - t) * (1 - u);
			float d2 = t * (1 - u);
			float d3 = t * u;
			float d4 = (1 - t) * u;
			PIXELDATA p1 = oldPic[h][w];
			PIXELDATA p2 = oldPic[h][w + 1];
			PIXELDATA p3 = oldPic[h + 1][w + 1];
			PIXELDATA p4 = oldPic[h + 1][w];
			PIXELDATA toAdd;
			toAdd.blueComponent = p1.blueComponent * d1 + p2.blueComponent * d2 + p3.blueComponent * d3 + p4.blueComponent * d4;
			toAdd.greenComponent = p1.greenComponent * d1 + p2.greenComponent * d2 + p3.greenComponent * d3 + p4.greenComponent * d4;
			toAdd.redComponent = p1.redComponent * d1 + p2.redComponent * d2 + p3.redComponent * d3 + p4.redComponent * d4;

			newPic[i][j] = toAdd;


		}
	}
	for (int i = 0; i < head.depth; i++) {
		for (int j = 0; j < head.width; j++) {
			fwrite(&newPic[i][j], sizeof(PIXELDATA), 1, out);
		}
		for (int j = 0; j < outPadding; j++)
		{
			fputc(0x00, out);
		}
	}
	fclose(in);
	fclose(out);
	cout << sizeof(PIXELDATA);
}