//ArmV8 simulator C version

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

struct R {
  unsigned int rd : 5;
  unsigned int rn : 5;
  unsigned int shamt : 6;
  unsigned int rm : 5;
  unsigned int opcode : 11;
};

struct I {
  unsigned int rd : 5;
  unsigned int rn : 5;
  unsigned int ALU_immediate : 12;
  unsigned int opcode : 10;
};

struct D {
  unsigned int rt : 5;
  unsigned int rn : 5;
  unsigned int op : 2;
  unsigned int DT_address : 9;
  unsigned int opcode : 11;
};

struct B {
  unsigned int BR_address : 26;
  unsigned int opcode : 6;
};

struct CB {
  unsigned int rt : 5;
  unsigned int COND_BR_address :19;
  unsigned int opcode : 8;
};

struct IW {
  unsigned int rd : 5;
  unsigned int MOV_immediate : 16;
  unsigned int opcode : 11;
};

//core
FILE *file;
unsigned int *Registers;
char *memory;

bool N=false;
bool Z=false;
bool C=false;
bool V=false;

unsigned int *IP0;
unsigned int *IP1;
char *SP;
unsigned int *FP;
unsigned int *LR;

//init
void init(){
  Registers=(unsigned int *)malloc(32*sizeof(unsigned int));
}


int main(int argc, char *argv[]){
  return 0;

}
