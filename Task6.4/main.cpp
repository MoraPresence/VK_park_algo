#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

//********************************************************************************************************************//
//                                   6.4 Порядковая статистика и параметры множества                                  //
//Дано множество целых чисел из [0..10^9] размера n.                                                                  //
//Используя алгоритм поиска k-ой порядковой статистики, требуется найти следующие параметры множества:                //
//10%  перцентиль                                                                                                     //
//        медиана                                                                                                     //
//90%  перцентиль                                                                                                     //
//                                                                                                                    //
//Требования: к дополнительной памяти: O(n).                                                                          //
//Среднее время работы: O(n)                                                                                          //
//Должна быть отдельно выделенная функция partition.                                                                  //
//Рекурсия запрещена.                                                                                                 //
//Решение должно поддерживать передачу функции сравнения снаружи.                                                     //
//                                                                                                                    //
//Реализуйте стратегию выбора опорного элемента “случайный элемент”.                                                  //
//Функцию Partition реализуйте методом прохода двумя итераторами от конца массива к началу.                           //
//********************************************************************************************************************//

template<typename T>
struct [[maybe_unused]] defaultComparator {
    bool operator()(const T &l, const T &r) {
        return l < r;
    }
};

template<typename T, class Comparator = defaultComparator<T>>
class OrderStatistics {
    std::vector<T> arr;
    uint32_t size;

    Comparator cmp = Comparator();

    size_t selectPivot(const int32_t &, const int32_t &);

    [[maybe_unused]] bool le(const T &, const T &);

    bool ge(const T &, const T &);

    [[maybe_unused]] bool eq(const T &, const T &);

public:
    OrderStatistics() = delete;

    explicit OrderStatistics(uint32_t);

    OrderStatistics(const std::vector<T> &, const uint32_t &);

    ~OrderStatistics() { arr.clear(); }

    size_t partition(const int32_t &, const int32_t &);

    T getOrderStat(uint32_t, int32_t, int32_t);

    void fillArr();
};

template<typename T, class Comparator>
OrderStatistics<T, Comparator>::OrderStatistics(uint32_t _size) {
    size = _size;
    arr.reserve(_size);
}

template<typename T, class Comparator>
OrderStatistics<T, Comparator>::OrderStatistics(const std::vector<T> &_arr, const uint32_t &_size) {
    size = _size;
    arr = _arr;
}

template<typename T, class Comparator>
size_t OrderStatistics<T, Comparator>::selectPivot(const int32_t &start, const int32_t &end) {
    srand(time(nullptr));
    return start + rand() % (end - start);
}

template<typename T, class Comparator>
size_t OrderStatistics<T, Comparator>::partition(const int32_t &start, const int32_t &end) {

    if (start != end) {
        auto pivot = selectPivot(start, end);
        std::swap(arr[pivot], arr[start]);
    }

    auto i = end + 1;
    for (auto j = end; j >= start; --j) {
        if (ge(arr[j], arr[start]))
            std::swap(arr[--i], arr[j]);
    }
    return i;
}

template<typename T, class Comparator>
T OrderStatistics<T, Comparator>::getOrderStat(const uint32_t k, int32_t start, int32_t end) {
    int32_t pivotPos = 0;
    for (; start <= end;) {
         pivotPos = partition(start, end);

        if (pivotPos < k)
            start = pivotPos + 1;
        else if (pivotPos > k)
            end = pivotPos - 1;
        else
            return arr[k];
    }
    return arr[pivotPos];
}

template<typename T, class Comparator>
void OrderStatistics<T, Comparator>::fillArr() {
    std::stringstream ss;
    std::string input;
    std::getline(std::cin, input);
    ss << input;

    for (int i = 0; i < size; ++i) {
        T num = 0;
        ss >> num;
        arr.emplace_back(num);
    }
    ss.clear();
}

template<typename T, class Comparator>
bool OrderStatistics<T, Comparator>::le(const T &l, const T &r) {
    return (!cmp(r, l));
}

template<typename T, class Comparator>
bool OrderStatistics<T, Comparator>::ge(const T &l, const T &r) {
    return (!cmp(l, r));
}

template<typename T, class Comparator>
bool OrderStatistics<T, Comparator>::eq(const T &l, const T &r) {
    return (!cmp(l, r) && !cmp(r, l));
}

int main() {
    std::stringstream ss;
    std::string input;

    std::getline(std::cin, input);
    ss << input;

    int32_t arrSize = 0;
    ss >> arrSize;
    ss.clear();

    OrderStatistics<uint32_t> obj(arrSize);

    obj.fillArr();

    int32_t k10 = (arrSize * 10) / 100;
    int32_t k50 = (arrSize * 50) / 100;
    int32_t k90 = (arrSize * 90) / 100;

    std::cout << obj.getOrderStat(k10, 0, arrSize - 1) << std::endl;
    std::cout << obj.getOrderStat(k50, 0, arrSize - 1) << std::endl;
    std::cout << obj.getOrderStat(k90, 0, arrSize - 1) << std::endl;

    return 0;
}
