#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>
#include <cmath>

#include "Gauss_solve.h"

// Вспомогательная функция для проверки близости элементов вектора к эталону
bool is_vector_close(const GaussVector& x, const std::vector<double>& expected, double epsilon = 1e-6)
{
    if (static_cast<size_t>(x.rows()) != expected.size())
    {
        return false;
    }
    for (size_t i = 0; i < expected.size(); ++i)
    {
        if (std::abs(x[i] - expected[i]) > epsilon)
        {
            return false;
        }
    }
    return true;
}

// --- 1. Простейший базовый тест (Система 2х2) ---
// 2x + 1y = 5
// 1x - 3y = -1
// Решение: x = 2, y = 1
TEST(GaussSolve, TrivialSystem2x2)
{
    GaussMatrix ab(2, 3);

    // Заполняем первую строку: 2x + 1y = 5
    ab(0, 0) = 2.0;
    ab(0, 1) = 1.0;
    ab(0, 2) = 5.0;

    // Заполняем вторую строку: 1x - 3y = -1
    ab(1, 0) = 1.0;
    ab(1, 1) = -3.0;
    ab(1, 2) = -1.0;

    GaussVector x = Gauss_solve(ab);

    ASSERT_EQ(x.rows(), 2);
    EXPECT_TRUE(is_vector_close(x, {2.0, 1.0}));
}

// --- 2. Более сложная система (3х3) ---
//  1x +  1y + 1z = 6
//  0x +  2y + 5z = -4
//  2x +  5y - 1z = 27
// Решение: x = 5, y = 3, z = -2
TEST(GaussSolve, NormalSystem3x3)
{
    GaussMatrix ab(3, 4);

    ab(0, 0) = 1.0;
    ab(0, 1) = 1.0;
    ab(0, 2) = 1.0;
    ab(0, 3) = 6.0;
    ab(1, 0) = 0.0;
    ab(1, 1) = 2.0;
    ab(1, 2) = 5.0;
    ab(1, 3) = -4.0;
    ab(2, 0) = 2.0;
    ab(2, 1) = 5.0;
    ab(2, 2) = -1.0;
    ab(2, 3) = 27.0;

    GaussVector x = Gauss_solve(ab);

    ASSERT_EQ(x.rows(), 3);
    EXPECT_TRUE(is_vector_close(x, {5.0, 3.0, -2.0}));
}

// --- 3. Тест на систему, где ведущий элемент равен нулю ---
//  0x + 2y = 4
//  3x - 1y = 1
// Решение: x = 1, y = 2
TEST(GaussSolve, ZeroPivotRowSwap)
{
    GaussMatrix ab(2, 3);

    ab(0, 0) = 0.0;
    ab(0, 1) = 2.0;
    ab(0, 2) = 4.0;
    ab(1, 0) = 3.0;
    ab(1, 1) = -1.0;
    ab(1, 2) = 1.0;

    GaussVector x = Gauss_solve(ab);

    ASSERT_EQ(x.rows(), 2);
    EXPECT_TRUE(is_vector_close(x, {1.0, 2.0}));
}

// --- 4. Проверка обработки вырожденной матрицы ---
TEST(GaussSolve, SingularMatrixException)
{
    GaussMatrix ab(2, 3);

    ab(0, 0) = 1.0;
    ab(0, 1) = 2.0;
    ab(0, 2) = 3.0;
    ab(1, 0) = 2.0;
    ab(1, 1) = 4.0;
    ab(1, 2) = 6.0;

    EXPECT_THROW(Gauss_solve(ab), std::runtime_error);
}
