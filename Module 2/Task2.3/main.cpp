//********************************************************************************************************************//
//                                                2.3. Порядок обхода                                                 //
// Дано число N < 106 и последовательность целых чисел из [-2^31..2^31] длиной N.                                     //
// Требуется построить бинарное дерево, заданное наивным порядком вставки.                                            //
// Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K, то узел K добавляется           //
// в правое поддерево root; иначе в левое поддерево root.                                                             //
// Требования: Рекурсия запрещена. Решение должно поддерживать передачу функции сравнения снаружи.                    //
// 2_3. Выведите элементы в порядке post-order (снизу вверх).                                                         //
//********************************************************************************************************************//

#include <iostream>
#include <unordered_set>

template<
        typename Key,
        class Compare = std::less<Key>
>
class BinaryTree {
public:
    class btNode {
    public:
        btNode() = default;
        btNode(Key data, btNode *parent,
               btNode *l_child, btNode *r_child);
        btNode(const Key &data)
                : _data(data), _parent(nullptr),
                  _l_child(nullptr), _r_child(nullptr) {}
        btNode(const btNode &);
        btNode(btNode &&) noexcept;
        btNode &operator=(const btNode &);
        btNode &operator=(btNode &&) noexcept;
        ~btNode();

    public:
        Key _data;
        btNode *_parent;
        btNode *_l_child;
        btNode *_r_child;
    };

public:
    using value_type = Key;
    using const_ref_type = const value_type &;
    using node_value_type = btNode;
    using const_node_value_type = const btNode;
    using node_pointer = btNode *;
    using const_node_pointer = const node_pointer;

public:
    BinaryTree() : _size(0), _root(nullptr), _header(new node_value_type) {}
    BinaryTree(const BinaryTree &);
    BinaryTree(BinaryTree &&) noexcept;
    explicit BinaryTree(const BinaryTree *);
    BinaryTree &operator=(const BinaryTree &);
    BinaryTree &operator=(BinaryTree &&) noexcept;
    ~BinaryTree();

    void insert(const_ref_type data);
    void erase(const_ref_type data);
    size_t size();
    bool empty();
    void clear(node_pointer);
    bool ge(const Key &l, const Key &r) const;
    bool gt(const Key &l, const Key &r) const;
    bool le(const Key &l, const Key &r) const;
    bool lt(const Key &l, const Key &r) const;
    bool eq(const Key &l, const Key &r) const;
    auto get_root(){return _root;}

    Compare cmp = Compare();

private:

    size_t _size = 0;
    node_pointer _header;
protected:
    node_pointer _root;
};

template<typename Key, class Compare>
bool BinaryTree<Key, Compare>
::ge(const Key &l, const Key &r) const {
    return !cmp(l, r);
}

template<typename Key, class Compare>
bool BinaryTree<Key, Compare>
::gt(const Key &l, const Key &r) const {
    return cmp(r, l);
}

template<typename Key, class Compare>
bool BinaryTree<Key, Compare>
::le(const Key &l, const Key &r) const {
    return !gt(l, r);
}

template<typename Key, class Compare>
bool BinaryTree<Key, Compare>
::lt(const Key &l, const Key &r) const {
    return cmp(l, r);
}

template<typename Key, class Compare>
bool BinaryTree<Key, Compare>
::eq(const Key &l, const Key &r) const {
    return !(cmp(l, r) || cmp(r, l));
}

template<typename Key, class Compare>
bool operator<(typename BinaryTree<Key, Compare>
::btNode const &c1,
               typename BinaryTree<Key, Compare>
               ::btNode const &c2) {
    return cmp(c1._data, c2._data);
}

template<typename Key, class Compare>
bool operator==(typename BinaryTree<Key, Compare>
::btNode const &c1,
                typename BinaryTree<Key, Compare>
                ::btNode const &c2) {
    return !((c1 < c2) || (c2 < c1));
}

template<typename Key, class Compare>
bool operator!=(typename BinaryTree<Key, Compare>
::btNode const &c1,
                typename BinaryTree<Key, Compare>
                ::btNode const &c2) {
    return !(operator==(c1, c2));
}

template<typename Key, class Compare>
bool operator>(typename BinaryTree<Key, Compare>
::btNode const &c1,
               typename BinaryTree<Key, Compare>
               ::btNode const &c2) {
    return operator<(c2, c1);
}

template<typename Key, class Compare>
bool operator<=(typename BinaryTree<Key, Compare>
::btNode const &c1,
                typename BinaryTree<Key, Compare>
                ::btNode const &c2) {
    return !(operator>(c1, c2));
}

template<typename Key, class Compare>
bool operator>=(typename BinaryTree<Key, Compare>
::btNode const &c1,
                typename BinaryTree<Key, Compare>
                ::btNode const &c2) {
    return !(operator<(c1, c2));
}

template<typename Key, class Compare>
BinaryTree<Key, Compare>::BinaryTree(const BinaryTree &other) : BinaryTree() {
    for (auto it : other) {
        insert(it);
    }
}

