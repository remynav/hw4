#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPaths(Node * root)
{
  // Add your code below
  if(root == nullptr) return true; 

  //leaf
  if(root->left == nullptr && root->right == nullptr){
    return true;
  }

  //one side empty
  if(root->left == nullptr){
    return equalPaths(root->right);
  }
  if(root->right == nullptr){
    return equalPaths(root->left);
  }

  //check if both sides have = depth
  int leftDepth = helper(root->left);
  int rightDepth = helper(root->right);

  if(leftDepth != rightDepth){
    return false;
  }

  //recurse
  return equalPaths(root->left) && equalPaths(root->right);

}

//return depth of subtree
int helper(Node* n){
  if(n == nullptr) return 0;

  int leftDepth = helper(n->left);
  int rightDepth = helper(n->right); 

  if(leftDepth > rightDepth){
    return leftDepth + 1;
  }
  else{
    return rightDepth + 1;
  }

}

