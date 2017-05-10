//Binary search tree take int as data
//author Anthony

#include <stdio.h>
#include <stdlib.h>
#include "binary_tree_header.h"


//}binTree;
////Am I just using a node?

void insertNode(tNode * inputNode, int value);
int pickSide(tNode * currentNode, int value);
void freeNode(tNode * node);
void printNode(tNode * node);
int searchNode(tNode * node, int target);
void deleteLeaf(tNode * node);
void deleteOne(tNode * node);
void deleteTwo(tNode * node);


binTree * createTree(){
    binTree * tree = (binTree *) malloc(sizeof(binTree));
    tree -> root = NULL;
    return tree;
}

void freeTree(binTree * inputTree){
    freeNode(inputTree -> root);
    free(inputTree);
    inputTree = NULL;
}

//Helper: free a node and it's children, eternally
void freeNode(tNode * node){
    if (node != NULL) {
        freeNode(node -> left);
        freeNode(node -> right);
        node -> left = NULL;
        node -> right = NULL;
        node -> parent = NULL;
        node -> data = NULL;
        free(node);
        node = NULL;
    }
}

// Insert the data into the given tree as a node
void insertValue(binTree * inputTree, int value){
    if (inputTree -> root == NULL){
        tNode* tmpNode = (tNode *)malloc(sizeof(tNode));
        tmpNode -> data = value;
        tmpNode -> parent = NULL;
        tmpNode -> left = NULL;
        tmpNode -> right = NULL;
        inputTree -> root = tmpNode;
    }
    else{
       insertNode(inputTree -> root, value);
 
    }
    }


                    
//Helper method: determine how to isert the data into a
//subtree with input node as root. 
void insertNode(tNode * inputNode, int value){
    if (inputNode != NULL) {
        tNode * next;
        if(pickSide(inputNode, value) == 1){
            next = inputNode -> left;
        }
        
        else{
            next = inputNode -> right;
        }
        
        //Check if next node is NULL or not
        if(next != NULL) {
            insertNode(next, value);
        }
        else{
            tNode* tmpNode = (tNode *)malloc(sizeof(tNode));
            tmpNode -> data = value;
            tmpNode -> parent = inputNode;
            tmpNode -> left = NULL;
            tmpNode -> right = NULL;
            if (pickSide(inputNode, value) == 1){
                inputNode -> left = tmpNode;
            }
            else{
                inputNode -> right = tmpNode;
            }
        }
     }
    
}

int search(binTree * inputTree, int target){
    if(inputTree -> root == NULL){
        return 0;
    }
    else{
        return searchNode(inputTree -> root, target);
    }
}

int searchNode(tNode * node, int target){
    if(node == NULL){
        return 0;
    }
    
    else if(target == node -> data){
        return 1;
    }
        
    else if (node -> data > target){
        return searchNode(node -> left, target);
    }
        
    else{
        return searchNode(node -> right, target);
    }
    
}

int deleteNode(binTree * inputTree, int target){
    
}

//Delete a leaf node
void deleteLeaf(tNode * node){
    tNode * parentN = node -> parent;
    if (node -> data < parentN ->data){
        parentN -> left = NULL;
    }
    else {
        parentN -> right = NULL;
    }
    node -> left = NULL;
    node -> right = NULL;
    node -> parent = NULL;
    node -> data = NULL;
    free(node);
    node = NULL;
    
}

//Delete a node with one childe
void deleteOne(tNode * node){
    tNode * parentN = node -> parent;
    
    
    tNode * child;
    if (node -> left != NULL){
        if (node -> data < parentN ->data){
            parentN -> left = node -> left;
        }
        else {
            parentN -> right = node -> left;
        }
        child = node -> left;
        child ->  parent = parentN;
    }
    
    else{
        if (node -> data < parentN ->data){
            parentN -> left = node -> right;
        }
        else {
            parentN -> right = node -> right;
        }
        child = node -> right;
        child ->  parent = parentN;
    }
    node -> left = NULL;
    node -> right = NULL;
    node -> parent = NULL;
    node -> data = NULL;
    free(node);
    node = NULL;
}

void deleteTwo(tNode node){
    tNode * parentN, childR;
    
    childR = node -> right;
    while(childR -> left != NULL){
        childR = childR -> left;
    }
    
    
}

void printTree(binTree * inputTree){
    if(inputTree -> root == NULL){
        printf("Empty tree \n");
    }
    else{
        printNode(inputTree -> root);
        printf("  \n");
    }
}

//Helper: print the subtree with the given node as root
void printNode(tNode * node){
    if (node != NULL) {
        if(node -> left != NULL){
            printf(" ( ");
            printNode(node -> left);
            printf(" ) ");
        }
        printf("%d", node -> data);
        
        if (node -> right != NULL){
            printf(" ( ");
        printNode(node -> right);
        printf(" ) ");
        }
        
    }
}

//Helper method: determine which side of the currentNode
//that the data should go
//1 means go left, -1 means go right, 0 means something
//bad happens
int pickSide(tNode * currentNode, int value){
    if (currentNode !=NULL){
        int currentValue = currentNode -> data;
        if (currentValue > value) {
            return 1;
        }
        else{
            return -1;
        }
    }
    
    else{
        return 0;
    }
    
}


