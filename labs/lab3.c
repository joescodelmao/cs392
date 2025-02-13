#include <stdio.h>

//type == 1: int
// else float

struct ArrayList{
    void* array;
    void (*init_arr)(struct ArrayList* parr, int len, int type);
    void (*destroy)(struct ArrayList* parr);
}

void initialize(struct ArrayList* parr, int len, int type){
    if (type == 1) parr ->array = malloc(len * sizeof(int));
    else parr -> array = malloc(len * sizeof(float));
}

int main(){
    struct ArrayList arr;
    arr.init_arr = initialize;
    arr.init_arr(&arr, 10, 1);

}