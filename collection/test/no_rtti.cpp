#include <stdio.h>
#include "no_rtti.h"

// compile with
// g++ no_rtti.cpp -std=c++03 -fno-rtti

void Sub::dox(){
  printf("do\n");
}

void f(Base &b){
  b.dox();
}
int main(){
  Sub s;
  f(s);
}
