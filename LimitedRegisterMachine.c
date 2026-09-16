#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Hello! My goal is to implement a limited register machine. As of now it just takes in sequences but once i find a more efficient way of encoding sequences it will take in 2 integers!
//For further reading, look out for the computability theory textbook by Tyler Arant at UCLA. 
//I have an early copy for a class but am not supposed to share it because its in-the-works
//however it is really helpful for understanding the concepts (as is MATH 136 at berkeley but I assume thats not accessible everyone)

//sequences are ordered lists of natural numbers, N = {0, 1, 2, ...}. 
//As the sequences we deal with will be theorhetically infinite, with non-defined indices defaulting to 0, we will represent them with list-array

typedef struct SequenceList { 
        unsigned int* entries; 
        int used; 
        int size; 
} Sequence; 

void SeqInit(Sequence* L) { 
    L->size = 1; 
    L->used = 0; 
    L->entries = malloc(sizeof(unsigned int)); 
} 

void SeqAdd(Sequence* L, unsigned int a) { 
    if (L->used == L->size) { 
        L->size *= 2; 
        L->entries = realloc(L->entries, sizeof(unsigned int) * L->size); 
    } 
    L->entries[L->used] = a; 
    L->used += 1; 
} 

void SeqArrayAdd(Sequence* L, unsigned int a[], unsigned int size) {
    for (int i = 0; i < size; i += 1) {
        SeqAdd(L, a[i]);
    }
}

#define SeqMake(name)\
    Sequence name;\
    SeqInit(&name);\

//Implementing helper functions to build up to a universal function, F(m, n): N^2 -> N, m, n in N using codes. 
//For now it is mostly an exercise as the 32 bit integer limit is reached quickly via encoding sequences being a lot of high prime powers.

bool IsPrime(unsigned int n) {
    //printf("IsPrime is run");
    if (n == 0 || n == 1 || (n % 2 == 0 && n != 2)) {return false;}
    
    if (n == 2) {return true;}

    for (int i = 3; i < sqrt(n); i += 2) {
        if (n % i == 0) {return false;}
    }
    return true;
}

unsigned int NthPrime(unsigned int n) {
    //printf("NthPrime is run\n");
    if (n == 0) {return 2;}
    n -= 1;
    for (int i = 3; i <= (int) 2 * (i * log(i) + i * log(log(i))) + 10; i += 2) {
        if (IsPrime(i)) {
            if (n == 0) {return i;}
            n -= 1;
        }
    }
}

//code of a sequence (in math notated as #seq) is the product of the (value at index n + 1)th power of the nth prime for index 0 through lh(seq)-1.
//Example: #[1, 2, 3] = 2^(1+1) * 3^(2+1) * 5^(3+1) (with 2, 3, and 5 being the 0th, 1st, and 2nd primes). The +1 in the power is to count indices with value 0 (since entries are in N = {0, 1, ...} 

unsigned int CodeOf(Sequence* seq) {
    unsigned int code = 1;

    //printf("CodeOf is run\n");

    for (int i = 0; i < seq->used; i += 1) {
        code = code * (unsigned int) powf(NthPrime(i), seq->entries[i] + 1);

        //printf("the %i th prime is %u. size of seq.used is %i\n", i, NthPrime(i), seq->used);

    }
    return code;
}

void PrintSeq(Sequence* seq) {
    printf("[%u", seq->entries[0]);
    for (int i = 1; i < seq->used; i+=1) {
        printf(", %u", seq->entries[i]);
    }
    printf("]\n");
}

//Reconsidering removing limited register machine struct as the machine is more of a process but idk.
//anywho, URMRun takes in a sequence of instructions and recursively iterates through them until it halts, then returns whatevers at index 0 of the register.
//There are 4 basic functions:
//      1. zero instruction: Z(n) tells the machine to change R_n (register n, ie index n of the registers) to a 0
//      2. successor instruction: S(n) tells the machine to increment R_n by 1
//      3. transfer instruction: T(m, n) tells the machine to change R_n to the entry at R_m
//      4. jump instruction: J(m, n, q) tells the machine to move to R_q if R_m = R_n, otherwise to increment the current instruction by 1
//Given these numberings, when making a sequence of these instructions, Z(n) -> <1, n>, T(m, n) -> <3, m, n>, etc.

typedef struct UnlimitedRegisterMachine {
    Sequence* R; //registers. array list of registers. think of it as the ram
    Sequence* I; //Instructions. sequence form of instruction list. think of it as the c code run on the cpu on a single thread
    unsigned int Ci; //current instruction
} URM;

