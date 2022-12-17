//********************************************************************************************************************//
//                                                3. B-дерево                                                         //
// Постройте B-дерево минимального порядка t и выведите его по слоям.                                                 //
// В качестве ключа используются числа, лежащие в диапазоне [0..232-1]                                                //
// Требования:                                                                                                        //
//                                                                                                                    //
// B-дерево должно быть реализовано в виде шаблонного класса.                                                         //
// Решение должно поддерживать передачу функции сравнения снаружи.                                                    //
//********************************************************************************************************************//

#include <iostream>
#include <cassert>
#include <vector>
#include <queue>
#include <sstream>

template<typename Key, typename Comparator = std::less<Key>>
class BTree {
public:
    struct Node {
        explicit Node(bool leaf);

        ~Node();

        bool _leaf;
        std::vector<Key> _keys;
        std::vector<Node *> _children;
    };

    explicit BTree(size_t min_order);

    ~BTree();

    void insert(const Key &key);
    bool find(const Key &key);
    std::vector<std::vector<Key>> levelOrder();

private:

    [[nodiscard]] bool lt(const Key &lhs, const Key &rhs) const {
        return cmp(lhs, rhs);
    }

    [[nodiscard]] bool eq(const Key &lhs, const Key &rhs) const {
        return !lt(lhs, rhs) && !lt(rhs, lhs);
    }

    [[nodiscard]] bool gt(const Key &lhs, const Key &rhs) const {
        return !lt(lhs, rhs) && !eq(lhs, rhs);
    }

    bool is_node_full_(Node *node);
    bool find_(Node *node, const Key &key);
    void split_(Node *parent, size_t index);
    void insert_non_full_(Node *node, const Key &key);

    Node *_root;
    size_t _min_order;
    size_t _height;
    Comparator cmp = Comparator();
};


template<typename Key, typename Comparator>
BTree<Key, Comparator>::Node::Node(bool leaf)
        : _leaf(leaf) {
}

template<typename Key, typename Comparator>
BTree<Key, Comparator>::Node::~Node() {
    for (Node *child: _children) {
        delete child;
    }
}

template<typename Key, typename Comparator>
BTree<Key, Comparator>::BTree(size_t min_order)
        : _root(nullptr), _min_order(min_order), _height(0) {
    assert(min_order >= 2);
}

template<typename Key, typename Comparator>
BTree<Key, Comparator>::~BTree() {
    delete _root;
}

template<typename Key, typename Comparator>
void BTree<Key, Comparator>::insert(const Key &key) {
    if (!_root) {
        _root = new Node(true);
        ++_height;
    }

    if (is_node_full_(_root)) {
        Node *newRoot = new Node(false);
        newRoot->_children.push_back(_root);
        _root = newRoot;
        split_(_root, 0);
        ++_height;
    }

    insert_non_full_(_root, key);
}

template<typename Key, typename Comparator>
bool BTree<Key, Comparator>::find(const Key &key) {
    return find_(_root, key);
}

template<typename Key, typename Comparator>
bool BTree<Key, Comparator>::is_node_full_(BTree::Node *node) {
    return node->_keys.size() == 2 * _min_order - 1;
}

template<typename Key, typename Comparator>
bool BTree<Key, Comparator>::find_(BTree::Node *node, const Key &key) {
    size_t i = 0;
    for (; i < node->_keys.size() && gt(key, node->_keys[i]); ++i) {}

    if (i < node->_keys.size() && eq(key, node->_keys[i])) {
        return true;
    } else if (node->_leaf) {
        return false;
    } else {
        return find_(node->_children[i], key);
    }
}

template<typename Key, typename Comparator>
void BTree<Key, Comparator>::split_(BTree::Node *parent, size_t index) {
    auto nodeToSplit = parent->_children[index];
    auto newNode = new Node(nodeToSplit->_leaf);

    parent->_keys.resize(parent->_keys.size() + 1);

    if (parent->_keys.size() != 1) {
        for (int32_t i = parent->_keys.size() - 1; i > index; --i) {
            parent->_keys[i] = parent->_keys[i - 1];
        }
    }

    parent->_keys[index] = nodeToSplit->_keys[_min_order - 1];

    parent->_children.resize(parent->_children.size() + 1);
    for (int32_t i = parent->_children.size() - 1; i > index + 1; --i) {
        parent->_children[i] = parent->_children[i - 1];
    }

    parent->_children[index + 1] = newNode;

    newNode->_keys.resize(_min_order - 1);
    for (size_t i = 0; i < _min_order - 1; ++i) {
        newNode->_keys[i] = nodeToSplit->_keys[_min_order + i];
    }

    nodeToSplit->_keys.resize(_min_order - 1);
    if (!nodeToSplit->_leaf) {
        newNode->_children.resize(_min_order);
        for (size_t i = 0; i < _min_order; ++i) {
            newNode->_children[i] = nodeToSplit->_children[_min_order + i];
        }
        nodeToSplit->_children.resize(_min_order);
    }
}

template<typename Key, typename Comparator>
void BTree<Key, Comparator>::insert_non_full_(BTree::Node *node, const Key &key) {
    int32_t pos = node->_keys.size() - 1;
    if (node->_leaf) {
        node->_keys.resize(node->_keys.size() + 1);
        for (; pos >= 0 && lt(key, node->_keys[pos]); --pos) {
            node->_keys[pos + 1] = node->_keys[pos];
        }
        node->_keys[pos + 1] = key;
    } else {
        for (; pos >= 0 && lt(key, node->_keys[pos]); --pos) {}

        if (is_node_full_(node->_children[pos + 1])) {
            split_(node, pos + 1);
            if (key > node->_keys[pos + 1]) {
                pos++;
            }
        }
        insert_non_full_(node->_children[pos + 1], key);
    }
}

template<typename Key, typename Comparator>
std::vector<std::vector<Key>> BTree<Key, Comparator>::levelOrder() {
    std::vector<std::vector<Key>> ans;
    std::queue<Node *> main_queue;

    main_queue.push(_root);

    std::vector<Key> tmp;

    for (; !main_queue.empty();) {
        size_t n = main_queue.size();
        for (size_t i = 0; i < n; i++) {
            Node *cur = main_queue.front();
            main_queue.pop();
            for (auto &it: cur->_keys) {
                tmp.push_back(it);
            }
            for (auto u : cur->_children)
                main_queue.push(u);
        }
        ans.push_back(tmp);
        tmp.clear();
    }
    return ans;
}

int main(int argc, const char *argv[]) {

    std::string degree_str;
    std::getline(std::cin, degree_str);
    size_t degree = std::stoi(degree_str);

    size_t key;
    BTree<size_t> btree(degree);

    std::string input;

    std::getline(std::cin, input);
    std::stringstream iss( input );

    for (; iss >> key;) {
        btree.insert(key);
    }

    auto vec = btree.levelOrder();

    for (auto &sub_vec: vec) {
        for (auto &it : sub_vec) {
            std::cout << it << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
