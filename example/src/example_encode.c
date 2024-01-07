#include <stdio.h>
#include <string.h>
#include <time.h>

#include <stdlib.h>

#include "stb_image.h"
#include "stb_image_resize.h"

#define USE_ENCODE_FUNCTIONS
#define USE_UINT16_FUNCTIONS

#include "icer.h"
#include "parametry.h"

/*  nie potrzebnie ale w stb_image -> zdefiniować #define STBI_NO_STDIO -> usuwa kod za ładowanie z pliku. Aby bez dynamicznej: STBI_MALLOC, STBI_REALLOC, STBI_FREE
 *  ale tak to nie ma mallocow. ez
 *  w icer.h -> #define USER_PROVIDED_BUFFERS własne bufory
 *  w icer.h -> mozna własną implementację crc32 -> cykliczny kod nadmiarowy.
 *
 * stbi__context - struktura do danych z obrazu, może dałoby się tu wpisać swoje dane i by działało.
 * stbi__result_info -> struktura info o zapisie, ile bitow na kanal, etc
 *
 *
 */

void saveArrayToBinaryFile(const uint16_t *array, const char *filename, size_t size) {
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(array, sizeof(uint16_t), size / sizeof(uint16_t), file);
        fclose(file);
        printf("Tablica została zapisana do pliku binarnego.\n");
    } else {
        fprintf(stderr, "Nie można otworzyć pliku do zapisu.\n");
    }
}


int main() {
    const char compressed_filename[] = "../ring/Dark_1_e745/compress.bin"; // wynikowy w binarce
    const char filename[] = "../ring/Dark_1_e745/out1_3.bmp";

    const size_t out_w = 640;
    const size_t out_h = 480;

    Params params;
    params.stages = 4;
    params.segments = 6;
    params.filter_type = ICER_FILTER_A;
    params.datastream_max_size = 1000000;

    uint8_t resized[out_w*out_h]; //    uint8_t *resized = malloc(out_w*out_h); na koniec free(resized);
    uint16_t compress[out_w*out_h*2]; //    uint16_t *compress = malloc(out_w*out_h*2); na koniec free(compress);

    icer_init();

    //TODO do wywalenia odczyt z pliku:
    int res = 0;
    int src_w, src_h, n;
    uint8_t *data;


    int x =0;
    data = stbi_load(filename, &src_w, &src_h, &n, 4);
    if (data == NULL) {
        printf("invalid image\nexiting...\n");
        return 0;
    }

    int num_pixels_to_display = 10;
    for (int i = 0; i < num_pixels_to_display; ++i) {
        // Obliczanie indeksu początkowego dla każdego piksela
        int pixel_index = i * n;

        // Wyświetlenie wartości zielonego kanału piksela
        printf("Piksel %d: %d\n", i + 1, data[pixel_index + 1]); // W przypadku RGBA, zielony kanał to drugi bajt
    }







    res = stbir_resize_uint8(data, src_w, src_h, 0,
                             resized, out_w, out_h, 0,
                             1);
    if (res == 0) {
        printf("resize failed\nexiting...\n");
        return 0;
    }
    // Do tego miejsca wywalić

    // to być może też:
    printf("converting to int16\n");
    for (size_t i = 0;i < out_h*out_w;i++) {
        compress[i] = resized[i];
    }



    FILE *ptr1;

    ptr1 = fopen(compressed_filename,"wb");
    size_t written = fwrite(data, sizeof(data), (out_w * out_h * 2), ptr1);
    printf("written: %llu\n", written);
    fflush(ptr1);
    fclose(ptr1);


    /*

    uint8_t datastream[params.datastream_max_size*2+500]; //uint8_t *datastream = malloc(params.datastream_max_size*2+500); na koniec free(datastream);
    icer_output_data_buf_typedef output;
    icer_init_output_struct(&output, datastream, params.datastream_max_size*2, params.datastream_max_size);

    clock_t begin, end;
    begin = clock();
    // uint16_t wskaźnik na miejsce gdzie zapisać obraz skomporesowany -> rozmiar w*h*2
    icer_compress_image_uint16(compress, out_w, out_h, params.stages, params.filter_type, params.segments, &output);

    end = clock();

    printf("compressed size %llu, time taken: %lf\n", output.size_used, (float)(end-begin)/CLOCKS_PER_SEC);

*/






/*
    long liczba = 0;
    // tutaj przekazujemy masz segment do przesłania,
    for (size_t it = 0; it < output.size_used; it += sizeof(icer_image_segment_typedef)) { // size_allocated size_used

        icer_image_segment_typedef *segment = (icer_image_segment_typedef *)(output.rearrange_start + it);

        char segment_filename[1000];

        sprintf(segment_filename, "segment_%ld.bin", segment->segment_number);
        printf("nazwa %s\n", segment_filename);


        printf("Number of segment: %ld\n", segment->segment_number);


        // d zamiast hhu
        printf("Decomp Level: %hhu\n", segment->decomp_level);
        printf("Subband Type: %hhu\n", segment->subband_type);
        printf("LSB Channel: %hhu\n", segment->lsb_chan);
        printf("Number of segment: %hhu\n", segment->segment_number);
        printf("Data_length: %ld\n", segment->data_length);


        FILE *segment_file = fopen(segment_filename, "wb");
        if (segment_file != NULL) {
            fwrite(segment, sizeof(icer_image_segment_typedef), 1, segment_file);
            fclose(segment_file);
        } else {
            printf("Nie można utworzyć pliku dla segmentu %d\n", segment->segment_number);
        }

        liczba++;
    }
    printf("ilosc segmentow %ld\n", liczba);


 */

    //  tego nie będziemy potrzbować

/*
    FILE *ptr1;

    ptr1 = fopen(compressed_filename,"wb");
    size_t written = fwrite(output.rearrange_start, sizeof(output.rearrange_start[0]), output.size_used, ptr1);
    printf("written: %llu\n", written);
    fflush(ptr1);
    fclose(ptr1);
*/
    printf("output saved\n");
    stbi_image_free(data);
    return 0;
}

