/***************************************************************************
 * 
 * Copyright (c) 2024 shudong All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 /**
 * @file lab0209-1.c
 * @author shudong(shudong)
 * @date 2024/02/09 15:18:17
 * @version $Revision$ 
 * @brief 
 *  
 **/


struct Elem {
    void* data;
};


struct ArrayList {
    struct Elem* array;

    /* Call  init to initialize every element in the array*/
    void (*init_arr)(struct ArrayList*, int, void (*)(struct Elem**));
    void (*destroy)(struct ArrayList*);
}

void initialize(struct ArrayList* p, int len, void (*fp)(struct Elem*)){
    p->array = (struct Elem*)malloc(len*sizeof(struct Elem));
    for (int i = 0; i < len; i++){
        fp(&p->array[i]);
    }
}

void init_float(struct Elem* p){
    p->data = malloc(sizeof(float));
}

void init_int(struct Elem* p){
    p->data = malloc(sizeof(int));
}

int main () {

    struct ArrayList arr;
    arr.init_arr = initialize;
    arr.init_arr(&arr, 10, init_int);


}
