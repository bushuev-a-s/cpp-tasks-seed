#include <gtest/gtest.h>
#include <vector>
#include <list>

// А вот тут collvalue.h не надо, берите просто инты
#include "sorting.h"

// Но проверьте, что сортировки таки работают...

// --- Вспомогательный шаблон для проверки сортировки контейнера ---
template <typename Container>
void BaseSortTest(const Container& input)
{
    // Проверка для Bubble Sort
    Container c1 = input;
    bubble_sort(c1.begin(), c1.end());
    EXPECT_TRUE(std::is_sorted(c1.begin(), c1.end()));
    EXPECT_EQ(c1.size(), input.size());

    // Проверка для Quick Sort
    Container c2 = input;
    quick_sort(c2.begin(), c2.end());
    EXPECT_TRUE(std::is_sorted(c2.begin(), c2.end()));
    EXPECT_EQ(c2.size(), input.size());
}

// --- Набор тестов (Test Suite) ---

// 1. Тест на пустой коллекции
TEST(SortingTest, EmptyContainer)
{
    std::vector<int> empty_vec;
    BaseSortTest(empty_vec);
}

// 2. Тест на одном элементе
TEST(SortingTest, SingleElement)
{
    std::vector<int> single = { 42 };
    BaseSortTest(single);
}

// 3. Тест на уже отсортированном массиве
TEST(SortingTest, AlreadySorted)
{
    std::vector<int> sorted = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    BaseSortTest(sorted);
}

// 4. Тест на массиве в обратном порядке
TEST(SortingTest, ReverseSorted)
{
    std::vector<int> reversed = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    BaseSortTest(reversed);
}

// 5. Тест на повторяющиеся элементы
TEST(SortingTest, DuplicateElements)
{
    std::vector<int> duplicates = { 5, 1, 5, 3, 2, 5, 1, 3, 2 };
    BaseSortTest(duplicates);
}

// 6. Тест на отрицательные и нулевые значения
TEST(SortingTest, NegativeAndZero)
{
    std::vector<int> mixed = { -10, 5, 0, -3, 2, -1, 0, 14 };
    BaseSortTest(mixed);
}

// 7. Тест на больших числах и больших объемах
TEST(SortingTest, LargeVector)
{
    std::vector<int> large(100);
    // Заполняем псевдослучайными числами
    int n = 0;
    std::generate(large.begin(), large.end(), [&n]()
    {
        return (n++ * 125) % 97;
    });
    BaseSortTest(large);
}

// 8. Тест на другом типе контейнера (std::list)
TEST(SortingTest, ListContainer)
{
    std::list<int> l = { 9, 3, 1, 7, 4, 2 };

    bubble_sort(l.begin(), l.end());
    EXPECT_TRUE(std::is_sorted(l.begin(), l.end()));

    // Пересоздаем список для QuickSort
    l = { 9, 3, 1, 7, 4, 2 };
    quick_sort(l.begin(), l.end());
    EXPECT_TRUE(std::is_sorted(l.begin(), l.end()));
}
