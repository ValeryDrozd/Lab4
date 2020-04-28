#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
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

typedef struct {
	int8_t redComponent;
	int8_t greenComponent;
	int8_t blueComponent;
} PIXELDATA;
const int width = 66;
const int height = 67;
int main() {
	FILE* in = fopen("test.bmp", "rb");
	if (!in) { cout << "Something wrong with input file"; exit(1); }
	FILE* out = fopen("out.bmp", "wb");
	if (!out) { cout << "Something wrong with output file"; exit(2); }
	BMPHEAD head;
	fread(&head, sizeof(BMPHEAD), 1, in);
	fwrite(&head, sizeof(BMPHEAD), 1, out);
	PIXELDATA p;
	cout << sizeof(PIXELDATA) << endl;
	for (int i = 1; i <= height; i++) {
		for (int j = 1; j <= width; j++) {
			fread(&p, sizeof(PIXELDATA), 1, in);
			fwrite(&p, sizeof(PIXELDATA), 1, out);
		}
		for (int j = 1; j <= (sizeof(PIXELDATA) * width) % 4; j++)
			fputc(0x00, out);
	}
	fclose(in);
	fclose(out);
	cout << sizeof(PIXELDATA);
}