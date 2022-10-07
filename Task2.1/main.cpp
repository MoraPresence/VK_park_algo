#include <iostream>
#include <algorithm>
#include <sstream>

//********************************************************************************************************************//
//                                          Задача № 2 (4 балла)                                                      //
//2_1. Дан отсортированный массив целых чисел A[0..n-1] и массив целых чисел B[0..m-1]. Для каждого элемента массив   //
//а B[i] найдите минимальный индекс k минимального элемента массива A, равного или превосходящего B[i]: A[k] >= B[i]. //
//Если такого элемента нет, выведите n. n, m ≤ 10000.                                                                 //
//Требования:  Время работы поиска k для каждого элемента B[i]: O(log(k)). Внимание! В этой задаче для каждого B[i]   //
//сначала нужно определить диапазон для бинарного поиска размером порядка k с помощью экспоненциального поиска,       //
//а потом уже в нем делать бинарный поиск.                                                                            //
//                                                                                                                    //
//Формат входных данных.                                                                                              //
//В первой строчке записаны числа n и m. Во второй и третьей массивы A и B соответственно.                            //
//********************************************************************************************************************//

int binarySearch(const unsigned *arr, int first, int last, const unsigned target) {
    for (; last - first > 1;) {
        int middle = (first + last) / 2;
        if (arr[middle] < target)
            first = middle + 1;
        else
            last = middle;
    }
    if(first == last && first > 0) first--;
    return (arr[last] >= target && arr[first] < target) ? last : first;
}

int exponentialSearch(const unsigned *arr, const int n, const unsigned x) {
    if (arr[0] == x)
        return 0;
    if (arr[n - 1] < x){
        return n;
    }

    int i = 1;
    for (; i < n && arr[i] < x;)
        i = i * 2;

    return binarySearch(arr, i / 2, std::min(i, n), x);
}

int fillArray(std::stringstream &ss, unsigned *array, const uint16_t len) {
    std::string input;
    std::getline(std::cin, input);
    ss << input;

    for (size_t i = 0; i < len; ++i) {
        ss >> array[i];
    }
    return 0;
}

int main() {
    std::stringstream ss;
    std::string input;

    std::getline(std::cin, input);
    ss << input;

    int sizeA = 0;
    int sizeB = 0;
    ss >> sizeA;
    ss >> sizeB;
    ss.clear();

    auto arrayA = new unsigned[sizeA];
    auto arrayB = new unsigned[sizeB];

    fillArray(ss, arrayA, sizeA);
    ss.clear();
    fillArray(ss, arrayB, sizeB);
    ss.clear();

    for (int i = 0; i < sizeB; ++i) {
        std::cout << exponentialSearch(arrayA, sizeA, arrayB[i]) << " ";
    }

    delete[] arrayA;
    delete[] arrayB;

    return 0;
}
