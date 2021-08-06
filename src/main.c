#include <FreeImage.h>
#include <stdio.h>
#include <stdlib.h>

void throwError(char message[]){
    fprintf(stderr, "\033[1;31m");
    fprintf(stderr,"%s\n", message);
    fprintf(stderr, "\033[0m");
    exit(0);
}

void doStuff(FIBITMAP *bm){
    if (!FreeImage_AdjustContrast(bm, 400)) throwError("Adjust Contrast Failed");
    *bm = *FreeImage_ConvertToGreyscale(bm);
}

int main(int argc, char *argv[]){
    FreeImage_Initialise(TRUE);
    printf("Image to Ascii\nMade using FreeImage %s\n", FreeImage_GetVersion());

    FIBITMAP *bitmap = FreeImage_Load(FIF_PNG, "sample.png", BMP_DEFAULT);
    if (bitmap) {
        doStuff(bitmap);
        if (!FreeImage_Save(FIF_PNG, bitmap, "sample-output.png", PNG_DEFAULT)) throwError("Error Saving Image");
        FreeImage_Unload(bitmap);
    }else throwError("Error Opening File");

    

    FreeImage_DeInitialise();
    return 0;
}
