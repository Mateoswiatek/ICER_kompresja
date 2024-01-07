//
// Created by mateusz on 29.11.23.
//
#include <stdint.h>

typedef struct {
    uint16_t stages; // typowo 4-6 - ile segmentacji
    uint16_t segments; // typowo kazdy jest kompresowany oddzielnie. Pakiety nie powinny zawierać info z różnych segmentów
    uint16_t filter_type; // enum, 0 - 6
    uint16_t filter_parms; // enum, 0 - 3
    uint16_t context; // czy potrzebujemy? enum, 0 - 16
    uint16_t codes;
    uint32_t datastream_max_size; // aby się zmieściło, ew jako dwa i dodać / skleić.
}Params;
