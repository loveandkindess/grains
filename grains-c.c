#include "grains-c.h"

uint8_t grains_memcmp(void* buffer1, void* buffer2, GRAINS_UINTPTR size)
{
    for (GRAINS_UINTPTR i = 0; i < size; i++)
    {
        if (*(uint8_t*)((GRAINS_UINTPTR)buffer1 + i) != *(uint8_t*)((GRAINS_UINTPTR)buffer2 + i))
        {
            return 0;
        }
    }

    return 1;
}

void grains_memset(void* buffer, uint8_t byte, GRAINS_UINTPTR size)
{
    for (GRAINS_UINTPTR i = 0; i < size; i++)
    {
        *(uint8_t*)((GRAINS_UINTPTR)buffer + i) = byte;
    }
}

void grains_memcpy(void* from, void* to, GRAINS_UINTPTR size)
{
    for (GRAINS_UINTPTR i = 0; i < size; i++)
    {
        *(uint8_t*)((GRAINS_UINTPTR)to + i) = *(uint8_t*)((GRAINS_UINTPTR)from + i);
    }
}


uint8_t grains_get_endianess()
{
    volatile uint32_t i = 0x01234567;
    // return 0 for big endian, 1 for little endian.
    return (*((uint8_t*)(&i))) == 0x67;
}

#if UINTPTR_MAX >= 0xFFFFu
uint16_t grains_bswap16(uint16_t uint16)
{
#if defined(__clang__) || defined(__llvm__)
    return __builtin_bswap16(uint16);
#elif defined(__GNUC__) || defined(__GNUG__)
    return __builtin_bswap16(uint16);
#elif defined(_MSC_VER)
    return _byteswap_uint16(uint16);
#endif
}
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFu
uint32_t grains_bswap32(uint32_t uint32)
{
#if defined(__clang__) || defined(__llvm__)
    return __builtin_bswap32(uint32);
#elif defined(__GNUC__) || defined(__GNUG__)
    return __builtin_bswap32(uint32);
#elif defined(_MSC_VER)
    return _byteswap_uint32(uint32);
#endif
}
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFFFFFFFFFu
uint64_t grains_bswap64(uint64_t uint64)
{
#if defined(__clang__) || defined(__llvm__)
    return __builtin_bswap64(uint64);
#elif defined(__GNUC__) || defined(__GNUG__)
    return __builtin_bswap64(uint64);
#elif defined(_MSC_VER)
    return _byteswap_uint64(uint64);
#endif
}
#endif

GRAINS_UINTPTR grains_add_counts(GRAINS_UINTPTR* counts, GRAINS_UINTPTR how_many_counts)
{
    GRAINS_UINTPTR result = 0;

    for (GRAINS_UINTPTR i = 0; i < how_many_counts; i++)
    {
        result += counts[i];
    }

    return result;
}

uint8_t grains_range_within_bounds(struct grains_context* context, GRAINS_UINTPTR length)
{
    if ((context->buffer + context->index + length)
    > (context->buffer + context->size))
    {
        return 0;
    }

    return 1;
}

uint8_t grains_peek_next_tag(struct grains_context* context, struct grains_tag_info* tag_info)
{
    switch (context->format)
    {
    case GRAINS_ONE_BYTE:
        if (!grains_range_within_bounds(context, 1))
        {
            return 0;
        }

        tag_info->format = context->format;
        tag_info->tag = *(union grains_tag*)(context->buffer + context->index);
        break;

    case GRAINS_WITH_CLASS_BYTE:
        if (!grains_range_within_bounds(context, 2))
        {
            return 0;
        }

        tag_info->format = context->format;
        tag_info->tag = *(union grains_tag*)(context->buffer + context->index);
        tag_info->class_byte = *(uint8_t*)(context->buffer + context->index);
        break;

    default:
        return 0;
        break;
    }
}

