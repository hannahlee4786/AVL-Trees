#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;

// Helper function that returns depth of a path
bool isDepth(Node* root, int depth, int& leafDepth) {
  if (root == NULL) {
    return true;
  }

  // Check if leaf node
  if (!root->left && !root->right) {
    if (leafDepth == -1) {
      leafDepth = depth;
      return true;
    }
    return depth == leafDepth;
  }

  // Check right and left subtrees
  return isDepth(root->left, depth + 1, leafDepth) && isDepth(root->right, depth + 1, leafDepth);
}

bool equalPaths(Node * root)
{
  if (root == NULL) {
    return true; // Empty tree is balanced
  }
  
  int leafDepth = -1;

  // Check for equal depths
  return isDepth(root, 0, leafDepth);
}

