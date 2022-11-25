#include <iostream>

//********************************************************************************************************************//
//                                          2.1. Следующий элемент                                                    //
// Дан отсортированный массив целых чисел A[0..n-1] и массив целых чисел B[0..m-1]. Для каждого элемента              //
// массива B[i] найдите минимальный индекс k минимального элемента массива A, равного или превосходящего              //
// B[i]: A[k] >= B[i]. Если такого элемента нет, выведите n. Время работы поиска k для каждого элемента               //
// B[i]: O(log(k)). n, m ≤ 10000.                                                                                     //
//                                                                                                                    //
//Формат ввода                                                                                                        //
//В первой строчке записаны числа n и m. Во второй и третьей массивы A и B соответственно.                            //
//********************************************************************************************************************//

int binarySearch(const int32_t *, uint16_t, uint16_t, int32_t);
int exponentialSearch(const int32_t *, uint16_t, int32_t);

int binarySearch(const int32_t *arr, uint16_t first, uint16_t last, const int32_t target) {
    for (; first != last;) {
        int middle = (first + last) / 2;
        if (arr[middle] < target) first = middle + 1;
        else last = middle;
    }
    return first;
}

int exponentialSearch(const int32_t *arr, const uint16_t size, const int32_t x) {
    if (arr[0] == x) return 0;
    if (arr[size - 1] < x) return size;
    if (size == 0) return 0;

    uint16_t i = 1;
    for (; i < size && arr[i] < x;)
        i = i * 2;

    return binarySearch(arr, i / 2, std::min(i, size), x);
}


int main() {
    uint16_t sortedArraySize = 0;
    uint16_t arraySize = 0;

    std::cin >> sortedArraySize >> arraySize;

    auto *sortedArray = new int32_t[sortedArraySize];
    auto *array = new int32_t[arraySize];

    for (size_t i = 0; i < sortedArraySize; ++i)
        std::cin >> sortedArray[i];

    for (size_t i = 0; i < arraySize; ++i)
        std::cin >> array[i];

    for (size_t i = 0; i < arraySize; ++i)
        std::cout << exponentialSearch(sortedArray, sortedArraySize, array[i]) << " ";

    delete[] sortedArray;
    delete[] array;

    return 0;
}