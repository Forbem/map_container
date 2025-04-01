#pragma once
#include "map.h"

template <typename Key, typename Value, typename Compare = less<Key>>
class MapRB : public Map<Key, Value, Compare> {
public:
    using Base = Map<Key, Value, Compare>;
    using Node = typename Base::NodeType;
    using Iterator = typename Base::Iterator;

protected:
    // Левый поворот вокруг узла x
    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        
        if (!x->parent) {
            this->root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->left = x;
        x->parent = y;
    }

    // Правый поворот вокруг узла y
    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;
        
        if (!y->parent) {
            this->root = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }
        
        x->right = y;
        y->parent = x;
    }

    // Восстановление свойств после вставки
    void fixInsert(Node* node) {
        while (node != this->root && node->parent->color) {
            if (node->parent == node->parent->parent->left) {
                Node* uncle = node->parent->parent->right;
                if (uncle && uncle->color) {
                    // Случай 1: дядя красный
                    node->parent->color = false;
                    uncle->color = false;
                    node->parent->parent->color = true;
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->right) {
                        // Случай 2: дядя черный, узел - правый потомок
                        node = node->parent;
                        leftRotate(node);
                    }
                    // Случай 3: дядя черный, узел - левый потомок
                    node->parent->color = false;
                    node->parent->parent->color = true;
                    rightRotate(node->parent->parent);
                }
            } else {
                // Зеркальные случаи для правого поддерева
                Node* uncle = node->parent->parent->left;
                if (uncle && uncle->color) {
                    node->parent->color = false;
                    uncle->color = false;
                    node->parent->parent->color = true;
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->left) {
                        node = node->parent;
                        rightRotate(node);
                    }
                    node->parent->color = false;
                    node->parent->parent->color = true;
                    leftRotate(node->parent->parent);
                }
            }
        }
        this->root->color = false; // Корень всегда черный
    }

    // Восстановление свойств после удаления
    void fixDelete(Node* node) {
        while (node != this->root && (!node || !node->color)) {
            if (node == node->parent->left) {
                Node* sibling = node->parent->right;
                if (sibling && sibling->color) {
                    // Случай 1: брат красный
                    sibling->color = false;
                    node->parent->color = true;
                    leftRotate(node->parent);
                    sibling = node->parent->right;
                }
                if ((!sibling->left || !sibling->left->color) && 
                    (!sibling->right || !sibling->right->color)) {
                    // Случай 2: брат черный и оба его ребенка черные
                    sibling->color = true;
                    node = node->parent;
                } else {
                    if (!sibling->right || !sibling->right->color) {
                        // Случай 3: брат черный, его левый ребенок красный
                        if (sibling->left) sibling->left->color = false;
                        sibling->color = true;
                        rightRotate(sibling);
                        sibling = node->parent->right;
                    }
                    // Случай 4: брат черный, его правый ребенок красный
                    sibling->color = node->parent->color;
                    node->parent->color = false;
                    if (sibling->right) sibling->right->color = false;
                    leftRotate(node->parent);
                    node = this->root;
                }
            } else {
                // Зеркальные случаи для правого поддерева
                Node* sibling = node->parent->left;
                if (sibling && sibling->color) {
                    sibling->color = false;
                    node->parent->color = true;
                    rightRotate(node->parent);
                    sibling = node->parent->left;
                }
                if ((!sibling->right || !sibling->right->color) && 
                    (!sibling->left || !sibling->left->color)) {
                    sibling->color = true;
                    node = node->parent;
                } else {
                    if (!sibling->left || !sibling->left->color) {
                        if (sibling->right) sibling->right->color = false;
                        sibling->color = true;
                        leftRotate(sibling);
                        sibling = node->parent->left;
                    }
                    sibling->color = node->parent->color;
                    node->parent->color = false;
                    if (sibling->left) sibling->left->color = false;
                    rightRotate(node->parent);
                    node = this->root;
                }
            }
        }
        if (node) node->color = false;
    }

    // Замена поддерева
    void transplant(Node* u, Node* v) {
        if (!u->parent) {
            this->root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        if (v) v->parent = u->parent;
    }

public:
    // Удаление по ключу
    void erase(const Key& key) override {
        Node* node = this->root;
        while (node) {
            if (this->comp(key, node->data.first)) {
                node = node->left;
            } else if (this->comp(node->data.first, key)) {
                node = node->right;
            } else {
                break;
            }
        }
        if (!node) return;

        Node* y = node;
        bool yOriginalColor = y->color;
        Node* x;

        if (!node->left) {
            x = node->right;
            transplant(node, node->right);
        } else if (!node->right) {
            x = node->left;
            transplant(node, node->left);
        } else {
            y = node->right;
            while (y->left) y = y->left;
            yOriginalColor = y->color;
            x = y->right;
            if (y->parent == node) {
                if (x) x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = node->right;
                y->right->parent = y;
            }
            transplant(node, y);
            y->left = node->left;
            y->left->parent = y;
            y->color = node->color;
        }
        delete node;
        if (!yOriginalColor && x) fixDelete(x);
    }

    // Удаление по итератору
    void erase(Iterator pos) override { erase(pos->first); }
};