template<typename Key, class Compare>
BinaryTree<Key, Compare>::BinaryTree(BinaryTree &&other) noexcept
: BinaryTree(other) {
    other._size = 0;
    other = nullptr;
}

template<typename Key, class Compare>
BinaryTree<Key, Compare>::BinaryTree(const BinaryTree *other)
        : BinaryTree(*other) {
}

template<typename Key, class Compare>
BinaryTree<Key, Compare> &BinaryTree<Key, Compare>
::operator=(const BinaryTree &other) {
    if (&other == this) {
        return *this;
    }

    clear(_root);
    _size = 0;

    for (auto &it : other) {
        insert(it);
    }
    return *this;
}

template<typename Key, class Compare>
BinaryTree<Key, Compare> &BinaryTree<Key, Compare>
::operator=(BinaryTree &&other) noexcept {
*this = other;

other._root = nullptr;
other._size = 0;
}

template<typename Key, class Compare>
BinaryTree<Key, Compare>::~BinaryTree() {
    clear(_root);
    delete _header;
}

template<typename Key, class Compare>
void BinaryTree<Key, Compare>::insert(const_ref_type data) {
    if (_root == nullptr) {
        _root = new node_value_type(data);
        _header->_l_child = _header->_r_child = _root;
        _root->_parent = _header;
        _size++;
        return;
    }else{
        auto node = _root;
        for (; ;) {
            if(le(node->_data, data)){
                if(node->_r_child == nullptr){
                    node->_r_child = new node_value_type(data);
                    node->_r_child->_parent = node;
                    _size++;
                    break;
                }
                node = node->_r_child;
            }else{
                if(node->_l_child == nullptr){
                    node->_l_child = new node_value_type(data);
                    node->_l_child->_parent = node;
                    _size++;
                    break;
                }
                node = node->_l_child;
            }
        }
    }
}



template<typename Key, class Compare>
size_t BinaryTree<Key, Compare>::size() {
    return _size;
}

template<typename Key, class Compare>
bool BinaryTree<Key, Compare>::empty() {
    return _size == 0;
}

template<typename Key, class Compare>
void BinaryTree<Key, Compare>::clear(node_pointer node) {
    if (node == nullptr) {
        return;
    }
    clear(node->_l_child);
    clear(node->_r_child);
    delete node;
}

template<typename Key, class Compare>
BinaryTree<Key, Compare>::btNode::btNode(
        Key data,
        BinaryTree::btNode *parent,
        BinaryTree::btNode *l_child,
        BinaryTree::btNode *r_child):
        _data(data),
        _parent(parent),
        _l_child(l_child),
        _r_child(r_child) {
}

template<typename Key, class Compare>
BinaryTree<Key, Compare>::btNode
::btNode(const btNode &other)
        : _data(other._data),
          _parent(other._parent),
          _l_child(other._l_child),
          _r_child(other._r_child) {
}

template<typename Key, class Compare>
BinaryTree<Key, Compare>::btNode
::btNode(btNode &&other) noexcept
: btNode(other) {
        delete other;
        other = nullptr;
}

template<typename Key, class Compare>
BinaryTree<Key, Compare>::btNode
::~btNode() {
    _parent = nullptr;
    _l_child = nullptr;
    _r_child = nullptr;
}

template<typename Key, class Compare>
typename BinaryTree<Key, Compare>
::btNode &BinaryTree<Key, Compare>
::btNode::operator=(const BinaryTree::btNode &other) {
    _data = other._data;
    _parent = other._parent;
    _l_child = other._l_child;
    _r_child = other._r_child;
}

template<typename Key, class Compare>
typename BinaryTree<Key, Compare>
::btNode &BinaryTree<Key, Compare>
::btNode::operator=(btNode &&other) noexcept {
*this = other;

other._parent = nullptr;
other._l_child = nullptr;
other._r_child = nullptr;
}

template<typename Key, class Compare>
std::ostream &operator<<(std::ostream &out, BinaryTree<Key, Compare> &tree) {
    auto tmp = tree.get_root();
    std::unordered_set<typename BinaryTree<Key, Compare>::btNode *> visited;

    for (; tmp && visited.find(tmp) == visited.end();) {
        if (tmp->_l_child &&
            visited.find(tmp->_l_child) == visited.end()) {
            tmp = tmp->_l_child;
        } else if (tmp->_r_child &&
                   visited.find(tmp->_r_child) == visited.end()) {
            tmp = tmp->_r_child;
        } else {
            out << tmp->_data << " ";
            visited.insert(tmp);
            tmp = tree.get_root();
        }
    }
    return out;
}

int main(){
    BinaryTree<int> tree;
    size_t count = 0;

    std::cin >> count;
    int32_t elem = 0;

    for (size_t i = 0; i < count; i++) {
        std::cin >> elem;
        tree.insert(elem);
    }

    std::cout << tree << std::endl;
}
