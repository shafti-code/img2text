#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../stb/stb_image.h"
#include "../stb/stb_image_write.h"
#include "../stb/stb_image_resize2.h"

bool debug;
bool use_stdout;

void debugf(const char * formatstring ,...){
    if(debug){
        va_list args;
        va_start(args,formatstring);
        printf("DEBUG: ");
        vprintf(formatstring,args);
        va_end(args);
    }
}

typedef struct {
    unsigned char* data;
    int width;
    int height;
    int channels;
}Image;

Image grayscale(Image image){

    Image grayscale_img;
    grayscale_img.data   = malloc(sizeof(char) * image.width * image.height * image.channels);
    grayscale_img.width  = image.width;
    grayscale_img.height = image.height;
    grayscale_img.channels = image.channels;

    for (int i = 0; i < image.width * image.height * image.channels; i += image.channels){
        float red   = (float)image.data[i];
        float blue  = (float)image.data[i+1];
        float green = (float)image.data[i+2];

        float gray  = (red + blue + green) / 3.0;

        grayscale_img.data[i]    = (unsigned char)gray;
        grayscale_img.data[i+1]  = (unsigned char)gray;
        grayscale_img.data[i+2]  = (unsigned char)gray;
    }
    if(debug){
        stbi_write_png("gray.png", grayscale_img.width, grayscale_img.height, grayscale_img.channels, grayscale_img.data, 0);
    }
    debugf(" Generated ./gray.png\n");
    return grayscale_img;
}

Image downscale(Image image, int height, int width){

    Image small_img;

    small_img.height = height;
    if (width == -1){
        small_img.width  = height * (image.height/image.width);
    } else {
        small_img.width = width;
    }
    small_img.channels = image.channels;
    debugf("downscale width: %d, height: %d\n",small_img.width, small_img.height);
    small_img.data = malloc(small_img.height * small_img.width * small_img.channels * sizeof(char));
    stbir_resize_uint8_linear(image.data,image.width,image.height,0,small_img.data,small_img.width,small_img.height,0, (stbir_pixel_layout)small_img.channels);
    if(debug){
        stbi_write_png("small.png", small_img.width, small_img.height, small_img.channels, small_img.data, 0);
    }
    debugf(" Generated ./small.png\n");

    return small_img;
}

Image quantize(Image image, int ammount){
    Image quantized_img;


    quantized_img.height = image.height;
    quantized_img.width  = image.width;
    quantized_img.channels = image.channels;

    quantized_img.data = malloc(sizeof(char) * quantized_img.width * quantized_img.height * quantized_img.channels);

    float lum = 0.0;
    unsigned char pixel_val = 0;
    for (int i = 0; i < image.height * image.width * image.channels; i+= image.channels){
        if(image.data[i] != 0){
            lum = (float)image.data[i] / 255.0;
        }

        lum = floorf(lum * (float)ammount) / (float)ammount;
        pixel_val = lum * 255.0;

        for (int j = 0; j < 3; j++){
            quantized_img.data[i + j] = pixel_val;
        }
    }
    debugf(" quantize generated range of values: ");
    if (debug){
        for (float i = 0.0; i <= 1.1; i += 0.1){
            printf("%d,",(unsigned char)(i * 255.0));
        }
    }
    debugf("\n");

    if(debug){
        stbi_write_png("quantized.png", quantized_img.width, quantized_img.height, quantized_img.channels, quantized_img.data, 0);
    }
    debugf(" Generated ./quantized.png\n");
    return quantized_img;
}

void image2ascii(Image image,char* outputpath){
    char * characters = {" .,-:ilIL#"};


    //i know code duplication is ugly get off my back its the simplest way to do this
    if (use_stdout){
        int index = 0;
        for (int i = 0; i < image.height; i++){
            for (int j = 0; j < image.width * image.channels; j+= image.channels){
                index = round((float)image.data[(i * image.width * image.channels) + j] / 255.0 * 10.0);
                putchar(characters[index]);
                putchar(characters[index]);
            }
            putchar('\n');
        }
    } else {
        FILE* fptr = fopen(outputpath, "w");
        if (fptr != NULL){
            int index = 0;
            for (int i = 0; i < image.height; i++){
                for (int j = 0; j < image.width * image.channels; j+= image.channels){
                    index = round((float)image.data[(i * image.width * image.channels) + j] / 255.0 * 10.0);
                    fputc(characters[index],fptr);
                    fputc(characters[index],fptr);
                }
                fputc('\n',fptr);
            }
            debugf(" Generated %s",outputpath);
        } else {
            printf("could not open file for writing");
        }
    }
}

int main(int argc, char ** argv){
    if (argc == 1){
        printf("img2ascii: <flags> <input_path>\n");
        printf("\t-h number\tdesired height of the image in characters (default 40)\n");
        printf("\t-w number\tdesired width  of the image in characters (if not provided it will be calculated from the original image ratio)\n");
        printf("\t-o path\t\toutput path\n");
        printf("\t-d\t\tenable debug information (emiting intermidiate files, debug messages)\n");
        printf("\t-s\t\tprint the output to standard output\n");
        return 0;
    }
    int height = 40;
    int width = -1;
    debug = false;
    use_stdout = false;

    char* path;
    char* outputpath = "ascii.txt";

    char c;
    const char* errstr;
    for (int i = 1;i < argc;i++){
        c = getopt(argc,argv,"dh:w:o:s");
        switch (c) {
            case -1:
                path = argv[i];
                debugf("path extracted %s\n",path);
                optind++;
                break;
            case 'h':
                height = strtonum(optarg,1,INT_MAX,&errstr);
                if (errstr != NULL){
                    printf("ERROR: the height value could not be parsed into an integer\n");
                }
                debugf("height extracted: %d \n",height);
                i++;
                break;
            case 'w':
                width = strtonum(optarg,1,INT_MAX,&errstr);
                if (errstr != NULL){
                    printf("ERROR: the width value could not be parsed into an integer\n");
                }
                debugf("width extracted: %d \n",width);
                i++;
                break;
            case 'd':
                debugf("debug is set\n");
                debug = true;
                break;
            case 'o':
                debugf("output path is %s\n",optarg);
                outputpath = optarg;
                outputpath = malloc(strlen(optarg) * sizeof(char));
                strcpy(outputpath,optarg);
                break;
            case 's':
                debugf("use_stdout is set\n");
                use_stdout = true;
        }
    }

    debugf("outputpath is %s\n",outputpath);

    Image img;
    img.data = stbi_load(path,&img.width, &img.height, &img.channels, 0);
    if (img.data == NULL){
        printf("could not load image");
        return 1;
    }

    debugf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", img.width, img.height, img.channels);

    Image grayscale_img = grayscale(img);

    Image quantized_img = quantize(grayscale_img,10);

    Image small_img = downscale(quantized_img,height,width);


    image2ascii(small_img,outputpath);


    free(grayscale_img.data);
    free(small_img.data);
    free(quantized_img.data);
    stbi_image_free(img.data);

}
