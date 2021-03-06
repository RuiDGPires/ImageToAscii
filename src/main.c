#include <FreeImage.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTPUT_SCALE 0.1
#define CONTRAST_MUL 500
#define NCHARS 25
#define MAX_FILENAME 200
#define INVERT 0

const char table[NCHARS] = {' ', '.', '-', '\'', ':', ';', '+', '=', '*', '/', ')', 'c', '7', 'x', 'f', 'X', '4', '%',  'O', '8', 'B', 'M', 'W', '@', '#'};

float out_scale = OUTPUT_SCALE;

char ColorToChar(int val){
    if (INVERT)
        val = 255 - val;
    return table[(NCHARS - 1) * val / 255];
}

void throwError(char message[]){
    fprintf(stderr, "\033[1;31m");
    fprintf(stderr,"%s\n", message);
    fprintf(stderr, "\033[0m");

    FreeImage_DeInitialise();
    exit(0);
}

char *doStuff(FIBITMAP *bm){
    int out_width = FreeImage_GetWidth(bm)*out_scale, out_height = FreeImage_GetHeight(bm)*out_scale;
    if (out_width < 4 || out_height < 4) throwError("Size is too small");
    FreeImage_SetTransparent(bm, FALSE);
    *bm = *FreeImage_ConvertTo24Bits(bm);
    
    if (!FreeImage_AdjustContrast(bm, out_scale < 0.15? CONTRAST_MUL*0.02/out_scale : CONTRAST_MUL/100)) throwError("Adjust Contrast Failed");

    *bm = *FreeImage_ConvertToGreyscale(bm);
    *bm = *FreeImage_Rescale(bm, out_width, out_height, FILTER_BILINEAR);

    char *buffer;
    unsigned buffer_size = sizeof(char)*(2*out_width + 1)*out_height +1;
    unsigned write_index = buffer_size - 2*out_width - 2;

    if ((buffer = malloc(buffer_size)) == NULL) throwError("Error allocating memory");

    // Calculate the number of bytes per pixel (3 for 24-bit or 4 for 32-bit)
    int bytespp = FreeImage_GetLine(bm) / out_width;


    for (unsigned y = 0; y < out_height; ++y){
        BYTE *bits = FreeImage_GetScanLine(bm, y);
        for (unsigned x = 0; x < out_width; ++x){
            buffer[write_index++] = ColorToChar(bits[FI_RGBA_RED]);
            buffer[write_index++] = ' ';
            bits += bytespp;
        }
        buffer[write_index++] = '\n';
        write_index -= 2*(2*out_width + 1);
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
    
    if (argc != 2 && argc != 3) throwError("Wrong number of command line arguments:\nAtoI <filename> [output_scale]");    

    if (argc == 3) out_scale = atof(argv[2]);

    FIBITMAP *bitmap = loadImage(argv[1]);

    if (bitmap) {
        char *buffer = doStuff(bitmap);
#ifdef DEBUG
        if (!FreeImage_Save(FIF_JPEG, bitmap, "sample-output.jpeg", JPEG_DEFAULT)) throwError("Error Saving Image");
#endif
        FreeImage_Unload(bitmap);

        printf("%s\n", buffer);
        free(buffer);
    }else throwError("Error Opening File");

    

    FreeImage_DeInitialise();
    return 0;
}