uint8_t grains_get_bin_info(struct grains_context* context, struct grains_tag_info* tag_info, uint8_t variable_size, uint8_t endianess)
{
    if (!grains_range_within_bounds(context, GRAINS_TYPE_SIZES[variable_size]))
    {
        return 0;
    }

    if (endianess == GRAINS_UNKNOWN_ENDIAN)
    {
        return 0;
    }

    switch (GRAINS_TYPE_SIZES[variable_size])
    {
    case GRAINS_SIZE_8:
        tag_info->bin_size = (GRAINS_UINTPTR) * (uint8_t*)(context->buffer + context->index);
        tag_info->data = (uint8_t*)(context->buffer + context->index + 1);

        if (!grains_range_within_bounds(context, GRAINS_TYPE_SIZES[variable_size] + tag_info->bin_size))
        {
            return 0;
        }

        return 1;
        break;

#if UINTPTR_MAX >= 0xFFFFu
    case GRAINS_SIZE_16:
        tag_info->bin_size = (GRAINS_UINTPTR) * (uint16_t*)(context->buffer + context->index);
        tag_info->data = (uint8_t*)(context->buffer + context->index + 2);

        if ((endianess != GRAINS_UNKNOWN_ENDIAN) && (grains_get_endianess() != endianess))
        {
            tag_info->bin_size = (GRAINS_UINTPTR)*(uint16_t*)grains_bswap16(*(uint16_t*)tag_info->bin_size);
        }

        if (!grains_range_within_bounds(context, GRAINS_TYPE_SIZES[variable_size] + tag_info->bin_size))
        {
            return 0;
        }

        return 1;
        break;
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFu
    case GRAINS_SIZE_32:
        tag_info->bin_size = (GRAINS_UINTPTR) * (uint32_t*)(context->buffer + context->index);
        tag_info->data = (uint8_t*)(context->buffer + context->index + 4);

        if ((endianess != GRAINS_UNKNOWN_ENDIAN) && (grains_get_endianess() != endianess))
        {
            tag_info->bin_size = (GRAINS_UINTPTR)*(uint32_t*)grains_bswap16(*(uint32_t*)tag_info->bin_size);
        }

        if (!grains_range_within_bounds(context, GRAINS_TYPE_SIZES[variable_size] + tag_info->bin_size))
        {
            return 0;
        }

        return 1;
        break;
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFFFFFFFFFu
    case GRAINS_SIZE_64:
        tag_info->bin_size = (GRAINS_UINTPTR)*(uint64_t*)(context->buffer + context->index);
        tag_info->data = (uint8_t*)(context->buffer + context->index + 8);

        if ((endianess != GRAINS_UNKNOWN_ENDIAN) && (grains_get_endianess() != endianess))
        {
            tag_info->bin_size = (GRAINS_UINTPTR)*(uint64_t*)grains_bswap16(*(uint64_t*)tag_info->bin_size);
        }

        if (!grains_range_within_bounds(context, GRAINS_TYPE_SIZES[variable_size] + tag_info->bin_size))
        {
            return 0;
        }

        return 1;
        break;
#endif

    default:
        return 0;
        break;
    }
}

// This calculates the string length with the null byte.
void* grains_memchr(const void* str, uint8_t character, GRAINS_UINTPTR count)
{
    if (count != 0) {
        const unsigned char* p = str;

        do {
            if (*p++ == character)
                return ((void*)(p));
        } while (--count != 0);
    }
    return NULL;
}

