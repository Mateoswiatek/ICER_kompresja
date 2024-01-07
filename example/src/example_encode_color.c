#include <stdio.h>
#include <string.h>

#define USE_ENCODE_FUNCTIONS
#define USE_UINT16_FUNCTIONS

#include "icer.h"
#include "color_util.h"

#include "parametry.h"

const char compressed_filename[] = "../ring/Dark_1_e745/compress.bin"; // wynikowy w binarce
const char filename[] = "../ring/Dark_1_e745/out1_3.bmp";

void rgb888_packed_to_yuv(uint16_t *y_channel, uint16_t *u_channel, uint16_t *v_channel, uint8_t *img, size_t image_w, size_t image_h, size_t rowstride) {
    int32_t r, g, b;
    uint8_t *pixel;

    uint16_t *output_y, *output_u, *output_v;
    for (size_t row = 0;row < image_h;row++) {
        pixel = img + 3 * rowstride * row;
        output_y = y_channel + rowstride * row;
        output_u = u_channel + rowstride * row;
        output_v = v_channel + rowstride * row;
        for (size_t col = 0;col < image_w;col++) {
            r = pixel[0];
            g = pixel[1];
            b = pixel[2];

            *output_y = CRGB2Y(r, g, b);
            *output_u = CRGB2Cb(r, g, b);
            *output_v = CRGB2Cr(r, g, b);
            pixel += 3;
            output_y++; output_u++; output_v++;
        }
    }
}

int read_file(char fileName[], unsigned char* buffer, size_t fileSize){
    FILE *file;

    // Open file
    file = fopen(fileName, "r+b");

    if (file == NULL) {
        perror("Error opening file\n");
        return -1;
    }

    // Read the file content into the fixed-size buffer
    size_t bytesRead;
    bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead == 0) {
        perror("Error reading file\n");
        fclose(file);
        return -1;
    }else
        printf("File reading was succesfull\n");

    // Close the file after reading into the buffer
    fclose(file);
    return 0;
}

// dostajac
void debayerowanie_stare(uint8_t **from_camera, uint8_t *my_data, size_t img_w, size_t img_h ){
    int data_index = 0;
    for(int i = 0; i<img_h/2; i+=2){ // i<214
        for(int j = 0; j<img_w/2; j+=2){
            my_data[data_index++] = from_camera[i][j+1]; // R
            my_data[data_index++] = from_camera[i][j];   // G
            my_data[data_index++] = from_camera[i+1][j]; // B
        }
    }
}




// symulacja dostępu do kamery - konkretna linia.
#define X 640
#define Y 428
#define BUFF_SIZE X*Y
unsigned char buffor[BUFF_SIZE];
unsigned char cam_buffor[Y][X];

unsigned char* get_line(int index) {
    unsigned char *pointer = cam_buffor[index];
    return pointer;
}



//210 pixeli x 320 pixeli
void dayberowanie_do_BIP(uint8_t *my_data, size_t img_pixel_w, size_t img_pixel_h ){
    int data_index = 0;
    for(size_t i = 0; i < img_pixel_h*2; i+=2){ // po liniach w wysokosci. 2x ilosc pixela
        // line1 G R
        // line2 B G

        //funkcja do supervisozra że chcemy dostać konkretną linię.
        uint8_t *line1 = get_line(i); // linia nr img_pixel_h
        uint8_t *line2 = get_line(i+1); // linia nr img_pixel_h+1

        for(size_t j = 0; j < img_pixel_w*2; j+=2){
            my_data[data_index++] = line1[j+1]; // R
            my_data[data_index++] = line1[j];   // G
            my_data[data_index++] = line2[j];   // B
        }
    }
}

// Zamienić rozmiary zdjęcia!!! wtutaj jak i w dekodowaniu!!

