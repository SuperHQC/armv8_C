//ArmV8 simulator C version

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// struct R {
//   unsigned int rd : 5;
//   unsigned int rn : 5;
//   unsigned int shamt : 6;
//   unsigned int rm : 5;
//   unsigned int opcode : 11;
// };
//
// struct I {
//   unsigned int rd : 5;
//   unsigned int rn : 5;
//   unsigned int ALU_immediate : 12;
//   unsigned int opcode : 10;
// };
//
// struct D {
//   unsigned int rt : 5;
//   unsigned int rn : 5;
//   unsigned int op : 2;
//   unsigned int DT_address : 9;
//   unsigned int opcode : 11;
// };
//
// struct B {
//   unsigned int BR_address : 26;
//   unsigned int opcode : 6;
// };
//
// struct CB {
//   unsigned int rt : 5;
//   unsigned int COND_BR_address :19;
//   unsigned int opcode : 8;
// };
//
// struct IW {
//   unsigned int rd : 5;
//   unsigned int MOV_immediate : 16;
//   unsigned int opcode : 11;
// };

struct Reg{
  int name;
  int value;
};

struct Mem{
  int addr;
  char value;
};


struct instruction{
  int addr;
  char label[32];
  char op[32];
  char v1[32];
  char v2[32];
  char v3[32];
  char comment[1000];
};

//core
FILE *fp;
struct Reg *registers;
struct Mem *stack;
// unsigned int *code;

struct instruction *codes;

int N=0;
int Z=0;
int C=0;
int V=0;

int PC;
int lines=0;
int temp;
//functions

/*
*    Read file function
*/
void readFile(FILE * fp){
  char c;
  char word[255];
  int n=0;
  int vi=0;
  while((c = fgetc(fp)) != EOF){
    if((c<32 ||c>=127)&&c!='\n'){
      c=fgetc(fp);  //read file char by char
    }
    if(c==','){ //change ',' to ' '
      c=' ';
    }
    if(c == '@'){ //deal with comment and store them in comment field of codes
      while(c != '\n'){
        // printf("%c\n",c );
        word[n]=c;
        n=n+1;
        c = fgetc(fp);
      }
      word[n]='\0';
      n=0;
      strcpy(codes[lines].comment,word);
    }
    else if(c == '['){  //deal with shifter
      while(c != ']'){
        c=toupper(c);
        word[n]=c;
        n=n+1;
        c=fgetc(fp);
      }
      word[n]=c;
      n+=1;
      c = fgetc(fp);
      word[n]='\0';
    }
    if(c == ' '){ //seperate codes by ' ' and store them in different fields
      word[n]='\0';
      if(strncmp(word,"\0",1)!=0){

        if(vi==0){
          strcpy(codes[lines].op,word);
        }
        else if(vi==1){
          strcpy(codes[lines].v1,word);
        }
        else if(vi==2){
          strcpy(codes[lines].v2,word);
        }
        else if(vi==3){
          strcpy(codes[lines].v3,word);
        }
        vi++;
    }
    n=0;
  }
    else if(c == ':'){  //labels
      word[n]='\0';
      n=0;
      strcpy(codes[lines].label,word);
    }
    else if(c == '\n'){ //new lines
      word[n]='\0';
      if(strncmp(word,"\0",1)!=0){
        //  printf("%s\n",word );
        if(vi==0){
          strcpy(codes[lines].op,word);
        }
        else if(vi==1){
          strcpy(codes[lines].v1,word);
        }
        else if(vi == 2){
          strcpy(codes[lines].v2,word);
        }
        else if(vi ==3){
          strcpy(codes[lines].v3,word);
        }

      }
      n=0;
      lines++;
      vi=0;
    }
    else{ //build words
      c=toupper(c);
      word[n]=c;
      n++;
    }
  }
}

