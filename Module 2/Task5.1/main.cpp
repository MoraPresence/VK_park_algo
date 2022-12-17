//********************************************************************************************************************//
//                                            5. Алгоритм сжатия данных Хаффмана                                      //
// Напишите две функции для создания архива из одного файла и извлечения файла из архива.                             //
//                                                                                                                    //
//Метод архивирует данные из потока original                                                                          //
//void Encode(IInputStream& original, IOutputStream& compressed);                                                     //
// В архиве сохраняйте дерево Хаффмана и код Хаффмана от исходных данных.                                             //
// Дерево Хаффмана требуется хранить эффективно - не более 10 бит на каждый 8-битный символ.                          //
//                                                                                                                    //
//В контест необходимо отправить .cpp файл содержащий функции Encode, Decode,                                         //
// а также включающий файл Huffman.h. Тестирующая программа выводит размер сжатого файла в процентах от исходного.    //
//********************************************************************************************************************//

#include "Huffman.h"

#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <vector>

#define INDENT_TREE 2

class Bits {
public:
    void WriteBit(unsigned char bit);

    void WriteByte(unsigned char byte);

    [[nodiscard]] std::vector<byte> Buffer() const { return buffer; }

private:
    std::vector<byte> buffer;
    int bitsCount = 0;
};

void Bits::WriteBit(unsigned char bit) {
    if (bitsCount + 1 > buffer.size() * 8) {
        buffer.push_back(0);
    }

    if (bit != '0') {
        buffer[bitsCount / 8] |= 1 << (bitsCount % 8);
    }

    ++bitsCount;
}

void Bits::WriteByte(unsigned char byte) {
    int test = bitsCount % 8;
    if (!test) {
        buffer.push_back(byte);
    } else {
        if (bitsCount) {
            buffer[bitsCount / 8] |= byte << (bitsCount % 8);
        }
        buffer.push_back(byte >> (8 - bitsCount % 8));
    }
    bitsCount += 8;
}

class Node {
public:
    Node() : _l_child(nullptr), _r_child(nullptr), _freq(0), _value(0), _is_empty(true) {};

    explicit Node(byte value) : _l_child(nullptr), _r_child(nullptr), _freq(1), _value(value), _is_empty(false) {};

    Node(byte value, size_t freq) : _l_child(nullptr), _r_child(nullptr), _freq(freq), _value(value),
                                    _is_empty(false) {};

    Node(Node *left, Node *right) : _l_child(left), _r_child(right), _value(0), _is_empty(true) {
        _freq = left->_freq + right->_freq;
    }

public:
    Node *_l_child = nullptr;
    Node *_r_child = nullptr;
    uint64_t _freq = 0;
    byte _value = 0;
    bool _is_empty = true;

    class Compare {
    public:
        bool operator()(Node *a, Node *b) {
            return b->_freq < a->_freq;
        }
    };
};

void BuildTable(Node *node, std::vector<byte> &code, std::map<byte, std::vector<byte>> &table) {
    if (node->_l_child) {
        code.push_back('0');
        BuildTable(node->_l_child, code, table);
    }

    if (node->_r_child) {
        code.push_back('1');
        BuildTable(node->_r_child, code, table);
    }

    if (!node->_is_empty) {
        if (code.empty()) {
            code.push_back('1');
        }
        table.insert(std::pair<byte, std::vector<byte>>(node->_value, code));
    }
    if (!code.empty()) {
        code.pop_back();
    }
}

void treeSerialize(Node *node, std::vector<byte> &tree, size_t &size_in_bits) {
    if (!node->_is_empty) {
        tree.push_back('1');
        for (int i = 0; i < 8; ++i) {
            if (node->_value & (1 << 7 - i)) {
                tree.push_back('1');
            } else {
                tree.push_back('0');
            }
        }
        size_in_bits += 9;
    } else {
        treeSerialize(node->_l_child, tree, size_in_bits);
        treeSerialize(node->_r_child, tree, size_in_bits);
        tree.push_back('0');
        size_in_bits += 1;
    }
}

