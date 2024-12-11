#include <stdint.h>

#define GRAINS_LIBRARY_VERSION 1
#define GRAINS_FORMAT_VERSION 1

#define GRAINS_HEADER "GRAINS"
#define GRAINS_ENDIANNESS_TET 0x01020304

#if UINTPTR_MAX == 0xFFFFu
typedef uint16_t GRAINS_UINTPTR;
#elif UINTPTR_MAX == 0xFFFFFFFFu
typedef uint32_t GRAINS_UINTPTR;
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFu
typedef uint64_t GRAINS_UINTPTR;
#endif


uint8_t grains_memcmp(void* buffer1, void* buffer2, GRAINS_UINTPTR size);

void grains_memset(void* buffer, uint8_t byte, GRAINS_UINTPTR size);

void grains_memcpy(void* from, void* to, GRAINS_UINTPTR size);


enum GRAINS_ENDIANESS
{
    GRAINS_BIG_ENDIAN = 0,
    GRAINS_LITTLE_ENDIAN = 1,
    GRAINS_UNKNOWN_ENDIAN = 2
};

uint8_t grains_get_endianess();

#if defined(_MSC_VER)
#include <intrin.h>
#endif

uint16_t grains_bswap16(uint16_t uint16);

uint32_t grains_bswap32(uint32_t uint32);

uint64_t grains_bswap64(uint64_t uint64);


enum GRAINS_FORMAT_TYPE
{
    GRAINS_ONE_BYTE = 0,
    GRAINS_WITH_CLASS_BYTE = 1
};

enum GRAINS_TYPE
{
    GRAINS_TYPE_UINT = 0,
    GRAINS_TYPE_SINT = 1,
    GRAINS_TYPE_FLOAT = 2,
    GRAINS_TYPE_BFLOAT = 3,
    GRAINS_TYPE_POSIT = 4,
    GRAINS_TYPE_QUIRE = 5,
    GRAINS_TYPE_ARRAY = 6, // Indicates there is supposed to be XYZ types ahead.
    GRAINS_TYPE_BIN = 7 // You can use this as a string.
};

enum GRAINS_SIZE
{
    GRAINS_SIZE_8 = 0,
    GRAINS_SIZE_16 = 1,
    GRAINS_SIZE_32 = 2,
    GRAINS_SIZE_64 = 3,
    GRAINS_SIZE_128 = 4,
    GRAINS_SIZE_256 = 5,
    GRAINS_SIZE_512 = 6,
    GRAINS_SIZE_1024 = 7,

    GRAINS_SIZE_24 = 8,
    GRAINS_SIZE_48 = 9,
    GRAINS_SIZE_80 = 10,
    GRAINS_SIZE_112 = 11,
    GRAINS_SIZE_224 = 12,
    GRAINS_SIZE_384 = 13,
    GRAINS_SIZE_768 = 14,
    GRAINS_SIZE_2048 = 15
};

GRAINS_UINTPTR GRAINS_TYPE_SIZES[16] = { 1, 2, 4, 8,
                                   16, 32, 64, 128,
                                   3, 4, 10, 14,
                                   28, 48, 96, 256 };

union grains_tag
{
    uint8_t byte;
    struct
    {
        uint8_t type : 3;
        uint8_t size : 4;
        uint8_t has_name : 1;
        uint8_t endianess : 1;
    } bits;
};

struct grains_tag_info
{
    uint8_t format;
    uint8_t endianess;

    uint8_t* buffer;

    union grains_tag tag;
    uint8_t class_byte;

    uint8_t* name;
    uint8_t name_size;

    GRAINS_UINTPTR bin_size;
    uint8_t* data;

    GRAINS_UINTPTR total_size;
};

struct grains_match_tag_info
{
    uint8_t format;
    uint8_t match_format; // If = 2, any is accepted.

    uint8_t endianess;
    uint8_t match_endianess; // If = 2, any is accepted.

    uint8_t* buffer;

    union grains_tag tag;
    uint8_t match_tag_type; // If = 2, any is accepted.
    uint8_t match_tag_size; // If = 2, any is accepted.
    uint8_t match_tag_has_name; // If = 2, any is accepted.
    uint8_t match_tag_endianess; // If = 2, any is accepted.

    uint8_t class_byte;
    uint8_t match_class; // If = 2, any is accepted.

    uint8_t* name;
    uint8_t match_name; // If = 2, any is accepted.
    uint8_t name_size;
    uint8_t match_name_size; // If = 2, any is accepted.

    GRAINS_UINTPTR bin_size;
    uint8_t match_bin_size; // If = 2, any is accepted.
    uint8_t* data;
    uint8_t match_data; // If = 2, any is accepted.

    GRAINS_UINTPTR total_size;
    uint8_t match_total_size; // If = 2, any is accepted.
};

struct grains_context
{
    uint8_t format;
    uint8_t endianess;

    uint8_t* buffer;
    GRAINS_UINTPTR index;
    GRAINS_UINTPTR size;

    uint8_t max_str_len;

    GRAINS_UINTPTR max_tag_count;
    GRAINS_UINTPTR type_counts[8];
    GRAINS_UINTPTR size_counts[16];
};

GRAINS_UINTPTR grains_add_counts(GRAINS_UINTPTR* counts, GRAINS_UINTPTR how_many_counts);

uint8_t grains_range_within_bounds(struct grains_context* context, GRAINS_UINTPTR length);

uint8_t grains_peek_next_tag(struct grains_context* context, struct grains_tag_info* tag_info);

uint8_t grains_get_bin_info(struct grains_context* context, struct grains_tag_info* tag_info, uint8_t variable_size, uint8_t endianess);

void* grains_memchr(const void* ptr, uint8_t character, GRAINS_UINTPTR count);

uint8_t grains_try_read_next_tag(struct grains_context* context, struct grains_tag_info* tag_info);

uint8_t grains_calculate_tag_size(struct grains_tag_info* tag_info);

uint8_t grains_try_write_tag(struct grains_context* context, struct grains_tag_info* tag_info);

uint8_t grains_compare_tag(struct grains_tag_info* tag_info, struct grains_match_tag_info* match_tag_info);

// Set tag_info to 0 to only get the tag count. *tag_count must be zero.
// If the tag limit is zero, it will count as many tags as possible.
// This will stop attempting to read after any error.
uint8_t grains_list_all_tags(struct grains_context* context, struct grains_tag_info* tag_info, GRAINS_UINTPTR* tag_count, GRAINS_UINTPTR tag_limit);

// This will return the matches to a search query.
// You can only compare bin types with bin types.
// Set results to 0 to get the number of results.
uint8_t grains_search(struct grains_context* context, struct grains_match_tag_info* search_query, struct grains_tag_info* tag_info, 
    GRAINS_UINTPTR range_start, GRAINS_UINTPTR range_end, GRAINS_UINTPTR* results, GRAINS_UINTPTR result_count);