#ifndef SORTING_H
#define SORTING_H

// Header-only

// Для swap
#include "collvalue.h"

// ----------------------------- Пузырёк (например) --------------------------------------
template <typename Iterator>
void bubble_sort(Iterator begin, Iterator end)
{
    if (begin == end)
    {
        return;
    }
    bool swapped = true;
    while (swapped)
    {
        swapped = false;
        for (Iterator it = begin; std::next(it) != end; ++it)
        {
            if (*std::next(it) < *it)
            {
                std::iter_swap(it, std::next(it));
                swapped = true;
            }
        }
    }
}

// --------------------------- QuickSort (например) ---------------------------------------
template <typename Iterator>
void quick_sort(Iterator begin, Iterator end)
{
    // чтобы не возникло риска бесконечной рекурсии
    if (begin == end || std::next(begin) == end)
    {
        return;
    }

    auto pivot = *std::prev(end);
    auto i = begin;
    for (auto j = begin; j != std::prev(end); ++j)
    {
        if (*j < pivot)
        {
            std::iter_swap(i, j);
            ++i;
        }
    }
    std::iter_swap(i, std::prev(end));
    Iterator pivot_it = i;

    quick_sort(begin, pivot_it);
    quick_sort(std::next(pivot_it), end);
}

#endif // SORTING_H