void mov(struct instruction ins){ //mov instruction
  int *dst;//destination pointer
  int value=0;
  char buff[32];
  int i;
  if(ins.v1[0]=='R'){   //read first variable and translate
    for(i=0; i<strlen(ins.v1)-1; i++){
      buff[i]=ins.v1[i+1];
    }
    buff[i]='\0';
    i=0;
    for(int i=0;i<32;i++){
      if(registers[i].name==atol(buff)){
        dst=&(registers[i].value);
        PC=PC+4;
      }
    }
  }
  else if(strncmp(ins.v1, "PC",2)==0){
    dst=&PC;
  }

  if(ins.v2[0]!='#'){ //read second variable and translate
    if(ins.v2[0]=='R'){
      for(i=0; i<strlen(ins.v2)-1; i++){
        buff[i]=ins.v2[i+1];
      }
      buff[i]='\0';
      i=0;
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          value=registers[i].value;
        }
      }
    }
    else if(strncmp(ins.v2,"LR",2)==0){
      value = registers[30].value;
    }
  }
  else{
    for(i=0; i<strlen(ins.v2)-1; i++){
      buff[i]=ins.v2[i+1];
    }
    buff[i]='\0';
    i=0;
    value = atol(buff);
  }

  *dst=value;
}

void cmp(struct instruction ins){ //compare two variables and setup flags
  int v1;
  int v2;
  int i;
  char buff[32];
  if(ins.v1[0]!='#'){//read first variable and
    if(ins.v1[0]=='R'){
      for(i=0; i<strlen(ins.v1)-1; i++){
        buff[i]=ins.v1[i+1];
      }
      buff[i]='\0';
      i=0;
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          v1=registers[i].value;
        }
      }
    }
  }
  else{
    for(i=0; i<strlen(ins.v1)-1; i++){
      buff[i]=ins.v1[i+1];
    }
    buff[i]='\0';
    i=0;
    v1 = atol(buff);
  }

  if(ins.v2[0]!='#'){ //read second variable
    if(ins.v2[0]=='R'){
      for(i=0; i<strlen(ins.v2)-1; i++){
        buff[i]=ins.v2[i+1];
      }
      buff[i]='\0';
      i=0;
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          v2=registers[i].value;
        }
      }
    }
  }
  else{//if the variable is immediate
    for(i=0; i<strlen(ins.v2)-1; i++){
      buff[i]=ins.v2[i+1];
    }
    buff[i]='\0';
    i=0;
    v2 = atol(buff);
  }
  int r = v1-v2;
  if(r==0){
    Z=1;
    N=0;
  }
  else if (r>0){
    Z=0;
    N=0;
  }
  else if(r<0){
    Z=0;
    N=1;
  }
  PC=PC+4;
}

void b(struct instruction ins){ //branch to the address
  for(int i=0;i<lines;i++){
    if(strncmp(codes[i].label,ins.v1,16)==0){
      PC=codes[i].addr;
    }
  }
}

void bl(struct instruction ins){  //branch to the address
  registers[30].value = ins.addr+4; //store the next pc value tu r30
  b(ins);
}

void b_ge(struct instruction ins){//branch when grate or equal
  if(Z==1 || (N==0 && Z==0)){
    b(ins);
  }
  else{
    PC+=4;
  }
}

void sub(struct instruction ins){ //substract two variables store result to v1
  int *dst;
  int v1=0;
  int v2=0;
  char buff[32];
  int i;
  if(ins.v1[0]=='R'){
    for(i=0; i<strlen(ins.v1)-1; i++){
      buff[i]=ins.v1[i+1];
    }
    buff[i]='\0';
    i=0;
    for(int i=0;i<32;i++){
      if(registers[i].name==atol(buff)){
        dst=&(registers[i].value);
      }
    }
  }
  else if(strncmp(ins.v1, "SP",2)==0){
    dst=&(registers[28].value);
  }

  if(ins.v2[0]!='#'){//read first variable
    if(ins.v2[0]=='R'){
      for(i=0; i<strlen(ins.v2)-1; i++){
        buff[i]=ins.v2[i+1];
      }
      buff[i]='\0';
      i=0;
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          v1=registers[i].value;
        }
      }
    }
    else if(strncmp(ins.v2, "SP",2)==0){
      v1=registers[28].value;
    }
  }
  else{
    for(i=0; i<strlen(ins.v2)-1; i++){
      buff[i]=ins.v2[i+1];
    }
    buff[i]='\0';
    i=0;
    v1 = atol(buff);
  }

  if(ins.v3[0]!='#'){ //read second variable
    if(ins.v3[0]=='R'){
      for(i=0; i<strlen(ins.v3)-1; i++){
        buff[i]=ins.v3[i+1];
      }
      buff[i]='\0';
      i=0;
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          v2=registers[i].value;
        }
      }
    }
    else if(strncmp(ins.v3, "SP",2)==0){
      v2=registers[28].value;
    }
  }
  else{
    for(i=0; i<strlen(ins.v3)-1; i++){
      buff[i]=ins.v3[i+1];
    }
    buff[i]='\0';
    i=0;
    v2 = atol(buff);
  }

  *dst=v1-v2;
  PC=PC+4;
}

