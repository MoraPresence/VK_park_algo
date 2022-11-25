#include <iostream>
#include <sstream>
#include <cstring>

//********************************************************************************************************************//
//                                           4.2. Топ K пользователей из лога                                         //
//Имеется лог-файл, в котором хранятся пары для N пользователей (Идентификатор пользователя, посещаемость сайта).     //
//Напишите программу, которая выбирает K пользователей, которые чаще других заходили на сайт,                         //
// и выводит их в порядке возрастания посещаемости.                                                                   //
//Количество заходов и идентификаторы пользователей не повторяются.                                                   //
//                                                                                                                    //
//Требования:                                                                                                         //
//Время работы O(N * logK)                                                                                            //
//Куча должна быть реализована в виде шаблонного класса.                                                              //
//Решение должно поддерживать передачу функции сравнения снаружи.                                                     //
//Куча должна быть динамической.                                                                                      //
//Формат ввода                                                                                                        //
//Сначала вводятся N и K, затем пары (Идентификатор пользователя, посещаемость сайта).                                //
//                                                                                                                    //
//Формат вывода                                                                                                       //
//Идентификаторы пользователей в порядке возрастания посещаемости.                                                    //
//********************************************************************************************************************//

/*
 *нужно дорабатывать (Требования: время работы O(N * logK), где N - кол-во пользователей. Ограничение на размер кучи O(K)).
- куча должна быть универсальной, не привязанной к задаче. У вас же появился еще один шаблонный тип К, который не имеет смысла,
 если решим хранить в куче просто int, например. В качестве T должна выступать вся структура из двух полей (user id и частотность),
 компаратор должен уметь сравнивать два экземпляра такой структуры, куча ничего про поля структуры знать не должна.
 Эту структуру нужно объявлять вне кучи, она не является ее неотъемлемой частью.(OKEY)

- методы ExtractMaxId и PeekMaxId также привязаны к условию задачи, надо их переименовать(OKEY)

- нельзя зачитывать в кучу сразу весь ввод (ограничение на размер кучи O(K)). Решить эту проблему можно так:
 строим minHeap по значению частотности (в вершине пользователь с минимальным значением частотности).
 Сначала зачитываем в кучу первые K строк, затем для каждой новой строки: добавляем ее в кучу,
 если размер кучи >K, выкидываем лишние элементы. В итоге получим кучу из топ K элементов.(Okey)

- В C++ нужно быть аккуратным с memcpy и memset. Они не вызывают конструктор или оператор присваивания.
 Если у копируемого типа нетривиальная логика копирования, получите баг.(OKEY???)
 */

#define DEFAULT_SIZE 2

template<typename T>
class DynamicArray {
public:
    int32_t size;
    size_t capacity;
    T *cells;

    void resize();

public:
    DynamicArray() : size(0), capacity(DEFAULT_SIZE) { cells = new T[capacity]; }

    virtual ~DynamicArray() { delete[] cells; }

    void add(const T &);

    [[nodiscard]] size_t getSize() const { return size; }

    [[nodiscard]] size_t getCapacity() const { return capacity; }

    T deleteLast();

    T &last() { return cells[size - 1]; }

    [[nodiscard]] bool isEmpty() const { return size == 0; }

    T &operator[](uint32_t);
};

template<typename T>
void DynamicArray<T>::resize() {
    auto toDelete = cells;

    capacity *= 2;
    cells = new T[capacity];

    for (int i = 0; i < size; ++i)
        cells[i] = toDelete[i];

    delete[] toDelete;
}

template<typename T>
void DynamicArray<T>::add(const T &item) {
    if (size == capacity) resize();
    cells[size] = item;
    size++;
}

template<typename T>
[[maybe_unused]] T DynamicArray<T>::deleteLast() {
    if (isEmpty()) {
        throw std::runtime_error("Array is empty");
    }
    size--;
    return cells[size];
}

template<typename T>
T &DynamicArray<T>::operator[](uint32_t index) {
    if (index < 0 || index >= size) {
        throw std::out_of_range("Out of range");
    }
    return cells[index];
}

template<typename T>
struct [[maybe_unused]] defaultComparator {
    bool operator()(const T &l, const T &r) {
        return l < r;
    }
};

struct HeapNode {
    uint32_t value;
    int32_t id;
} typedef HeapNode;

struct ComparatorMaxHeapNode {
    bool operator()(const HeapNode &l, const HeapNode &r) {
        return l.value < r.value;
    }
};

