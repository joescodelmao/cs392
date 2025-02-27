/***************************************************************************
 * 
 * Copyright (c) 2024 shudong All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file demo.c
 * @author shudong(shudong)
 * @date 2024/02/01 22:30:52
 * @version $Revision$ 
 * @brief 
 *  
 **/


#include <stdio.h>


struct ST {
    int* p;
};


void swap(struct ST* pa, struct ST* pb) {

    /* Your code here  */
    /* Swaps pointers in the structs */
    int* temp = pa -> p;
    pa->p = pb->p;
    pb->p = temp;

}




void swap2(struct ST* pa, struct ST* pb){
    /* Swaps two integers pointed by the pointers */
    int temp = *(pa->p);
    *(pa->p) = *(pb->p);
    *(pb->p) = temp;

}


int main() {
    
    int a = 10;
    int b = 20;


    struct ST sta = {.p = &a};
    struct ST stb = {.p = &b};


    swap(&sta, &stb);


    printf("*(sta.p) = %d\n*(stb.p) = %d\n", *(sta.p), *(stb.p));
    /* Expected output:  20 10*/

    return 0;
}













