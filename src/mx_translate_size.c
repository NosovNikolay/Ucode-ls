#include "uls.h"

void mx_translate_size(long_data_t **all_long_data, int size) {
    char sizes[5] = {'B', 'K', 'M', 'G', 'T'}; 
    int current_size = 0;
    float remainder = 0;
    for (int i = 0; i < size; i++) {
        current_size = 0;
        remainder = all_long_data[i]->f_size;

        while (remainder >= 1000) {
            remainder /= 1000;
            current_size++;
        }

        if (remainder >= 10) {
            all_long_data[i]->f_size = (int)(remainder + .5f);
        }
        else {
            all_long_data[i]->f_size = (int)(remainder);
            remainder *= 10;
            all_long_data[i]->size_remainder = (int)remainder % 10;
        }
        all_long_data[i]->type_size = sizes[current_size];
    }
    
}