Node *treeUnserialize(std::vector<byte> &tree, uint8_t size) {
    std::stack<Node *> stack;
    for (auto it = tree.begin(); it != tree.end() && size != 0;) {
        if (*it == '1') {
            byte value = 0;
            it++;
            for (size_t j = 0; j < 8; ++j) {
                if (*it == '1') {
                    value |= 1 << (7 - j);
                }
                it++;
            }
            stack.push(new Node(value));
            --size;
        } else {
            ++it;
            if (stack.size() == 1 && size == 1) {
                auto node = stack.top();
                stack.pop();
                return node;
            }

            if (stack.size() != 1) {
                auto right = stack.top();
                stack.pop();
                auto left = stack.top();
                stack.pop();

                stack.push(new Node(left, right));
            }
        }
    }

    for (; stack.size() != 1;) {
        auto right = stack.top();
        stack.pop();
        auto left = stack.top();
        stack.pop();

        stack.push(new Node(left, right));
    }
    auto node = stack.top();
    stack.pop();
    return node;
}

class Encoder {
public:
    ~Encoder() {
        if (_root == nullptr) {
            return;
        }
        std::stack<Node *> stack;
        stack.push(_root);
        for (; !stack.empty();) {
            Node *node = stack.top();
            stack.pop();
            if (node->_r_child != nullptr) {
                stack.push(node->_r_child);
            }
            if (node->_l_child != nullptr) {
                stack.push(node->_l_child);
            }
            delete node;
        }
    }

    bool GetDataFromFile(IInputStream &file, IOutputStream &out) {
        byte value;

        for (; file.Read(value);) {
            bool foundFlag = false;
            for (auto &node:_freq) {
                if (node->_value == value) {
                    node->_freq++;
                    foundFlag = true;
                }
            }
            if (!foundFlag) {
                auto *node = new Node(value);
                _freq.push_back(node);
            }
            _data.push_back(value);
        }

        return !(_freq.size() == 0);
    }

    void fillHeap() {
        for (const auto &symbol: _freq) {
            _heap.push(new Node(symbol->_value, symbol->_freq));
        }
    }

    void BuildTree() {
        fillHeap();

        for (; _heap.size() != 1;) {
            Node *left = _heap.top();
            _heap.pop();
            Node *right = _heap.top();
            _heap.pop();
            _heap.push(new Node(left, right));
        }
        _root = _heap.top();
    }

    uint64_t MakeRawData() {
        std::vector<byte> code;
        BuildTable(_root, code, _table);
        for (unsigned char i : _data) {
            for (const unsigned char &bin : _table.find(i)->second) {
                _encode_data.push_back(bin);
            }
        }
        return _data.size();
    }

    void WriteData(IOutputStream &compressed) {
        MakeRawData();
        std::vector<byte> Data(createTable(_encode_data.size()));

        for (auto &i:Data) {
            compressed.Write(i);
        }
    }

    std::vector<byte> createTable(uint64_t dataSize) {
        std::stack<Node *> stack;
        stack.push(_root);
        std::vector<byte> symbols;
        Bits table;

        uint8_t size_data_reminder = 0;

        for (; dataSize % 8 != 0;) {
            _encode_data.push_back('0');
            ++dataSize;
            ++size_data_reminder;
        }

        _size = _freq.size();


        table.WriteByte((_size >> 0 * 8) & 0xff);

        table.WriteByte((size_data_reminder >> 0 * 8) & 0xff);


        size_t size_in_bits = 0;
        treeSerialize(_root, symbols, size_in_bits);
        for (; size_in_bits % 8 != 0;) {
            symbols.push_back('0');
            ++size_in_bits;
        }

        byte value = 0;
        std::vector<byte> tree_in_bytes;
        for (size_t i = 0; i < size_in_bits;) {
            for (int j = 0; j < 8; ++j) {
                if (symbols[i] == '1') {
                    value |= 1 << (7 - j);
                }
                ++i;
            }

            tree_in_bytes.push_back(value);
            value = 0;
        }

        for (const auto &i: tree_in_bytes) {
            table.WriteByte(i);
        }

        std::vector<byte> data_in_bytes;
        for (size_t i = 0; i < _encode_data.size();) {
            for (int j = 0; j < 8; ++j) {
                if (_encode_data[i] == '1') {
                    value |= 1 << (7 - j);
                }
                ++i;
            }

            data_in_bytes.push_back(value);
            value = 0;
        }

        for (const auto &i:data_in_bytes) {
            table.WriteByte(i);
        }

        return table.Buffer();
    }

public:
    std::vector<byte> _data;
    Node *_root = nullptr;
    std::vector<byte> _encode_data;
    std::priority_queue<Node *, std::vector<Node *>, Node::Compare> _heap;
    std::vector<Node *> _freq;
    uint8_t _size = 0;
    std::map<byte, std::vector<byte>> _table;
};


