#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
	Struct with data of image
*/
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
/*
	Struct with pixel
*/
typedef struct {
	int8_t redComponent;
	int8_t greenComponent;
	int8_t blueComponent;
} PIXELDATA;

/*
	Image class
*/
class Image {
	/*
		File with filename and header
	*/
	char baseImg[256];
	BMPHEAD head;
public:
	/*
		Initialising Image class
	*/
	Image() {
		cout << "Reading image\n";
		cout << "Enter path to file\n";
		gets_s(baseImg);
		FILE* imgfile = fopen(baseImg, "rb");
		if (!imgfile)
		{
			cout << "File error!"; exit(1);
		}
		fclose(imgfile);
	}

	void Resize() {
		/*
			Opening base image file and entering resize coefficient
		*/
		FILE* img = fopen(baseImg,"rb");
		fread(&head, sizeof(BMPHEAD), 1, img);
		cout << "Enter coefficient to resize image\n";
		int n;
		cin >> n;
		cout << "Enter output file\n";
		cin.ignore(256, '\n');
		char outFile[256];
		gets_s(outFile);
		FILE* out = fopen(outFile, "wb");
		fwrite(&head,sizeof(BMPHEAD),1,out);
		PIXELDATA* first;
	//	fread(&first, sizeof(first), 1, img);
		for (int i = 0;i<65;i++)
		{
			for (int j = 0; j < 65;j++) {
				fread(&first, sizeof(PIXELDATA), 1, img);
				fwrite(&first, sizeof(PIXELDATA), 1, out);
			}
		}
 		fclose(img);
		fclose(out);
	}


};


int main()
{
	
	Image img;
	img.Resize();
	int i;
	
	FILE* f = fopen("test.bmp", "rb");
	unsigned char info[54];

	// read the 54-byte header
	fread(info, sizeof(unsigned char), 54, f);

	// extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];
}