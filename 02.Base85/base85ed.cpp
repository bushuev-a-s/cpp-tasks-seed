#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "base85ed.h"

// Алфавит согласно RFC 1924 (используется в Python base64.b85encode)
static const char ALPHABET[] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";

// Обратная таблица для быстрого декодирования
static const int8_t DECODE_MAP[256] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, 62, -1, 63, 64, 65, 66, -1, 67, 68, 69, 70, -1, 71, -1, -1,
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, 72, 73, 74, 75, 76,
        77, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, 78, 79,
        80, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
        51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 81, 82, 83, 84, -1,
        // Все остальные значения заполняются -1 (для значений > 127)
    };


std::vector<uint8_t> base85::encode(std::vector<uint8_t> const &bytes)
{
    std::vector<uint8_t> out;
    size_t len = bytes.size();
    size_t i = 0;

    // Обрабатываем блоки по 4 байта -> превращаем в 5 символов
    for (; i + 3 < len; i += 4)
    {
        uint32_t val = ((uint32_t)bytes[i] << 24) |
                       ((uint32_t)bytes[i + 1] << 16) |
                       ((uint32_t)bytes[i + 2] << 8) |
                       (uint32_t)bytes[i + 3];

        uint8_t block[5];
        for (int j = 4; j >= 0; --j)
        {
            block[j] = ALPHABET[val % 85];
            val /= 85;
        }
        out.insert(out.end(), block, block + 5);
    }

    // Обработка остатка данных (дополнение нулями)
    if (i < len)
    {
        size_t rem = len - i;
        uint32_t val = 0;
        for (size_t j = 0; j < 4; ++j)
        {
            val <<= 8;
            if (i + j < len)
            {
                val |= bytes[i + j];
            }
        }

        uint8_t block[5];
        for (int j = 4; j >= 0; --j)
        {
            block[j] = ALPHABET[val % 85];
            val /= 85;
        }
        // Записываем только rem + 1 значащих символов
        out.insert(out.end(), block, block + rem + 1);
    }

    return out;
}


std::vector<uint8_t> base85::decode(std::vector<uint8_t> const &b85str)
{
    std::vector<uint8_t> out;
    size_t len = b85str.size();
    size_t i = 0;

    // Обрабатываем полные блоки по 5 символов -> восстанавливаем 4 байта
    for (; i + 4 < len; i += 5)
    {
        uint32_t val = 0;
        for (int j = 0; j < 5; ++j)
        {
            int8_t digit = DECODE_MAP[b85str[i + j]];
            if (digit < 0)
            {
                throw std::runtime_error("Invalid character in Base85 string");
            }
            val = val * 85 + digit;
        }

        out.push_back((val >> 24) & 0xFF);
        out.push_back((val >> 16) & 0xFF);
        out.push_back((val >> 8) & 0xFF);
        out.push_back(val & 0xFF);
    }

    // Обработка неполного финального блока
    if (i < len)
    {
        size_t rem = len - i;
        if (rem < 2)
        {
            throw std::runtime_error("Invalid Base85 string padding");
        }

        uint32_t val = 0;
        for (size_t j = 0; j < 5; ++j)
        {
            val *= 85;
            if (j < rem)
            {
                int8_t digit = DECODE_MAP[b85str[i + j]];
                if (digit < 0) throw std::runtime_error("Invalid character");
                val += digit;
            }
            else
            {
                val += 84; // Дополнение максимальным значением разряда (как в b85decode)
            }
        }

        // Извлекаем только значащие байты (rem - 1)
        for (size_t j = 0; j < rem - 1; ++j)
        {
            out.push_back((val >> (24 - j * 8)) & 0xFF);
        }
    }

    return out;
}