//What is regbound? In order to model a URM without unlimited memory, we must make a few concessions. 
//In this case, lest a register be specified, it will be assumed to be 0. 
//If we want to operate on such a register but it is out of our sequence's bounds, we can add it and and all fill in all registers before it
//I am considering a dictionary-structure to avoid adding these extra registers that are all zeroes, but assuming programs try and use registers efficiently, this structure should suffice

void URMRegBound(URM machine, unsigned int n) {
    if (n >= machine.R->used){
        for (int i = machine.R->used; machine.R->used < n; i += 1) {
            SeqAdd(machine.R, 0);
        }
    }
}

//In order to iterate through in an uncomplicated fashion, we will append chunks of 4 numbers for each instruction (as the larges instruction, jumps, take in 3 inputs),
//In reality, the instruction sequence would contain instruction "chunks" of differing length. However, as that is not the focus, it is on hold.
//machine is of type lrm

void Zeroer(URM machine, unsigned int (n)[3]) {
    URMRegBound(machine, n[0]);
    machine.R->entries[n[0]] = 0;
    machine.Ci += 1;
}

void Successor(URM machine, unsigned int (n)[3]) {
    URMRegBound(machine, n[0]);
    machine.R->entries[n[0]] += 1;
    machine.Ci += 1;
}

void Transfer(URM machine, unsigned int (n)[3]) {
    URMRegBound(machine, n[0]);
    URMRegBound(machine, n[1]);
    machine.R->entries[n[0]] = machine.R->entries[n[1]];
    machine.Ci += 1;
}

void Jumper(URM machine, unsigned int (n)[3]) {
    URMRegBound(machine, n[0]);
    URMRegBound(machine, n[1]);
    if (machine.R->entries[n[1]] == machine.R->entries[n[0]]) {
        machine.Ci = n[2];
    }
    else {machine.Ci += 1;}
}

void (*funk[4])(URM, unsigned int[3]) = {Zeroer, Successor, Transfer, Jumper};

unsigned int pholder[4];

//These macros automatically add instructions to the instruction sequence!
//I will leave a a section down in the main function such that you may try programming a URM!
//Note that "machine" is of time URM, and pholder will only store the last instruction added (it is a place-holder, thus pholder)
#define Z(n, machine)\
    pholder = {1, n, 0, 0};\
    SequenceArrayAdd(machine.I, pholder, 4);\

#define S(n, machine)\
    pholder = {2, n, 0, 0};\
    SequenceArrayAdd(machine.I, pholder, 4);\

#define T(m, n, machine)\
    pholder = {3, m, n, 0};\
    SequenceArrayAdd(machine.I, pholder, 4);\

#define J(m, n, q, machine)\
    pholder = {4, m, n, q};\
    SequenceArrayAdd(machine.I, pholder, 4);\

//Now, how to run this URM? Recursively! Because the behavior of a given instruction on a specific sequence of registers is determined, 
//we can think of a specific register "input", the same instructions, and a "current instruction" (as denoted by ci) as all the information we need to determine a specific state of the URM
//A urm whose ci is beyond the index of the last instruction "halts" (as there is nothing left to do), and so the program returns the number in register 0
unsigned int ciIndex;
unsigned int inputs[3];

unsigned int URMRun(URM machine) {
    if (machine.Ci > machine.I->used) {
        return machine.R->entries[0];
    }
    else{
        ciIndex = machine.Ci * 4;
        inputs[0] = machine.I->entries[ciIndex + 1];
        inputs[1] = machine.I->entries[ciIndex + 2];
        inputs[2] = machine.I->entries[ciIndex + 3];
    
        funk[machine.I->entries[ciIndex] - 1](machine, inputs);
        PrintSeq(machine.R);
        return URMRun(machine);
    }
}

void main() {
    SeqMake(TestRegi);
    SeqMake(TestInst);
    unsigned int R[] = {
        0, 0
    };
    unsigned int I[] = {
        2, 0, 0, 0,
        3, 0, 1, 0,
        4, 0, 1, 5
    };
    SeqArrayAdd(&TestRegi, R, 2);
    SeqArrayAdd(&TestInst, I, 3 * 4);
    PrintSeq(&TestInst);
    PrintSeq(&TestRegi);
    URM TestUrm = {&TestRegi, &TestInst, 0};
    URMRun(TestUrm);
}