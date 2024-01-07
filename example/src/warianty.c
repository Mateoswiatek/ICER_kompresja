#include <stdio.h>

// Deklaracje funkcji pomocniczych
extern int encodeLogic();
extern int decodeLogic();

void ImageSizeByStages(int stages){
    int i;
    int W = 640;
    int H = 480;
    float W1, H1;

    printf("%d x %d = W x H\n", W, H);
    for (i = 1; i <= stages; i++) {
        W1 = (float)W / (1 << i);
        H1 = (float)H / (1 << i);
        printf("i=%d %f x %f\n", i, W1, H1);
    }
}

int main() {
    ImageSizeByStages(8);

    /*
    encodeLogic();
    decodeLogic();
    */

    return 0;
}