uint8_t grains_try_read_next_tag(struct grains_context* context, struct grains_tag_info* tag_info)
{
    switch (context->format)
    {
    case GRAINS_ONE_BYTE:
        if (!grains_range_within_bounds(context, 1))
        {
            return 0;
        }

        GRAINS_UINTPTR start = (context->buffer + context->index);

        tag_info->format = GRAINS_ONE_BYTE;

        tag_info->tag = *(union grains_tag*)(context->buffer + context->index);
        context->index++;

        if (tag_info->tag.bits.has_name)
        {
            void* name_size = grains_memchr((context->buffer + context->index), '\0',
                (context->max_str_len > (context->buffer + context->size) - (context->buffer + context->index)) ? context->max_str_len : (context->buffer + context->size) - (context->buffer + context->index));

            if (name_size != NULL)
            {
                tag_info->name = (uint8_t*)(context->buffer + context->index);
                tag_info->name_size = name_size;
                context->index += (GRAINS_UINTPTR)name_size;
            }
            else
            {
                return 0;
            }
        }

        if (tag_info->tag.bits.type == GRAINS_TYPE_BIN)
        {
            GRAINS_UINTPTR size_tag_size = GRAINS_TYPE_SIZES[tag_info->tag.bits.size];
            if (!grains_range_within_bounds(context, size_tag_size))
            {
                return 0;
            }

            if (!grains_get_bin_info(context, tag_info, tag_info->tag.bits.size
                , tag_info->tag.bits.endianess))
            {
                return 0;
            }

            context->index += size_tag_size;
            context->index += tag_info->bin_size;

            tag_info->total_size = (context->buffer + context->index) - start;
            break;
        }

        GRAINS_UINTPTR size = GRAINS_TYPE_SIZES[tag_info->tag.bits.size];
        if (!grains_range_within_bounds(context, size))
        {
            return 0;
        }

        tag_info->data = (context->buffer + context->index + size);
        context->index += size;

        tag_info->total_size = (context->buffer + context->index) - start;
        break;

    case GRAINS_WITH_CLASS_BYTE:
        if (!grains_range_within_bounds(context, 2))
        {
            return 0;
        }

        GRAINS_UINTPTR start = (context->buffer + context->index);

        tag_info->format = GRAINS_WITH_CLASS_BYTE;

        tag_info->tag = *(union grains_tag*)(context->buffer + context->index);
        context->index++;

        tag_info->class_byte = *(uint8_t*)(context->buffer + context->index);
        context->index++;

        if (tag_info->tag.bits.has_name)
        {
            void* name_size = grains_memchr((context->buffer + context->index), '\0',
                (context->buffer + context->size) - (context->buffer + context->index));

            if (name_size != NULL)
            {
                tag_info->name = (uint8_t*)(context->buffer + context->index);
                tag_info->name_size = name_size;
                context->index += (GRAINS_UINTPTR)name_size;
            }
            else
            {
                return 0;
            }
        }
        

        if (tag_info->tag.bits.type == GRAINS_TYPE_BIN)
        {
            GRAINS_UINTPTR size_tag_size = GRAINS_TYPE_SIZES[tag_info->tag.bits.size];
            if (!grains_range_within_bounds(context, size_tag_size))
            {
                return 0;
            }

            if (!grains_get_bin_info(context, tag_info, tag_info->tag.bits.size
                , (tag_info->tag.bits.endianess)))
            {
                return 0;
            }

            context->index += size_tag_size;
            context->index += tag_info->bin_size;

            tag_info->total_size = (context->buffer + context->index) - start;
            break;
        }

        GRAINS_UINTPTR size = GRAINS_TYPE_SIZES[tag_info->tag.bits.size];
        if (!grains_range_within_bounds(context, size))
        {
            return 0;
        }

        tag_info->data = (context->buffer + context->index + size);
        context->index += size;

        tag_info->total_size = (context->buffer + context->index) - start;
        break;

    default:
        return 0;
        break;
    }

    context->type_counts[tag_info->tag.bits.type]++;
    context->size_counts[tag_info->tag.bits.size]++;

    return 1;
}

