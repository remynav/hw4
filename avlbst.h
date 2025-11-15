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
    void rotateRight(AVLNode<Key, Value>* x);
    void rotateLeft(AVLNode<Key, Value>* x);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* x);
    void removeFix(AVLNode<Key, Value>* x, int diff);
    int height(AVLNode<Key, Value>* node) const;

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    //empty tree
    if(this->root_ == nullptr){
      this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
      return;
    }
    
    //find spot
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while(temp !=nullptr){
      parent = temp;

      if(new_item.first < temp->getKey()){
        temp = temp->getLeft();
      }
      else if(new_item.first > temp->getKey()){
        temp = temp->getRight();
      }
      else{
        //key already in tree
        temp->setValue(new_item.second);
        return;
      }
    }

    AVLNode<Key, Value>* newN = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    if(new_item.first < parent->getKey()){
      parent->setLeft(newN);
    }
    else{
      parent->setRight(newN);
    }

    insertFix(parent, newN);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* y){
  if(y == nullptr) return;
  
  //left child
  AVLNode<Key, Value>* x = y->getLeft();
  if(x == nullptr) return;
  
  //right subtree of left child
  AVLNode<Key, Value>* B = x->getRight();
  AVLNode<Key, Value>* p = y->getParent();

  //rotate
  x->setRight(y);
  y->setParent(x);

  y->setLeft(B);
  if(B!= nullptr){
    B->setParent(y);
  }
  
  x->setParent(p);

  //if(B != nullptr){
  //  B->setParent(y);
  //}

  if(p != nullptr){
    if(p->getLeft() == y){
      p->setLeft(x);
    }
    else{
      p->setRight(x);
    }
  }
  else{
    this->root_ = x;
  }


  //update balance
  int yleftH = height(y->getLeft());
  int yrightH = height(y->getRight());
  y->setBalance(yleftH - yrightH);

  int xleftH = height(x->getLeft());
  int xrightH = height(x->getRight());
  x->setBalance(xleftH - xrightH);

}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x){
  if(x == nullptr) return;
  //right child
  AVLNode<Key, Value>* c1 = x->getRight();
  if(c1 == nullptr) return;
  //left subtree of right child
  AVLNode<Key, Value>* B = c1->getLeft();
  AVLNode<Key, Value>* p = x->getParent();

  c1->setLeft(x);
  x->setRight(B);

  if(B!= nullptr){
    B->setParent(x);
  }
  
  c1->setParent(p);
  x->setParent(c1);

  if(p != nullptr){
    if(p->getLeft() == x){
      p->setLeft(c1);
    }
    else{
      p->setRight(c1);
    }
  }
  else{
    this->root_ = c1;
  }


  //update balance
  int xleftH = height(x->getLeft());
  int xrightH = height(x->getRight());
  x->setBalance(xleftH - xrightH);

  int c1leftH = height(c1->getLeft());
  int c1rightH = height(c1->getRight());
  c1->setBalance(c1leftH - c1rightH);

}

template<class Key, class Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value>* node) const
{
  if(node == nullptr) return 0;

  int leftH = height(node->getLeft());
  int rightH = height(node->getRight());
  
  int maxH;
  if(leftH > rightH){
    maxH = leftH;
  }
  else{
    maxH = rightH;
  }

  return maxH + 1;

}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* p ){
  
  while(parent != nullptr){
    int leftH = height(parent->getLeft());
    int rightH = height(parent->getRight());
    int balance = leftH - rightH;
    parent->setBalance(balance);

    //left heavy
    if(balance >1){
      AVLNode<Key,Value>* leftChild = static_cast<AVLNode<Key, Value>*>(parent->getLeft());

      int childBalance = height(leftChild->getLeft()) - height(leftChild->getRight());

      if(childBalance >= 0){
        //LL
        rotateRight(parent);
      }
      else{
        //LR
        rotateLeft(leftChild);
        rotateRight(parent);
      }
      break;
    }

    

    //right heavy
    else if(balance < -1){
      AVLNode<Key,Value>* rightChild = static_cast<AVLNode<Key, Value>*>(parent->getRight());
      
      int childBalance = height(rightChild->getLeft()) - height(rightChild->getRight());

      if(childBalance <= 0){
        //RR
        rotateLeft(parent);
      }
      else{
        //RL
        rotateRight(rightChild);
        rotateLeft(parent);
      }
      break;
    }

    
    if(balance == 0){
      break;
    }
  
    //keep going up tree
    //p = parent;
    parent = static_cast<AVLNode<Key, Value>*>(parent->getParent());

  }
 
}



/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    //find node
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    
    // key not in tree
    if(node == nullptr){
      return; 
    }

    //if two children, swap with predecessor
    if(node->getLeft() != nullptr && node->getRight() != nullptr){
      AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
      nodeSwap(node, pred);
    }

    //node has 0 or 1 child
    AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(node->getParent());
    AVLNode<Key, Value>* child = nullptr;

    if(node->getLeft() != nullptr){
      child = static_cast<AVLNode<Key, Value>*>(node->getLeft());
    }
    else if (node->getRight() != nullptr){
      child = static_cast<AVLNode<Key, Value>*>(node->getRight());
    }

    if(child != nullptr){
      child->setParent(parent);
    }

    if(parent == nullptr){
      this->root_ = child;
    }
    else if(parent->getLeft() == node){
      parent->setLeft(child);
      removeFix(parent, 1);
    }
    else{
      parent->setRight(child);
      removeFix(parent, -1);
    }

    delete node;

}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* p, int diff){
  //rebalance bubbling up
  while(p != nullptr){
    //balance of p
    int leftH = height(p->getLeft());
    int rightH = height(p->getRight());
    int balance = leftH - rightH;
    p->setBalance(balance);

    //left heavy
    if(balance > 1){
      AVLNode<Key, Value>* leftChild = static_cast<AVLNode<Key, Value>*>(p->getLeft());
      
      int childBalance = height(leftChild->getLeft()) - height(leftChild->getRight());

      if(childBalance >=0){
        //LL
        rotateRight(p);
      }
      else{
        //LR
        rotateLeft(leftChild);
        rotateRight(p);
      }
    }

    //right heavy
    else if(balance < -1){
      AVLNode<Key, Value>* rightChild = static_cast<AVLNode<Key, Value>*>(p->getRight());
      int childBalance = height(rightChild->getLeft()) - height(rightChild->getRight());

      if(childBalance <= 0){
        //RR
        rotateLeft(p);
      }
      else{
        //RL
        rotateRight(rightChild);
        rotateLeft(p);
      }
    }

    //keep walking up
    AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(p->getParent());

    if(parent == nullptr) break;

    if(parent->getLeft() == p){
      diff = 1;
    }
    else{
      diff = -1;
    }
    p = parent;

  }
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
