#include <stdio.h>
#include <stdlib.h>
#include "binary_tree.h"

int main() {
    binTree * tree = createTree();
    printf("Print an empty tree");
    printTree(tree);
    
    //Insertion check
    printf("How many ints are going to be inserted?\n");
    int n, i , input;
    scanf("%d", &n);
    for( i = 0; i<n; i++){
        printf("Enter a integer: \n");
        scanf("%d",&input);
        insertValue(tree, input);
        printTree(tree);
    }
    
    //Search check
    printf("This is the current tree:");
    printTree(tree);
    printf("How many times of search is wanted? \n");
    int a, b ,c;
    scanf("%d", &a);
    for(b = 0; b<a; b++){
        printf("Enter the search target: \n");
        scanf("%d", &c);
        if (search(tree, c) == 1){
            printf("Find! \n");
        }
        else{
            printf("Not there!\n");
        }
        printTree(tree);
    }
    
    //delete check
    printf("This is the current tree:");
    printTree(tree);
    printf("How many times of deleteh is wanted? \n");
    int d, e ,f;
    scanf("%d", &d);
    for(e = 0; e<d; e++){
        printf("Enter the search target: \n");
        scanf("%d", &f);
        if (deleteNode(tree, f)){
            printf("Find! Delete Sucessful!");
        }
        else{
            printf("Not there! Can't delete");
        }
        printTree(tree);
    }
    
    //free tree check
    printf("Free the tree");
    freeTree(tree);
    printf("Free? %p\n", tree);
    printTree(tree);

}