int main() {
    char* cam = "../ring/Dark_1_e745/data.bin";
    if(read_file(cam, buffor, BUFF_SIZE) == -1)
        return -1;
    int b = 0;
    for(int k=0; k<Y; k++){
        for(int q=0; q<X; q++){
            cam_buffor[k][q] = buffor[b];
            b++;
        }
    }

    unsigned char* ptr = get_line(3);
    for(int i=0; i<X; i++){
        printf("Indeks %u\n", ptr[i]);
    }

    // Symulacja Supervisora
    // =======================================================
    // Program



    const size_t out_w = 320; // 640
    const size_t out_h = 214; // 480


    Params params;
    params.stages = 4;
    params.segments = 6;
    params.filter_type = ICER_FILTER_A; // 0 - 6
    params.datastream_max_size = 1000000;

    clock_t begin, end;

    uint8_t data[out_h*out_w*3];
    uint16_t compress0[out_w*out_h];
    uint16_t compress1[out_w*out_h];
    uint16_t compress2[out_w*out_h];

    // tutaj będą 640 x 480
    // zapis z kamery do data
    dayberowanie_do_BIP(data, X/2, Y/2);


// odczytywanie od Kuby z RRRR...GGGG...BBBB to jest BSQ do data w formacie BIP
/*
    size_t pixels = out_w*out_h;
    uint8_t buffer[pixels*3];
    read_file("../ring/Dark_1_e745/input3.lun", buffer, pixels*3);

    for (int i = 0; i < 100; i++) {
        // Wyświetlenie wartości zielonego kanału piksela
        printf("z bufora[RGB] %i = %d %d %d\n", i, buffer[i], buffer[i+pixels], buffer[i+2*pixels]); // W przypadku RGBA, zielony kanał to drugi bajt
    }

    for (int i = 0; i < pixels; i++) {
        data[3*i] = buffer[i];
        data[3*i+1] = buffer[i+pixels];
        data[3*i+2] = buffer[i+2*pixels];
    }
*/


// Odczyt z biblioteki stbi_load
    /*

    int src_w, src_h, n, res = 0;
    printf("test compression code\n");

    printf("loading image: \"%s\"\n", filename);
    data = stbi_load(filename, &src_w, &src_h, &n, 3);
    if (data == NULL) {
        printf("invalid image\nexiting...\n");
        return 0;
    }

    FILE *ptr1;
    ptr1 = fopen(compressed_filename,"wb");
    size_t written = fwrite(data, sizeof(data), (out_w * out_h * 3), ptr1);
    printf("written: %llu\n", written);
    fflush(ptr1);
    fclose(ptr1);




    printf("loaded image\nwidth    : %5d\nheight   : %5d\nchannels : %5d\n", src_w, src_h, n);

    printf("resizing image to width: %4llu, height: %4llu\n", out_w, out_h);
    res = stbir_resize_uint8(data, src_w, src_h, 0,
                             resized, out_w, out_h, 0,
                             3);
    if (res == 0) {
        printf("resize failed\nexiting...\n");
        return 0;
    }
    printf("resize complete\n");

    int num_pixels_to_display2 = 100;
    for (int i = 0; i < num_pixels_to_display2; i += 3) {
        // Obliczanie indeksu początkowego dla każdego piksela
        int pixel_index = i * n;

        // Wyświetlenie wartości zielonego kanału piksela
        printf("resized[RGB] %i = %d %d %d\n", i/3, resized[i], resized[i+1], resized[i+2]); // W przypadku RGBA, zielony kanał to drugi bajt
    }
    */


    // Czy data jest w BIP-ie

    int num_pixels_to_display = 100;
    for (int i = 0; i < num_pixels_to_display; i += 3) {
        // Wyświetlenie wartości zielonego kanału piksela
        printf("data[RGB] %i = %d %d %d\n", i/3, data[i], data[i+1], data[i+2]); // W przypadku RGBA, zielony kanał to drugi bajt
    }

    printf("converting to yuv\n");
    // tu było resized, tera daje data
    rgb888_packed_to_yuv(compress0, compress1, compress2, data, out_w, out_h, out_w);


    icer_init();


    uint8_t datastream[params.datastream_max_size*2+500]; // * = malloc(params.datastream_max_size*2+500);
    icer_output_data_buf_typedef output;
    icer_init_output_struct(&output, datastream, params.datastream_max_size*2, params.datastream_max_size);

    icer_compress_image_yuv_uint16(compress0, compress1, compress2, out_w, out_h, params.stages, params.filter_type, params.segments, &output);

    FILE *ptr1;

    ptr1 = fopen(compressed_filename,"wb");
    size_t written = fwrite(output.rearrange_start, sizeof(output.rearrange_start[0]), output.size_used, ptr1);
    printf("written: %llu\n", written);
    fflush(ptr1);
    fclose(ptr1);

    printf("output saved\n");

    return 0;
}