struct ComparatorMinHeapNode {
    bool operator()(const HeapNode &l, const HeapNode &r) {
        return l.value > r.value;
    }
};

template<typename T, class Comparator = defaultComparator<T>>
class Heap {
public:
    DynamicArray<T> array;

    [[maybe_unused]] void buildHeap();

    void siftDown(uint32_t);

    void siftUp(uint32_t);

    Comparator cmp = Comparator();

    [[maybe_unused]] bool le(const T &l, const T &r);

    bool ge(const T &l, const T &r);

    [[maybe_unused]] bool eq(const T &, const T &r);


public:
    Heap();

    ~Heap() = default;

    void Insert(T);

    T ExtractRoot();

    [[maybe_unused]] [[nodiscard]] T PeekRoot() const;

    int32_t getSize() { return array.size; }

};

template<typename T, class Comparator>
Heap<T, Comparator>::Heap() {
    array.capacity = DEFAULT_SIZE;
    array.size = 0;

    array.cells = new T[DEFAULT_SIZE];
}

template<typename T, class Comparator>
[[maybe_unused]] void Heap<T, Comparator>::buildHeap() {
    for (int i = (array.getSize() / 2) - 1; i >= 0; --i) {
        siftDown(i);
    }
}

template<typename T, class Comparator>
void Heap<T, Comparator>::siftUp(uint32_t index) {
    for (; index > 0;) {
        auto parent = (index - 1) / 2;
        if (le(array.cells[index], array.cells[parent])) return;

        std::swap(array.cells[index], array.cells[parent]);
        index = parent;
    }
}

template<typename T, class Comparator>
void Heap<T, Comparator>::siftDown(uint32_t index) {
    auto left = 2 * index + 1;
    auto right = 2 * index + 2;

    auto largest = index;
    if (left < array.getSize() && ge(array.cells[left], array.cells[index])) largest = left;
    if (right < array.getSize() && ge(array.cells[right], array.cells[largest])) largest = right;

    if (largest != index) {
        std::swap(array.cells[index], array.cells[largest]);
        siftDown(largest);
    }
}


template<typename T, class Comparator>
[[maybe_unused]] void Heap<T, Comparator>::Insert(T node) {
    if (array.getSize() == array.getCapacity()) array.resize();
    array.add(node);
    siftUp(array.getSize() - 1);
}

template<typename T, class Comparator>
T Heap<T, Comparator>::ExtractRoot() {
    auto result = array.cells[0];

    array.cells[0] = array.last();
    array.deleteLast();

    if (!array.isEmpty()) {
        siftDown(0);
    }

    return result;
}

template<typename T, class Comparator>
[[maybe_unused]] T Heap<T, Comparator>::PeekRoot() const {
    return array.cells[0];
}

template<typename T, class Comparator>
[[maybe_unused]] bool Heap<T, Comparator>::le(const T &l, const T &r) {
    return (!cmp(r, l));
}

template<typename T, class Comparator>
bool Heap<T, Comparator>::ge(const T &l, const T &r) {
    return (!cmp(l, r));
}

template<typename T, class Comparator>
[[maybe_unused]] bool Heap<T, Comparator>::eq(const T &l, const T &r) {
    return (!cmp(l, r) && !cmp(r, l));
}


void testingHeap(Heap<HeapNode, ComparatorMinHeapNode> &heap, const unsigned countUsers, const unsigned countTopUsers,
                 std::stringstream &ss) {
    if (countTopUsers == 0) return;
    for (size_t i = 0; i < countUsers; ++i) {//O(n*log(n))
        int32_t userId = 0;
        uint32_t value = 0;
        std::string input;

        std::getline(std::cin, input);
        ss << input;
        ss >> userId;
        ss >> value;

        heap.Insert(HeapNode{value, userId});

        if (heap.getSize() > countTopUsers){
            heap.ExtractRoot();
        }

        ss.clear();
    }

    auto resultArr = new int32_t[countTopUsers];

    for (int i = 0; i < countTopUsers; ++i) {
        resultArr[i] = heap.ExtractRoot().id;
    }

    for (int i = 0; i < countTopUsers; ++i) {
        std::cout << resultArr[i] << " ";
    }
}


int main() {
    std::stringstream ss;
    std::string input;

    std::getline(std::cin, input);
    ss << input;

    unsigned countUsers = 0;
    unsigned countTopUsers = 0;
    ss >> countUsers >> countTopUsers;
    ss.clear();

    Heap<HeapNode, ComparatorMinHeapNode> heap;

    testingHeap(heap, countUsers, countTopUsers, ss);

    return 0;
}