void add(struct instruction ins){ //add two variables
  int *dst;
  int v1=0;
  int v2=0;
  char buff[32];
  int i;
  if(ins.v1[0]=='R'){ //read the dst
    for(i=0; i<strlen(ins.v1)-1; i++){
      buff[i]=ins.v1[i+1];
    }
    buff[i]='\0';
    i=0;
    for(int i=0;i<32;i++){
      if(registers[i].name==atol(buff)){
        dst=&(registers[i].value);
      }
    }
  }
  else if(strncmp(ins.v1, "SP",2)==0){
    dst=&(registers[28].value);
  }

  if(ins.v2[0]!='#'){ //read first variable
    if(ins.v2[0]=='R'){
      for(i=0; i<strlen(ins.v2)-1; i++){
        buff[i]=ins.v2[i+1];
      }
      buff[i]='\0';
      i=0;
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          v1=registers[i].value;
        }
      }
    }
    else if(strncmp(ins.v2, "SP",2)==0){
      v1=registers[28].value;
    }
  }
  else{
    for(i=0; i<strlen(ins.v2)-1; i++){
      buff[i]=ins.v2[i+1];
    }
    buff[i]='\0';
    i=0;
    v1 = atol(buff);
  }

  if(ins.v3[0]!='#'){ //read second variable
    if(ins.v3[0]=='R'){
      for(i=0; i<strlen(ins.v3)-1; i++){
        buff[i]=ins.v3[i+1];
      }
      buff[i]='\0';
      i=0;
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          v2=registers[i].value;
        }
      }
    }
    else if(strncmp(ins.v3, "SP",2)==0){
      v2=registers[28].value;
    }
  }
  else{
    for(i=0; i<strlen(ins.v3)-1; i++){
      buff[i]=ins.v3[i+1];
    }
    buff[i]='\0';
    i=0;
    v2 = atol(buff);
  }

  *dst=v1+v2;
  PC=PC+4;
}

void mul(struct instruction ins){ //multiply
  int *dst;
  int v1=0;
  int v2=0;
  char buff[32];
  int i;
  if(ins.v1[0]=='R'){ //read dst
    for(i=0; i<strlen(ins.v1)-1; i++){
      buff[i]=ins.v1[i+1];
    }
    buff[i]='\0';
    i=0;
    for(int i=0;i<32;i++){
      if(registers[i].name==atol(buff)){
        dst=&(registers[i].value);
      }
    }
  }
  else if(strncmp(ins.v1, "SP",2)==0){
    dst=&(registers[28].value);
  }

  if(ins.v2[0]!='#'){ //read first var
    if(ins.v2[0]=='R'){
      for(i=0; i<strlen(ins.v2)-1; i++){
        buff[i]=ins.v2[i+1];
      }
      buff[i]='\0';
      i=0;
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          v1=registers[i].value;
        }
      }
    }
    else if(strncmp(ins.v2, "SP",2)==0){
      v1=registers[28].value;
    }
  }
  else{
    for(i=0; i<strlen(ins.v2)-1; i++){
      buff[i]=ins.v2[i+1];
    }
    buff[i]='\0';
    i=0;
    v1 = atol(buff);
  }

  if(ins.v3[0]!='#'){ //read second var
    if(ins.v3[0]=='R'){
      for(i=0; i<strlen(ins.v3)-1; i++){
        buff[i]=ins.v3[i+1];
      }
      buff[i]='\0';
      i=0;
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          v2=registers[i].value;
        }
      }
    }
    else if(strncmp(ins.v3, "SP",2)==0){
      v2=registers[28].value;
    }
  }
  else{
    for(i=0; i<strlen(ins.v3)-1; i++){
      buff[i]=ins.v3[i+1];
    }
    buff[i]='\0';
    i=0;
    v2 = atol(buff);
  }

  *dst=v1*v2;
  PC=PC+4;
}

