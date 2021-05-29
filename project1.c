#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_PAS_LEN 500

// Instruction register.
int op;
int l;
int m;
// base pointer and stack pointer.
int bp;
int sp;
int pc;
// The posistion where the stack starts, use for print stack.
int stack_start_point;
char name[4];
// Path array.
int pas[MAX_PAS_LEN];

int base(int L);

int main(void)
{
  FILE *fp;
  fp = fopen("input.txt", "r");
  int halt = 1;

  // Initialize path array.
  for (int i = 0; i < MAX_PAS_LEN; i++)
    pas[i] = 0;

  // Read the input to an array.
  int index = 0;
  while (fscanf(fp, "%d", &pas[index]) == 1)
  {
    index++;
  }

  // Initialize bp, sp, pc.
  bp = index;
  sp = index-1;
  pc = 0;
  stack_start_point = bp;

  printf("                    PC   BP   SP   stack\n");
  printf("Initial values:     %d    %d   %d\n",pc,bp,sp);

  int old_pc = pc;

  while (halt == 1)
  {
    // Fetch
    op = pas[pc];
    l = pas[pc + 1];
    m = pas[pc + 2];
    pc += 3;
    // Execute
    switch (op)
    {
      // 01 – LIT 0, M Pushes a constant value (literal) M onto the stack
      case 1:
        strcpy(name,"LIT");
        sp++;
        pas[sp] = m;
        break;
      // 02 – OPR 0, M Operation to be performed on the data at the top of the stack.
      case 2:
        switch(m)
        {
           case 0:
             strcpy(name,"RTN");
             sp = bp - 1;
             pc = pas[sp + 3];
             bp = pas[sp + 2];
             break;
           case 1:
             strcpy(name,"NEG");
             pas[sp] = -1 * pas[sp];
             break;
           case 2:
             strcpy(name,"ADD");
             sp --;
             pas[sp] += pas[sp + 1];
             break;
           case 3:
             strcpy(name,"SUB");
             sp --;
             pas[sp] -= pas[sp + 1];
             break;
           case 4:
             strcpy(name,"MUL");
             sp --;
             pas[sp] *= pas[sp + 1];
             break;
           case 5:
             strcpy(name,"DIV");
             sp --;
             pas[sp] /= pas[sp + 1];
             break;
           case 6:
             strcpy(name,"ODD");
            pas[sp] = pas[sp] % 2;
            break;
           case 7:
             strcpy(name,"MOD");
             sp --;
             pas[sp] %= pas[sp + 1];
             break;
           case 8:
             strcpy(name,"EQL");
             sp --;
             pas[sp] = (pas[sp] == pas[sp + 1]);
             break;
           case 9:
             strcpy(name,"NEQ");
             sp --;
             pas[sp] = (pas[sp] != pas[sp + 1]);
             break;
           case 10:
             strcpy(name,"LSS");
             sp --;
             pas[sp] = (pas[sp] < pas[sp + 1]);
             break;
           case 11:
             strcpy(name,"LEQ");
             sp --;
             pas[sp] = (pas[sp] <= pas[sp + 1]);
             break;
           case 12:
             strcpy(name,"GTR");
             sp --;
             pas[sp] = (pas[sp] > pas[sp + 1]);
             break;
           case 13:
             strcpy(name,"GEQ");
             sp --;
             pas[sp] = (pas[sp] >= pas[sp + 1]);
             break;
          default:
             printf("Wrong M!\n");
             return 0;
        }
        break;
      // 03 – LOD L, M Load value to top of stack from the stack location at
      // set M from L lexicographical levels down
      case 3:
        strcpy(name,"LOD");
        sp++;
        pas[sp] = pas[base(l) + m];
        break;
      // 04 – STO L, M Store value at top of stack in the stack location at offset M
      // from L lexicographical levels down
      case 4:
        strcpy(name,"STO");
        pas[base(l) + m] = pas[sp];
        sp--;
        break;
      // 05 – CAL L, M Call procedure at code index M (generates new
      // Activation Record and PC <- M)
      case 5:
        strcpy(name,"CAL");
        pas[sp + 1] = base(l); // static link (SL)
        pas[sp + 2] = bp; // dynamic link (DL)
        pas[sp + 3] = pc; // return address (RA)
        bp = sp + 1;
        pc = m;
        break;
      // 06 – INC 0, M Allocate M memory words (increment SP by M). First four
      // are reserved to Static Link (SL), Dynamic Link (DL), Return Address (RA), and Parameter (P)
      case 6:
        strcpy(name,"INC");
        sp += m;
        break;
      // 07 – JMP 0, M Jump to instruction M (PC <- M)
      case 7:
        strcpy(name,"JMP");
        pc = m ;
        break;
      // 08 – JPC 0, M Jump to instruction M if top stack element is 0
      case 8:
        strcpy(name,"JPC");
        if (pas[sp] == 1)
          pc = m;
        sp--;
        break;
      // 09 – SYS 0, 1 Write the top stack element to the screen
      //      SYS 0, 2 Read in input from the user and store it on top of the stack
      //      SYS 0, 3 End of program (Set Halt flag to zero)
      case 9:
        strcpy(name,"SYS");
        if (m == 1)
        {
          printf("Output result is: %d\n", pas[sp]);
          sp--;
        }
        else if (m == 2)
        {
          sp++;
          printf("Please Enter an Integer: ");
          scanf("%d", &pas[sp]);
        }
        else
          halt = 0;
        break;
      default:
        printf("Wrong op code!\n");
        return 0;
    }

    // Print Required Output
    printf("%2d   %s   %d   %2d   %2d   %2d   %d   ",old_pc,name,l,m,pc,bp,sp);
    old_pc = pc;
    for (int i = stack_start_point; i <= sp; i++)
    {
      if (i == bp && i != stack_start_point)
        printf("| ");
      printf("%d ", pas[i]);
    }

    printf("\n");
  }

  return 0;
}

int base(int L)
{
  int arb = bp; // arb = activation record base

  while (L > 0) //find base L levels down
  {
    arb = pas[arb];
    L--;
  }

  return arb;
}
