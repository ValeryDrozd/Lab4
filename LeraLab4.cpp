// LeraLab4.cpp : This file contains the 'main' function. Program execution begins and ends there.

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


class Image{
    FILE* baseImg;
    BMPHEAD head;
public:

    Image() {
        cout << "Reading image\n";
        cout << "Enter path to file\n";
        char path[256];
        gets_s(path);
        baseImg = fopen(path, "rb");
        fread(&head, sizeof(BMPHEAD), 1, baseImg);
    }

    void Resize() {
        cout << "Enter coefficient to resize image\n";
        int n;
        cin >> n;
        int oldHeight = head.depth,oldWidht = head.width;
        this->head.depth *= n; this->head.width *= n;
		int padding = (4 - (head.width * sizeof(PIXELDATA)) % 4) % 4;
		int oldPadding = (4 - (oldWidht * sizeof(PIXELDATA)) % 4) % 4;
        head.biSizeImage = abs(head.depth) * ((head.width * sizeof(PIXELDATA)) + padding);
        head.filesize = head.biSizeImage + sizeof(head);
        PIXELDATA* pixels = new PIXELDATA [this->head.width];
        cout << "Enter name of output file\n";
        char outFile[256];
        gets_s(outFile);
        FILE* outImg = fopen(outFile, "wb");
        FILE* outImg = fopen(outFile, "wb");
        PIXELDATA currpix;
        for (int i = 0; i < abs(oldHeight); i++) {
            int currPixNum = 0;
            for (int j = 0; j < oldWidht; j++) {
                fread(&currpix, sizeof(currpix), 1, baseImg);
                for (int k=0;k<n;k++)
                {
                    pixels[currPixNum] = currpix;
                    currPixNum += 1;
                }
            }
            for (int j = 0; j < n; i++) {
                for (int k = 0;k<head.width;k++)
                {
                    fwrite(&pixels[k], sizeof(PIXELDATA), 1, outImg);
                }
            }
        }
        fclose(outImg);
    }

  
};


int main()
{
    
}
