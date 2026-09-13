#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Hello! My goal is to implement a limited register machine. As of now it just takes in sequences but once i find a more efficient way of encoding sequences it will take in 2 integers!
//For further reading, look out for the computability theory textbook by Tyler Arant at UCLA. 
//I have an early copy for a class but am not supposed to share it because its in-the-works
//however it is really helpful for understanding the concepts (as is MATH 136 at berkeley but I assume thats not accessible everyone)

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))
#define LISTARRAY(type, name) \
    typedef struct name##List { \
        type* entries; \
        int used; \
        int size; \
    } name; \
    void name##Init(name* L) { \
        L->size = 1; \
        L->used = 0; \
        L->entries = malloc(sizeof(type)); \
    } \
    void name##Add(name* L, type a) { \
        if (L->used == L->size) { \
            L->size *= 2; \
            L->entries = realloc(L->entries, sizeof(type) * L->size); \
        } \
        L->entries[L->used] = a; \
        L->used += 1; \
    } \
    void name##ArrayAdd(name* L, type a[], unsigned int size) {\
        for (int i = 0; i < size; i += 1) {\
            name##Add(L, a[i]);\
        }\
    }\

#define LA(name, LAname)\
    LAname name;\
    LAname##Init(&name);\

LISTARRAY(unsigned int, Sequence);

//Implementing helper functions to build up to a universal function, F(m, n): N^2 -> N, m, n in N using codes. For now it is mostly an exercise as the 32 bit integer limit is reached quickly via encoding sequences being a lot of high prime powers.

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
// 1. zero instruction: Z(n) tells the machine to change R_n (register n, ie index n of the registers) to a 0
// 2. successor instruction: S(n) tells the machine to increment R_n by 1
// 3. transfer instruction: T(m, n) tells the machine to change R_n to the entry at R_m
// 4. jump instruction: J(m, n, q) tells the machine to move to R_q if R_m = R_n, otherwise to increment the current instruction by 1
//Given these numberings, when making a sequence of these instructions, Z(n) -> <1, n>, T(m, n) -> <3, m, n>, etc.

typedef struct LimitedRegisterMachine {
    Sequence* R; //registers. array list of registers. think of it as the ram
    Sequence* I; //Instructions. sequence form of instruction list. think of it as the c code run on the cpu on a single thread
    unsigned int Ci; //current instruction
} LRM;
//In order to iterate through these instructions which take up different chunks of the sequence, we must define a few helper functions
//machine is of type lrm
#define INavigation(machine)\
    void machine##INavToFrom(unsigned int n, unsigned int index) {\
        if\
    }\

#define Instructions(machine)\
    void machine##RegBound(unsigned int n) {\
        if (n > machine.R.used){\
            for (int i = machine.R.used; machine.R.used < n; i += 1) {\
                SequenceAdd(*machine.R, 0);\
            }\
        }\
    }\
    void machine##Z(unsigned int n) {\
        machine##RegBound(n);\
        *machine.R[n] = 0;\
        machine.Ci += 1;\
    }\
    void machine##S(unsigned int n) {\
        machine##RegBound(n);\
        *machine.R[n] += 1;\
        machine.Ci += 1;\
    }\
    void machine##T(unsigned int m, unsigned int n) {\
        machine##RegBound(n);\
        machine##RegBound(m);\
        *machine.R[n] = *machine.R[m]\
        machine.Ci += 1;\
    }\
    void machine##J(unsigned int m, unsigned int n, unsigned int q) {\
        machine##RegBound(n);\
        machine##RegBound(m);\
        if (*machine.R[n] == *machine.R[m]) {\
            machine.Ci = q;\
        }\
        else {machine.Ci += 1}\
    }\

unsigned int URMRun(LRM lrm) {
    if (lrm.Ci > lrm.I->used) {
        return lrm.R->entries[0];
    }
    
}

void main() {
    LA(A, Sequence);
    unsigned int toAdd[] = {0, 1};
    SequenceArrayAdd(&A,  toAdd, ARRAYSIZE(toAdd));

}