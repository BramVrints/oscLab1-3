#include<stdio.h>
#include<stdlib.h>

// Struct to hold the data and the pointer to the next element.
struct element {
    char data;
    struct element *next;
};

void push(char data, struct element **stack) {
    struct element *Element = (struct element *) malloc(sizeof(struct element));
    Element->data = data;
    Element->next = *stack;
    (*stack) = Element;
    printf("Element pushed: %c\n", (*stack)->data);
}

void pop(struct element **stack) {
    if (*stack != NULL) {
        printf("Element popped: %c\n", (*stack)->data);
        struct element *tempPtr = *stack;
        *stack = (*stack)->next;
        free(tempPtr);
    } else {
        printf("The stack is empty.\n");
    }
}

//Answer to question 3 (on the blackboard)
void peek(struct element* stack){
      if(stack != NULL){

          printf("Element on top: %c\n", stack -> data);
          }
      else{
            printf("The stack is empty.\n");
          }
}


int main() {
    struct element *root = NULL;

    push('a', &root);//question 1: how to push data || what if root = null ?
    push('b', &root);
    push('c', &root);
    //question 3: implement a peek() to peek at the top element.
    peek(root);
    pop(&root);//question 2: how to pop data
    pop(&root);
    pop(&root);
    peek(root);

    exit(0);

}