void str(struct instruction ins){ //store to register
  int *dst;
  int *src;
  int shift;

  int i;
  char buff[32];
  if(ins.v1[0]=='R'){ //read src address
    for(i=0; i<strlen(ins.v1)-1; i++){
      buff[i]=ins.v1[i+1];
    }
    buff[i]='\0';
    for(int i=0;i<32;i++){
      if(registers[i].name==atol(buff)){
        src=&(registers[i].value);
      }
    }
  }
  else if(strncmp(ins.v1, "SP",2)==0){
    src=&(registers[28].value);
  }
  else if(strncmp(ins.v1, "LR",2)==0){
    src=&(registers[30].value);
  }
  i=0;

  if(ins.v2[0]=='['){ //deal with address shift

    while(ins.v2[i+1]!=','){

      buff[i]=ins.v2[i+1];
      i++;
    }
    buff[i]='\0';

    i++;

    //dst
    if(buff[0]=='R'){
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          dst=&(registers[i].value);
        }
      }
    }
    else if(strncmp(buff, "SP",2)==0){
      dst=&(registers[28].value);
    }
    else if(strncmp(buff, "LR",2)==0){
      dst=&(registers[30].value);
    }
    int j=i+1;
    i=0;

    while(ins.v2[j]!=']'){
      while(ins.v2[j]==' '){
        j++;
      }
      buff[i]=ins.v2[j];
      i++;
      j++;
    }
    buff[i]='\0';
    i=0;

    //#
    if(buff[0]!='#'){ //calculate the value in []
      if(buff[0]=='R'){
        for(i=0; i<strlen(buff)-1; i++){
          buff[i]=buff[i+1];
        }
        buff[i]='\0';
        i=0;
        printf("%s\n","H" );
        for(int i=0;i<32;i++){
          if(registers[i].name==atol(buff)){
            shift=registers[i].value;
          }
        }
      }
      else if(strncmp(buff,"LR",2)==0){
        shift = registers[30].value;
      }
    }
    else{
      for(i=0; i<strlen(buff)-1; i++){
        buff[i]=buff[i+1];
      }
      buff[i]='\0';
      i=0;
      shift = atoi(buff);
    }
  }

  int temp;
  temp=*dst+shift;

  //store value in little endian
  for(int i=0;i<8000;i++){
    if(stack[i].addr==temp){
      stack[i].value=((char *)src)[0];
      stack[i-1].value=((char *)src)[1];
      stack[i-2].value=((char *)src)[2];
      stack[i-3].value=((char *)src)[3];
    }
  }
  PC=PC+4;
}

void ldr(struct instruction ins){ //load value from address to register
  int *dst;
  int *src;
  int shift;

  int i;
  char buff[32];
  if(ins.v1[0]=='R'){
    for(i=0; i<strlen(ins.v1)-1; i++){
      buff[i]=ins.v1[i+1];
    }
    buff[i]='\0';
    for(int i=0;i<32;i++){
      if(registers[i].name==atol(buff)){
        dst=&(registers[i].value);
      }
    }
  }
  else if(strncmp(ins.v1, "SP",2)==0){
    dst=&(registers[28].value);
  }
  else if(strncmp(ins.v1, "LR",2)==0){
    dst=&(registers[30].value);
  }
  i=0;
  if(ins.v2[0]=='['){
    while(ins.v2[i+1]!=','){
      buff[i]=ins.v2[i+1];
      i++;
    }
    buff[i]='\0';
    i++;
    //src
    if(buff[0]=='R'){
      for(int i=0;i<32;i++){
        if(registers[i].name==atol(buff)){
          src=&(registers[i].value);
        }
      }
    }
    else if(strncmp(buff, "SP",2)==0){
      src=&(registers[28].value);
    }
    else if(strncmp(buff, "LR",2)==0){
      src=&(registers[30].value);
    }
    int j=i+1;
    i=0;
    while(ins.v2[j]!=']'){
      while(ins.v2[j]==' '){
        j++;
      }
      buff[i]=ins.v2[j];
      i++;
      j++;
    }
    buff[i]='\0';
    i=0;
    //#
    if(buff[0]!='#'){
      if(buff[0]=='R'){
        for(i=0; i<strlen(buff)-1; i++){
          buff[i]=buff[i+1];
        }
        buff[i]='\0';
        i=0;
        for(int i=0;i<32;i++){
          if(registers[i].name==atol(buff)){
            shift=registers[i].value;
          }
        }
      }
      else if(strncmp(buff,"LR",2)==0){
        shift = registers[30].value;
      }
    }
    else{
      for(i=0; i<strlen(buff)-1; i++){
        buff[i]=buff[i+1];
      }
      buff[i]='\0';
      i=0;
      shift = atol(buff);
    }



  }
  int temp;
  temp=*src+shift;
  for(int i=0;i<8000;i++){
    if(stack[i].addr==temp){
      ((char *)dst)[0]=stack[i].value;
      ((char *)dst)[1]=stack[i-1].value;
      ((char *)dst)[2]=stack[i-2].value;
      ((char *)dst)[3]=stack[i-3].value;
    }
  }
  PC=PC+4;
}

