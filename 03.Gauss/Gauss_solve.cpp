#include "Gauss_solve.h"
#include <cmath>
#include <stdexcept>

GaussVector Gauss_solve(GaussMatrix &ab)
{
    const size_t n = ab.rows();

    // --- ПРЯМОЙ ХОД (Приведение к верхнетреугольному виду) ---
    for (size_t row = 0; row < n; ++row)
    {
        // 1. Поиск главного элемента в текущем столбце (выбор строки)
        size_t pivot_row = row;
        double max_val = std::abs(ab(row, row));

        for (size_t i = row + 1; i < n; ++i)
        {
            double current_val = std::abs(ab(i, row));
            if (current_val > max_val)
            {
                max_val = current_val;
                pivot_row = i;
            }
        }

        // 2. Проверка системы на вырожденность (единственность решения)
        if (max_val == 0)
        {
            throw std::runtime_error("Matrix is singular or has infinite solutions.");
        }

        // 3. Перестановка текущей строки с ведущей строкой
        if (pivot_row != row)
        {
            for (size_t j = row; j <= n; ++j)
            {
                std::swap(ab(pivot_row, j), ab(row, j));
            }
        }

        // 4. Исключение элементов под главной диагональю
        for (size_t i = row + 1; i < n; ++i)
        {
            double factor = ab(i, row) / ab(row, row);
            for (size_t j = row; j <= n; ++j)
            {
                ab(i, j) -= factor * ab(row, j);
            }
        }
    }

    // --- ОБРАТНЫЙ ХОД (Вычисление вектора неизвестных) ---
    GaussVector x(n);

    for (int i = static_cast<int>(n) - 1; i >= 0; --i)
    {
        double sum = 0.0;
        for (size_t j = i + 1; j < n; ++j)
        {
            sum += ab(i, j) * x[j];
        }

        // ab(i, n) — это свободный член b[i] из расширенной матрицы
        x[i] = (ab(i, n) - sum) / ab(i, i);
    }

    return x;
}