uint8_t grains_calculate_tag_size(struct grains_tag_info* tag_info)
{
    tag_info->total_size += 1;

    if (tag_info->format == GRAINS_WITH_CLASS_BYTE)
    {
        tag_info->total_size += 1;
    }

    if (tag_info->tag.bits.has_name)
    {
        // If the memory was zeroed around initialization, was it forgotten?
        // Or in general is the data valid?
        if (tag_info->name == 0)
        {
            return 0;
        }

        tag_info->total_size += tag_info->name_size;
    }

    if (tag_info->tag.bits.type == GRAINS_TYPE_BIN)
    {
        tag_info->total_size += GRAINS_TYPE_SIZES[tag_info->tag.bits.type] + tag_info->bin_size;
        return 1;
    }

    tag_info->total_size += GRAINS_TYPE_SIZES[tag_info->tag.bits.type];
    return 1;
}

uint8_t grains_try_write_tag(struct grains_context* context, struct grains_tag_info* tag_info)
{
    if ((grains_add_counts(context->size_counts, 8) + 1) > context->max_tag_count)
    {
        return 0;
    }

    if (!grains_calculate_tag_size(tag_info))
    {
        return 0;
    }

    if (!grains_range_within_bounds(context, tag_info->total_size))
    {
        return 0;
    }

    *(union grains_tag*)(context->buffer + context->index) = tag_info->tag;
    context->index++;

    if (context->format == GRAINS_WITH_CLASS_BYTE)
    {
        *(uint8_t*)(context->buffer + context->index) = tag_info->class_byte;
        context->index++;
    }

    if (tag_info->tag.bits.has_name)
    {
        if (tag_info->name_size > context->max_str_len)
        {
            return 0;
        }

        grains_memcpy(tag_info->name, (GRAINS_UINTPTR*)(context->buffer + context->index), tag_info->name_size);
        context->index += tag_info->name_size;
    }

    if (tag_info->tag.bits.type == GRAINS_TYPE_BIN)
    {
        switch (tag_info->tag.bits.size)
        {
        case GRAINS_SIZE_8:
            *(uint8_t*)(context->buffer + context->index) = (uint8_t)tag_info->bin_size;
            context->index += 1;
            grains_memcpy(tag_info->data, (uint8_t*)(context->buffer + context->index - 1), tag_info->bin_size);
            context->index += tag_info->bin_size;
            break;

#if UINTPTR_MAX >= 0xFFFFu
        case GRAINS_SIZE_16:
            *(uint16_t*)(context->buffer + context->index) = (uint16_t)tag_info->bin_size;
            context->index += 2;
            grains_memcpy(tag_info->data, (uint8_t*)(context->buffer + context->index - 2), tag_info->bin_size);
            context->index += tag_info->bin_size;
            break;
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFu
        case GRAINS_SIZE_32:
            *(uint32_t*)(context->buffer + context->index) = (uint32_t)tag_info->bin_size;
            context->index += 4;
            grains_memcpy(tag_info->data, (uint8_t*)(context->buffer + context->index - 4), tag_info->bin_size);
            context->index += tag_info->bin_size;
            break;
#endif

#if UINTPTR_MAX >= 0xFFFFFFFFFFFFFFFFu
        case GRAINS_SIZE_64:
            *(uint64_t*)(context->buffer + context->index) = (uint64_t)tag_info->bin_size;
            context->index += 8;
            grains_memcpy(tag_info->data, (uint8_t*)(context->buffer + context->index - 8), tag_info->bin_size);
            context->index += tag_info->bin_size;
            break;
#endif

        default:
            return 0;
            break;
        }
    }

    grains_memcpy(tag_info->data, *(uint8_t*)(context->buffer + context->index), GRAINS_TYPE_SIZES[tag_info->tag.bits.size]);
    context->index += GRAINS_TYPE_SIZES[tag_info->tag.bits.size];

    context->type_counts[tag_info->tag.bits.type]++;
    context->size_counts[tag_info->tag.bits.size]++;

    return 1;
}

