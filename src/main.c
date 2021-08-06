#include <FreeImage.h>
#include <stdio.h>

void doStuff(FIBITMAP *bm){
    if (FreeImage_AdjustContrast(bm, 400)){
        printf("yey!\n");
    }
}

int main(int argc, char *argv[]){
    FreeImage_Initialise(TRUE);
    printf("Image to Ascii\nMade using FreeImage %s\n", FreeImage_GetVersion());

    FIBITMAP *bitmap = FreeImage_Load(FIF_PNG, "sample.png", BMP_DEFAULT);
    if (bitmap) {
        doStuff(bitmap);
        FreeImage_Save(FIF_PNG, bitmap, "sample-output.png", PNG_DEFAULT);
        FreeImage_Unload(bitmap);
    }else {
        printf("Error opening file\n");
    }

    

    FreeImage_DeInitialise();
    return 0;
}
