#include <gtest/gtest.h>

#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string.h>

#include "base85ed.h"

const std::vector<std::pair<const char *, const char * >> short_cases =
{
    { "",     ""     },
    { "F#",   "1"    },
    { "F){",  "12"   },
    { "F)}j", "123"  },
    { "F)}kW","1234" }
};

static std::vector<uint8_t> cstr2v(const char *s)
{
    return std::vector<uint8_t>(
               s,
               s + std::string(s).size()
           );
}

// Тесты encode
TEST(Base85ShortsEncode, TrivialShortEncodes)
{
    for (const auto &p : short_cases)
    {
        EXPECT_EQ(base85::encode(cstr2v(p.second)), cstr2v(p.first));
    }
}

// Тесты decode
TEST(Base85ShortsDecode, TrivialShortDecodes)
{
    for (const auto &p : short_cases)
    {
        EXPECT_EQ(base85::decode(cstr2v(p.first)), cstr2v(p.second));
    }
}

// 1. Проверка на полных блоках (кратных 4 байтам) и длинных строках
TEST(Base85Extended, FullBlocksAndSentences)
{
    // Строка "Hello, World!!" (14 байт) -> остаток 2 байта
    const char* plain = "Hello, World!!";
    // Результат b85encode в Python: b'NM&qnZ!92JZ*pv8At3'
    const char* encoded = "NM&qnZ!92JZ*pv8At3";

    EXPECT_EQ(base85::encode(cstr2v(plain)), cstr2v(encoded));
    EXPECT_EQ(base85::decode(cstr2v(encoded)), cstr2v(plain));
}

// 2. Тестирование крайних бинарных значений (все нули и все 255)
TEST(Base85Extended, BinaryEdgeCases)
{
    // 4 нулевых байта превращаются в "00000"
    std::vector<uint8_t> zeroes = {0, 0, 0, 0};
    std::vector<uint8_t> zeroes_enc = {'0', '0', '0', '0', '0'};
    EXPECT_EQ(base85::encode(zeroes), zeroes_enc);
    EXPECT_EQ(base85::decode(zeroes_enc), zeroes);

    // 4 байта 0xFF превращаются в "|NsC0"
    std::vector<uint8_t> max_bytes = {0xFF, 0xFF, 0xFF, 0xFF};
    std::vector<uint8_t> max_bytes_enc = {'|', 'N', 's', 'C', '0'};
    EXPECT_EQ(base85::encode(max_bytes), max_bytes_enc);
    EXPECT_EQ(base85::decode(max_bytes_enc), max_bytes);
}

// 3. Круговой тест на случайных данных
TEST(Base85Extended, RandomDataRoundTrip)
{
    std::vector<uint8_t> random_data(257); // Специально не кратно 4
    for (size_t i = 0; i < random_data.size(); ++i)
    {
        random_data[i] = static_cast<uint8_t>((i * 13) % 256);
    }

    // Кодируем и сразу декодируем обратно
    std::vector<uint8_t> enc = base85::encode(random_data);
    std::vector<uint8_t> dec = base85::decode(enc);

    // Исходные данные должны идеально совпадать с декодированными
    EXPECT_EQ(random_data, dec);
}

// 4. Проверка обработки ошибок (Валидация некорректного ввода)
TEST(Base85Extended, InvalidInputsException)
{
    // Запрещенный символ для RFC 1924 Base85 (например, пробел или перевод строки)
    std::vector<uint8_t> invalid_chars = {'F', ' ', 'j'};
    EXPECT_THROW(base85::decode(invalid_chars), std::runtime_error);

    // Некорректная длина финального остатка (остаток в 1 символ невозможен по стандарту)
    std::vector<uint8_t> invalid_len = {'F'};
    EXPECT_THROW(base85::decode(invalid_len), std::runtime_error);

    std::vector<uint8_t> invalid_len_block = {'F', ')', '}', 'k', 'W', 'X'}; // 6 символов вместо 5
    // При обработке блока 5 съедятся нормально, а на 6-м (длина 1) вылетит исключение
    EXPECT_THROW(base85::decode(invalid_len_block), std::runtime_error);
}