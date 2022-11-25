//********************************************************************************************************************//
//                                                2.1. Хеш-таблица                                                    //
// Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией.       //
// Хранимые строки непустые и состоят из строчных латинских букв.                                                     //
// Хеш-функция строки должна быть реализована с помощью вычисления значения многочлена методом Горнера.               //
// Начальный размер таблицы должен быть равным 8-ми. Перехеширование выполняйте при добавлении элементов в случае,    //
// когда коэффициент заполнения таблицы достигает 3/4.                                                                //
// Структура данных должна поддерживать операции добавления строки в множество,                                       //
// удаления строки из множества и проверки принадлежности данной строки множеству.                                    //
//                                                                                                                    //
// 1_1. Для разрешения коллизий используйте квадратичное пробирование. i-ая проба                                     //
// g(k, i)=g(k, i-1) + i (mod m). m - степень двойки.                                                                 //
//********************************************************************************************************************//

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#define DEFAULT_TABLE_SIZE 8
#define REHASH_INDEX 0.75
#define SALT 13

template<typename Key>
class [[maybe_unused]] defaultHash {
public:
    int32_t operator()(const Key &key) const {
        return static_cast<int32_t> (std::hash<std::string>{}(key));
    }
};

class [[maybe_unused]] stringHash {
public:
    int32_t operator()(const std::string &key) const {
        int32_t hash = 0;
        const char *str = key.c_str();
        for (; *str != 0; ++str) {
            hash = (hash * SALT + *str);
        }
        return hash;
    }
};

template<typename Key, typename Value, class Hash = defaultHash<Key>>
class QuadraticProbingTable {
    const static uint32_t FREE = 0;
    const static uint32_t BUSY = 1;
    const static uint32_t REMOVED = 2;

    Hash HashFunc = Hash();

    struct HashNode {
        size_t key_len = 0;
        size_t value_len = 0;
        Key key{};
        Value value{};
        int state = FREE;
    };

    size_t _size = 0;

    std::vector<HashNode> _cells;

public:

    QuadraticProbingTable() = default;
    explicit QuadraticProbingTable(const uint32_t &tableSize);

    bool Insert(const Key &key, const size_t &key_len, const Value &value, const size_t &value_len);
    bool Remove(const Key &key, const size_t &key_len);
    bool Find(const Key &key, const size_t &key_len) const;

    ~QuadraticProbingTable();

private:
    void grow();
};


template<typename Key, typename Value, class Hash>
QuadraticProbingTable<Key, Value, Hash>::QuadraticProbingTable(const uint32_t &tableSize) : _size(0) {
    _cells.resize(tableSize);
}


template<typename Key, typename Value, class Hash>
bool QuadraticProbingTable<Key, Value, Hash>::Insert(
        const Key &key,
        const size_t &key_len,
        const Value &value,
        const size_t &value_len) {
    size_t sequenceLength = 0;
    int32_t hash = HashFunc(key);

    if (((double) _size / (double) _cells.capacity()) >= REHASH_INDEX) {
        grow();
    }

    auto a = _cells.capacity();

    auto first_deleted_index = -1;

    for (; sequenceLength < _cells.capacity();) {
        auto q = sequenceLength * sequenceLength;
        auto index = (hash + sequenceLength / 2 + q / 2) % _cells.capacity();


        if (_cells[index].state == BUSY
            && _cells[index].key == key) {
            return false; // add comparator
        }

        if (_cells[index].state == REMOVED && first_deleted_index < 0) {
            first_deleted_index = index;
        }

        if (_cells[index].state == FREE) {
            _cells[index].key_len = key_len;
            _cells[index].value_len = value_len;
            _cells[index].key = key;
            _cells[index].value = value;
            _cells[index].state = BUSY;

            _size++;

            return true;
        }
        sequenceLength++;
    }

    if (first_deleted_index >= 0) {
        _cells[first_deleted_index].key_len = key_len;
        _cells[first_deleted_index].value_len = value_len;
        _cells[first_deleted_index].key = key;
        _cells[first_deleted_index].value = value;
        _cells[first_deleted_index].state = BUSY;

        _size++;
        return true;
    }

    return false;
}

template<typename Key, typename Value, class Hash>
bool QuadraticProbingTable<Key, Value, Hash>::Remove(
        const Key &key,
        const size_t &key_len) {
    size_t sequenceLength = 0;
    int32_t hash = HashFunc(key);

    for (; sequenceLength < _cells.capacity();) {
        auto q = sequenceLength * sequenceLength;
        auto index = (hash + sequenceLength / 2 + q / 2) % _cells.capacity();

        if (_cells[index].state == BUSY && _cells[index].key == key) {
            _cells[index].state = REMOVED;

            _size--;

            return true;
        }

        if (_cells[index].state == FREE) {
            return false;
        }
        sequenceLength++;
    }

    return false;
}

template<typename Key, typename Value, class Hash>
bool QuadraticProbingTable<Key, Value, Hash>::Find(
        const Key &key,
        const size_t &key_len) const {
    size_t sequenceLength = 0;
    int32_t hash = HashFunc(key);

    for (; sequenceLength < _cells.capacity();) {
        auto q = sequenceLength * sequenceLength;
        auto index = (hash + sequenceLength / 2 + q / 2) % _cells.capacity();

        if (_cells[index].state == BUSY && _cells[index].key == key) {
            return true;
        }

        if (_cells[index].state == FREE) {
            return false;
        }
        sequenceLength++;
    }
    return false;
}

template<typename Key, typename Value, class Hash>
QuadraticProbingTable<Key, Value, Hash>::~QuadraticProbingTable() {
    _cells.clear();
}


template<typename Key, typename Value, class Hash>
void QuadraticProbingTable<Key, Value, Hash>::grow() {
    std::vector<HashNode> tmp;
    tmp.resize(_size);
    std::copy_if(_cells.begin(), _cells.end(), tmp.begin(), [](HashNode x) {
        return x.state == BUSY;
    });

    auto cap = _cells.capacity();
    _cells.clear();
    _size = 0;
    _cells.resize(cap * 2);

    for (HashNode &it: _cells) { //mb delete
        it.state = FREE;
    }

    for (HashNode &it: tmp) {
        Insert(it.key, it.key_len, it.value, it.value_len);
    }
}


int main() {
    QuadraticProbingTable<std::string, int, stringHash> table(DEFAULT_TABLE_SIZE);
    char command = ' ';
    std::string key;
    bool status;

    while (std::cin >> command >> key) {
        switch (command) {
            case '+':
                status = table.Insert(key, key.size(), 4, sizeof(int) * 4);
                break;
            case '?':
                status = table.Find(key, key.size());
                break;
            case '-':
                status = table.Remove(key, key.size());
                break;
            default:
                continue;
        }

        if (status) {
            std::cout << "OK" << std::endl;
        } else {
            std::cout << "FAIL" << std::endl;
        }
    }

    return 0;
}
