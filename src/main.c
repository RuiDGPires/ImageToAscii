#include <FreeImage.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTPUT_SCALE 0.1
#define CONTRAST_MUL 900000
#define NCHARS 15
#define MAX_FILENAME 200

const char table[NCHARS] = {'.', '-', ':', ';', ')', '+', 'x', '7', 'O', '8', '%', '@', 'N', 'M','#'};

char ColorToChar(int val){
    val = 255 - val;
    return table[(NCHARS - 1) * val / 255];
}

void throwError(char message[]){
    fprintf(stderr, "\033[1;31m");
    fprintf(stderr,"%s\n", message);
    fprintf(stderr, "\033[0m");
    exit(0);
}

char *doStuff(FIBITMAP *bm){
    int out_width = FreeImage_GetWidth(bm)*OUTPUT_SCALE, out_height = FreeImage_GetHeight(bm)*OUTPUT_SCALE;
    FreeImage_SetTransparent(bm, FALSE);
    *bm = *FreeImage_ConvertTo24Bits(bm);
    
    if (!FreeImage_AdjustContrast(bm, CONTRAST_MUL)) throwError("Adjust Contrast Failed");
    *bm = *FreeImage_ConvertToGreyscale(bm);
    *bm = *FreeImage_Rescale(bm, out_width, out_height, FILTER_BILINEAR);

    char *buffer;
    unsigned buffer_size = sizeof(char)*(out_width + 1)*out_height + 1;
    unsigned write_index = buffer_size - out_width - 2;

    if ((buffer = malloc(buffer_size)) == NULL) throwError("Error allocating memory");

    // Calculate the number of bytes per pixel (3 for 24-bit or 4 for 32-bit)
    int bytespp = FreeImage_GetLine(bm) / out_width;


    for (unsigned y = 0; y < out_height; ++y){
        
        BYTE *bits = FreeImage_GetScanLine(bm, y);
        for (unsigned x = 0; x < out_width; ++x){
            buffer[write_index++] = ColorToChar(bits[FI_RGBA_RED]);
            bits += bytespp;
        }
        buffer[write_index++] = '\n';
        write_index -= 2*(out_width + 1);
    }

    return buffer;
}

FIBITMAP *loadImage(const char filename[]){
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    fif = FreeImage_GetFileType(filename, 0);
    if (fif == FIF_UNKNOWN){
        fif = FreeImage_GetFIFFromFilename(filename);
    }
    if (fif == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(fif)) return NULL;
    
    return FreeImage_Load(fif, filename, BMP_DEFAULT);
}

int main(int argc, char *argv[]){
    FreeImage_Initialise(TRUE);
    printf("Image to Ascii\nMade using FreeImage %s\n", FreeImage_GetVersion());
    
    if (argc != 2) throwError("Wrong number of command line arguments:\nAtoI <filename>");    

    FIBITMAP *bitmap = loadImage(argv[1]);

    if (bitmap) {
        char *buffer = doStuff(bitmap);
        if (!FreeImage_Save(FIF_JPEG, bitmap, "sample-output.jpeg", PNG_DEFAULT)) throwError("Error Saving Image");
        FreeImage_Unload(bitmap);

        printf("%s\n", buffer);
        free(buffer);
    }else throwError("Error Opening File");

    

    FreeImage_DeInitialise();
    return 0;
}
