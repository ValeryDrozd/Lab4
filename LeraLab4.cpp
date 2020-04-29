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
	int8_t redComponent;
	int8_t greenComponent;
	int8_t blueComponent;
} PIXELDATA;

int main() {
	char filename[256];
	cout << "Enter input picture name\n;";
	gets_s(filename);
	FILE* in = fopen(filename, "rb");
	if (!in) { cout << "Something wrong with input file"; exit(1); }
	cout << "Enter output picture name\n;";
	gets_s(filename);
	FILE* out = fopen(filename, "wb");
	if (!out) { cout << "Something wrong with output file"; exit(2); }
	/*Opening input and putput file*/
	int n;
	cout << "Enter coefficient to resize: ";
	cin >> n;
	BMPHEAD head;
	fread(&head, sizeof(BMPHEAD), 1, in);
	/*reconfiguring old image*/
	int oldDepth = head.depth;
	int newDepth = head.depth = oldDepth * n;
	int oldWidth = head.width;
	int newWidth = head.width = oldWidth * n;
	int inPadding = (4 - (oldWidth * sizeof(PIXELDATA)) % 4) % 4;
	int outPadding = (4 - (newWidth * sizeof(PIXELDATA)) % 4) % 4;
	head.biSizeImage = ((sizeof(PIXELDATA) * newWidth) + outPadding) * abs(newDepth);
	head.filesize = head.biSizeImage + sizeof(BMPHEAD);
	fwrite(&head, sizeof(BMPHEAD), 1, out);
	PIXELDATA p;
	PIXELDATA* line = new PIXELDATA[oldWidth * sizeof(PIXELDATA)];// determine ratio
	double widthRatio = (double)oldWidth / (double)newWidth;
	double depthRatio = (double)oldDepth / (double)newDepth;
	int cachedScanline = -1;
	for (int i = 0; i < head.depth; i++)
	{
		int row = i * depthRatio;
		if (cachedScanline != row)
		{
			fseek(in, sizeof(head) + (((sizeof(PIXELDATA) * oldWidth) + inPadding) * row), SEEK_SET);
			fread(line, sizeof(PIXELDATA), oldWidth, in);
			cachedScanline = row;
		}
		for (int j = 0; j < newWidth; j++)
		{
			int column = j * widthRatio;
			fwrite(&line [column] , sizeof(PIXELDATA), 1, out);
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