void swi(struct instruction ins){ //function call
  if(strncmp(ins.v1,"0X6B",32)==0){
    printf("%d\n",registers[registers[0].value].value );
    PC=PC+4;
  }
  else if(strncmp(ins.v1,"0X11",32)==0){
    exit(0);
  }
}

void execute(struct instruction ins){ //execute instruction
  if(strncmp(ins.op,"MOV",5)==0){
    mov(ins);
  }
  else if(strncmp(ins.op,"BL",5)==0){
    bl(ins);
  }
  else if(strncmp(ins.op,"SWI",5)==0){
    swi(ins);
  }
  else if(strncmp(ins.op,"SUB",5)==0){
    sub(ins);
  }
  else if(strncmp(ins.op,"ADD",5)==0){
    add(ins);
  }
  else if(strncmp(ins.op,"STR",5)==0){
    str(ins);
  }
  else if(strncmp(ins.op,"LDR",5)==0){
    ldr(ins);
  }
  else if(strncmp(ins.op,"B.GE",5)==0){
    b_ge(ins);
  }
  else if(strncmp(ins.op,"B",5)==0){
    b(ins);
  }
  else if(strncmp(ins.op,"MUL",5)==0){
    mul(ins);
  }
  else if(strncmp(ins.op,"CMP",5)==0){
    cmp(ins);
  }
}


//print
void printIns(int index){ //print single instruction
  printf("%08x\t%s\t%s\t\t%s\t\t%s\t\t%s\t%s\n",codes[temp].addr ,codes[temp].label,codes[temp].op, codes[temp].v1,codes[temp].v2, codes[temp].v3,codes[temp].comment);
}

void printReg(){  //print registers
  for(int i=0; i<4; i++){
    for(int j=0; j<8;j++){
      if(i*8+j==28 || i*8+j==30){
        printf("R%d: %08x\t", registers[i*8+j].name,registers[i*8+j].value);
      }
      else{
        printf("R%d: %d\t", registers[i*8+j].name,registers[i*8+j].value);
      }
    }
    printf("\n");
  }
}

void printStack(){  //print stack
  for (size_t i = 0; i < 40; i++) {
    if(stack[i].addr>=registers[28].value)
    printf("%08x\t%02x\n", stack[i].addr,stack[i].value);
  }
}

//init
//initiate enviroment, reset registers to 0;
//stack size 8000 bytes
//maximum code length 1000 lines
void init(){
  registers=(struct Reg *)malloc(32*sizeof(struct Reg));
  stack = (struct Mem *)malloc(8000*sizeof(struct Mem));
  codes =(struct instruction*)malloc(1000*sizeof(struct instruction));
  for(int i=0; i<32; i++){
    registers[i].value=0;
    registers[i].name=i;
  }
  for(int i=0; i<8000; i++){
    stack[i].value=0;
    stack[i].addr =0x7ffffffc-i;
  }
  for(int i=0; i<1000;i++){
    codes[i].addr=0x4000 + 4*i;
  }
  registers[28].value=stack[0].addr;
  PC=codes[0].addr;
}



int main(int argc, char *argv[]){
  fp=fopen(argv[1],"r");

  init();

  readFile(fp);
  fclose(fp);
#ifdef P
  for(int x=0; x< lines; x++){
    printf("%08x\t%s\t%s\t\t%s\t\t%s\t\t%s\t%s\n",codes[x].addr ,codes[x].label,codes[x].op, codes[x].v1,codes[x].v2, codes[x].v3,codes[x].comment);
  }
#endif

  char input[255];
  while(PC<0x4000+4*lines){
    for(int i=0;i<1000;i++){
      if(codes[i].addr==PC){
        temp=i;
        break;
      }
    }
#ifdef P
    printf("%s", "Next step press enter: ");
    scanf("%s", input);
    printIns(temp);
#endif
    if(strlen(codes[temp].op)>1){
      execute(codes[temp]);
    }
    else{
      PC=PC+4;
    }
#ifdef P
    printReg();
    printStack();
#endif
  }
  free(registers);
  free(stack);
  free(codes);
  // free(code);
  return 0;

}