uint8_t grains_compare_tag(struct grains_tag_info* tag_info, struct grains_match_tag_info* match_tag_info)
{
    if (match_tag_info->match_format != 2)
    {
        if (match_tag_info->format != tag_info->format)
        {
            return 0;
        }
    }

    if (match_tag_info->match_endianess != 2)
    {
        if (match_tag_info->endianess != tag_info->endianess)
        {
            return 0;
        }
    }

    if (match_tag_info->match_tag_type != 2)
    {
        if (match_tag_info->tag.bits.type != tag_info->tag.bits.type)
        {
            return 0;
        }
    }
    if (match_tag_info->match_tag_size != 2)
    {
        if (match_tag_info->tag.bits.size != tag_info->tag.bits.size)
        {
            return 0;
        }
    }
    if (match_tag_info->match_tag_has_name != 2)
    {
        if (match_tag_info->tag.bits.has_name != tag_info->tag.bits.type)
        {
            return 0;
        }
    }
    if (match_tag_info->match_tag_endianess != 2)
    {
        if (match_tag_info->tag.bits.endianess != tag_info->tag.bits.endianess)
        {
            return 0;
        }
    }

    if (match_tag_info->match_class != 2)
    {
        if (match_tag_info->class_byte != tag_info->class_byte)
        {
            return 0;
        }
    }

    if (match_tag_info->match_name_size != 2)
    {
        if (match_tag_info->name_size != tag_info->name_size)
        {
            return 0;
        }
    }
    if (match_tag_info->match_name != 2)
    {
        if (!grains_memcmp(match_tag_info->name, tag_info->name, match_tag_info->name_size))
        {
            return 0;
        }
    }

    if (match_tag_info->match_bin_size != 2)
    {
        if (match_tag_info->bin_size != tag_info->bin_size)
        {
            return 0;
        }
    }
    if (match_tag_info->match_data != 2)
    {
        if ((match_tag_info->tag.bits.type == GRAINS_TYPE_BIN != match_tag_info->tag.bits.type == GRAINS_TYPE_BIN)
            && (match_tag_info->bin_size != tag_info->bin_size))
        {
            if (!grains_memcmp(match_tag_info->data, tag_info->data, match_tag_info->bin_size))
            {
                return 0;
            }
            return 0;
        }
    }

    if (match_tag_info->match_total_size != 2)
    {
        if (match_tag_info->total_size != tag_info->total_size)
        {
            return 0;
        }
    }

    return 1;
}

uint8_t grains_list_all_tags(struct grains_context* context, struct grains_tag_info* tag_info, GRAINS_UINTPTR* tag_count, GRAINS_UINTPTR tag_limit)
{
    struct grains_context context_copy = { 0 };
    grains_memcpy((void*)context, (void*)&context_copy, sizeof(context_copy));

    struct grains_tag_info current_tag = { 0 };

    do
    {
        *tag_count++;

        if (tag_info)
        {
            grains_memcpy((void*)&current_tag, (void*)&tag_info[*tag_count], sizeof(struct grains_tag_info));
        }
    } while ((*tag_count < tag_limit) && (grains_try_read_next_tag(&context_copy, &current_tag) == 1));

    return (*tag_count > 0) ? 1 : 0;
}

uint8_t grains_search(struct grains_context* context, struct grains_match_tag_info* search_query, struct grains_tag_info* tag_info,
    GRAINS_UINTPTR range_start, GRAINS_UINTPTR range_end, GRAINS_UINTPTR* results, GRAINS_UINTPTR result_count)
{
    uint8_t count_results = 0;
    if (!results)
    {
        count_results = 1;
    }

    uint8_t found_a_match = 0;

    for (GRAINS_UINTPTR i = range_start; i < range_end; i++)
    {
        if (!grains_compare_tag(&tag_info[i], search_query))
        {
            continue;
        }

        found_a_match = 1;

        if (!results)
        {
            result_count++;
        }
        else
        {
            results[i] = &tag_info[i];
        }
    }

    return (found_a_match) ? 1 : 0;
}