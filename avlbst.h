#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    virtual void balanceTree(AVLNode<Key, Value>* node);
    // Rotation functions
    virtual void rotationRight(AVLNode<Key, Value>* node);
    virtual void rotationRL(AVLNode<Key, Value>* node);
    virtual void rotationLeft(AVLNode<Key, Value>* node);
    virtual void rotationLR(AVLNode<Key, Value>* node);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // Add element to the root if tree is empty
    if (this->root_ == nullptr) {
        this->root_ = new Node<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    Node<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    Node<Key, Value>* parent = nullptr;

    // Find the parent node for new_item
    while (current != nullptr) {
        parent = current;

        // Key exists, update the value to new_item's value
        if (new_item.first == current->getKey()) {
            current->setValue(new_item.second);
        }
        // Key doesn't exist, keyValuePair's value is less than current node's value
        // Go to left subtree
        else if (new_item.first < current->getKey()) {
            parent = current->getLeft();
        }
        // Key doesn't exist, keyValuePair's value is greater than current node's value
        // Go to right subtree
        else {
            parent = current->getRight();
        }
    }

    // Create the new node to add
    Node<Key, Value>* newNode = new Node<Key, Value>(new_item.first, new_item.second, parent);

    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    } 
    else {
        parent->setRight(newNode);
    }
    newNode->setParent(parent);

    // Call balanceTree() to balance the tree
    balanceTree(newNode);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    Node<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    // Node with key is not in the tree, return
    if (nodeToRemove == nullptr) {
        return;
    } 

    // Case 1: Node has two children
    if ((nodeToRemove->getLeft() != nullptr) && (nodeToRemove->getRight() != nullptr)) {
        Node<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(nodeToRemove));
        // Swap nodeToRemove with predecessor if it exists
        if (pred) {
            nodeSwap(nodeToRemove, pred);
        }
    }

    // Case 2: Node has one or no children
    Node<Key, Value>* child = nodeToRemove->getLeft() ? nodeToRemove->getLeft() : nodeToRemove->getRight();
    Node<Key, Value>* parent = nodeToRemove->getParent();

    if (nodeToRemove == this->root_) {
        this->root_ = child;
    } 
    else {
        if (parent->getLeft() == nodeToRemove) {
            parent->setLeft(child);
        }
        else { 
            parent->setRight(child);
        }
    }
    
    if (child) {
        child->setParent(parent);
    }

    delete nodeToRemove;

    // Call balanceTree() to balance tree
    balanceTree(parent);
}

/*
 * Helper function that balances the tree
 */
template<class Key, class Value>
void AVLTree<Key, Value>::balanceTree(AVLNode<Key, Value>* node) {
    while (node != nullptr) {
        // Find the balance factor for the current node
        int balance = getHeight(node->getLeft()) - getHeight(node->getRight());

        node->setBalance(balance);

        // More in the left
        if (balance > 1) { 
            // LL-rotation needed
            if (getHeight(node->getLeft()->getLeft()) >= getHeight(node->getLeft()->getRight())) {
                rotationLeft(node);
            } 
            // LR-rotation needed
            else {
                rotationLR(node);
            }
        } 
        // More in the right
        else if (balance < -1) {
            // RR-rotation needed
            if (getHeight(node->getRight()->getRight()) >= getHeight(node->getRight()->getLeft())) {
                rotationRight(node);
            } 
            // RL-rotation needed
            else {
                rotationRL(node);
            }
        }
        // Move up tree
        node = node->getParent();
    }
}

/*
 * Helper functions that rotates based on specific imbalance type.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotationRight(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* leftChild = node->getLeft();
    node->setLeft(leftChild->getRight());

    if (leftChild->getRight() != nullptr) {
        leftChild->getRight()->setParent(node);
    }
    leftChild->setParent(node->getParent());

    if (node->getParent() == nullptr) {
        this->root_ = leftChild;
    } else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(leftChild);
    } else {
        node->getParent()->setRight(leftChild);
    }

    leftChild->setRight(node);
    node->setParent(leftChild);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotationRL(AVLNode<Key, Value>* node) {
    rotationRight(node->getRight());
    rotationLeft(node);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotationLeft(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* rightChild = node->getRight();
    node->setRight(rightChild->getLeft());

    if (rightChild->getLeft() != nullptr) {
        rightChild->getLeft()->setParent(node);
    }
    rightChild->setParent(node->getParent());

    if (node->getParent() == nullptr) {
        this->root_ = rightChild;
    } else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(rightChild);
    } else {
        node->getParent()->setRight(rightChild);
    }

    rightChild->setLeft(node);
    node->setParent(rightChild);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotationLR(AVLNode<Key, Value>* node) {
    rotationLeft(node->getLeft());
    rotationRight(node);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