class Decoder {
public:
    ~Decoder() {
        if (_root == nullptr) {
            return;
        }
        std::stack<Node *> stack;
        stack.push(_root);
        for (; !stack.empty();) {
            Node *node = stack.top();
            stack.pop();
            if (node->_r_child != nullptr) {
                stack.push(node->_r_child);
            }
            if (node->_l_child != nullptr) {
                stack.push(node->_l_child);
            }
            delete node;
        }
    }

    void HandleRawData(IOutputStream &compressed) {
        uint8_t treeSize = GetTreeSize();
        uint8_t dataSize = GetDataSize();
        uint16_t realTreeSize = getTreeSizeInBits(treeSize);

        std::vector<byte> binTree = getBinTree(realTreeSize);
        _root = treeUnserialize(binTree, treeSize);
        std::vector<byte> code;
        BuildTable(_root, code);
        GetOriginFile(realTreeSize, dataSize, compressed);
    }

    void GetOriginFile(uint32_t treeSize, uint32_t data_remainder, IOutputStream &compressed) {
        Node *parent = _root;
        byte bit = 0;
        uint64_t currentBit = 0;
        uint32_t skipSize = INDENT_TREE + treeSize;

        Bits bits;
        byte value = 0;
        int64_t end = 0;
        std::vector<byte> t;

        for (auto &i : _table) {
            tableCode.insert(std::make_pair(i.second, i.first));
        }


        for (uint64_t i = skipSize; i < _encodeData.size(); ++i) {
            value = _encodeData[i];
            if (i == _encodeData.size() - 1) {
                end = data_remainder;
            }
            for (int64_t j = 7; j >= end; --j) {
                unsigned char a = (value & (1 << j));
                if ((value & (1 << j))) {
                    t.push_back('1');
                } else {
                    t.push_back('0');
                }

                if (tableCode.find(t) != tableCode.end()) {
                    compressed.Write(tableCode[t]);
                    t.clear();
                }
            }

        }
    }

    void BuildTable(Node *node, std::vector<byte> &code) {
        if (node->_l_child) {
            code.push_back('0');
            BuildTable(node->_l_child, code);
        }

        if (node->_r_child) {
            code.push_back('1');
            BuildTable(node->_r_child, code);
        }

        if (!node->_is_empty) {
            if (code.empty()) {
                code.push_back('1');
            }
            _table.insert(std::pair<byte, std::vector<byte>>(node->_value, code));
        }
        if (!code.empty()) {
            code.pop_back();
        }
    }

    static uint32_t getTreeSizeInBits(uint32_t size) {
        return std::ceil(size * 1.25);
    }

    std::vector<byte> getBinTree(uint32_t treeSize) {
        std::vector<byte> tree;
        uint64_t pos = INDENT_TREE;
        for (; pos < treeSize + INDENT_TREE;) {
            for (int i = 0; i < 8; ++i) {
                if (_encodeData[pos] & (1 << 7 - i)) {
                    tree.push_back('1');
                } else {
                    tree.push_back('0');
                }
            }
            pos++;
        }
        return tree;
    }

    uint8_t GetDataSize() {
        return _encodeData[1];
    }

    uint8_t GetTreeSize() {
        return (_encodeData[0]);
    }

    bool GetDataFromFile(IInputStream &file, IOutputStream &out) {
        byte value;
        for (; file.Read(value);) {
            _encodeData.push_back(value);
        }

        if (_encodeData.size() == 0) {
            return false;
        }
        return true;
    }

public:
    std::vector<byte> _encodeData;
    Node *_root = nullptr;
    std::map<byte, std::vector<byte>> _table;
    std::map<std::vector<byte>, byte> tableCode;
};

void Encode(IInputStream &original, IOutputStream &compressed) {
    Encoder Zip;
    if (Zip.GetDataFromFile(original, compressed)) {
        Zip.BuildTree();
        Zip.WriteData(compressed);
    }
}

void Decode(IInputStream &compressed, IOutputStream &original) {
    try {
        Decoder Unzip;
        if (Unzip.GetDataFromFile(compressed, original)) {
            Unzip.HandleRawData(original);
        }
    }
    catch (...) {
        std::cerr << "error";
    }
}
