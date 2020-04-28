#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
typedef unsigned __int16 WORD;

typedef struct {
	WORD   bfType;         // 0x4d42 | 0x4349 | 0x5450
	int    bfSize;         // размер файла
	int    bfReserved;     // 0
	int    bfOffBits;      // смещение до поля данных,
						   // обычно 54 = 16 + biSize
	int    biSize;         // размер струкуры в байтах:
						   // 40(BITMAPINFOHEADER) или 108(BITMAPV4HEADER)
						   // или 124(BITMAPV5HEADER)
	int    biWidth;        // ширина в точках
	int    biHeight;       // высота в точках
	WORD   biPlanes;       // всегда должно быть 1
	WORD   biBitCount;     // 0 | 1 | 4 | 8 | 16 | 24 | 32
	int    biCompression;  // BI_RGB | BI_RLE8 | BI_RLE4 |
						   // BI_BITFIELDS | BI_JPEG | BI_PNG
						   // реально используется лишь BI_RGB
	int    biSizeImage;    // Количество байт в поле данных
						   // Обычно устанавливается в 0
	int    biXPelsPerMeter;// горизонтальное разрешение, точек на дюйм
	int    biYPelsPerMeter;// вертикальное разрешение, точек на дюйм
	int    biClrUsed;      // Количество используемых цветов
						   // (если есть таблица цветов)
	int    biClrImportant; // Количество существенных цветов.
						   // Можно считать, просто 0
} BMPheader;
int* loadBMP(const char* fname)
{
	FILE* f = fopen(fname, "rb");
	if (!f) return NULL;
	BMPheader bh;    // File header sizeof(BMPheader) = 56
	size_t res;

	// читаем заголовок
	res = fread(&bh, 1, sizeof(BMPheader), f);
	if (res != sizeof(BMPheader)) { fclose(f); return NULL; }

	// проверяем сигнатуру
	if (bh.bfType != 0x4d42 && bh.bfType != 0x4349 && bh.bfType != 0x5450) { fclose(f); return NULL; }

	// проверка размера файла
	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	// восстановим указатель в файле:
	fseek(f, sizeof(BMPheader), SEEK_SET);
	// проверим условия
	if (bh.bfSize != filesize ||
		bh.bfReserved != 0 ||
		bh.biPlanes != 1 ||
		(bh.biSize != 40 && bh.biSize != 108 && bh.biSize != 124) ||
		bh.bfOffBits != 14 + bh.biSize ||

		bh.biWidth < 1 || bh.biWidth >10000 ||
		bh.biHeight < 1 || bh.biHeight>10000 ||
		bh.biBitCount != 24 ||             // пока рассматриваем только полноцветные изображения
		bh.biCompression != 0                // пока рассматриваем только несжатие изображения
		)
	{
		fclose(f);
		return NULL;
	}
	// Заголовок прочитан и проверен, тип - верный (BGR-24), размеры (mx,my) найдены
	int mx = bh.biWidth;
	int my = bh.biHeight;
	int mx3 = (3 * mx + 3) & (-4);    // Compute row width in file, including padding to 4-byte boundary
	unsigned char* tmp_buf = new unsigned  char[mx3 * my];    // читаем данные
	res = fread(tmp_buf, 1, mx3 * my, f);
	if ((int)res != mx3 * my) { delete[]tmp_buf; fclose(f); return NULL; }
	// данные прочитаны
	fclose(f);

	// выделим память для результата
	int* v = new int[mx * my];

	// Перенос данных (не забудем про BGR->RGB)
	unsigned char* ptr = (unsigned char*)v;
	for (int y = my - 1; y >= 0; y--) {
		unsigned char* pRow = tmp_buf + mx3 * y;
		for (int x = 0; x < mx; x++) {
			*ptr++ = *(pRow + 2);
			*ptr++ = *(pRow + 1);
			*ptr++ = *pRow;
			pRow += 3;
			ptr++;
		}
	}
	delete[]tmp_buf;
	return v;    // OK
}
int main() {
	int mx, my;
	loadBMP("test.bmp");
}