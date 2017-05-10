//
//  binary_tree_header.h
//  
//
//  Created by Anthony Xu on 4/26/17.
//
//

//#ifndef _binary_tree_header_h
//#define _binary_tree_header_h
typedef struct treeNode {
    int data;
    struct treeNode* parent;
    struct treeNode* left;
    struct treeNode* right;
}tNode;

typedef struct binaryTree{
    tNode * root;
}binTree;


binTree * createTree();
// Create a tree with no nodes in it

void freeTree(binTree * inputTree);
// Free the given tree.

void insertValue(binTree * inputTree, int value);
// Insert the given data as a new node into a given tree

int search(binTree * inputTree, int target);
// Search the target integer in the input tree. Return 1 if
// target is found, else return 0

int deleteNode(binTree * inputTree, int target);
// Delete the first target integer it encount from the input
// tree if it exist, else return NULL.

void printTree(binTree * inputTree);
//Print out the input tree.



//#endif

//// Local allocation
//tNode n;
// 
//n.data = 7;
//
//return &n; // Bad idea  (return value of type tNode * on the stack which is garbage after return).
//
//// Global allocation
//tNode * n_p = malloc
//
//n_p -> data = 7;

