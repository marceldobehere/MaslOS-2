#include "heap/heap.h"


#ifdef __cplusplus
template<typename T> void swap(T &a, T &b) {
    T tmp{a}; a = b;
    b = tmp;
}

template<typename X>
void Merge(X A[], X TmpArry[],
        int Lpos, int Rpos, int RightEnd){
    int i, LeftEnd, NumElements,Tmpos;
    LeftEnd = Rpos - 1;
    NumElements = RightEnd - Lpos + 1;

    while (Lpos <= LeftEnd && Rpos <= RightEnd)
    {
        if(A[Lpos] <= A[Rpos]){
            TmpArry[Tmpos++] = A[Lpos++];
        }else
        {
            TmpArry[Tmpos++] = A[Rpos++];
        }
        
    }
    while (Lpos <= LeftEnd)
    {
        TmpArry[Tmpos++] = A[Lpos++];
    }
    while(Rpos <= RightEnd){
        TmpArry[Tmpos++] = A[Rpos++];
    }
    for(i = 0;i < NumElements;i++,RightEnd--){
        A[RightEnd] = TmpArry[RightEnd];
    }
    
}


template<typename ElementType>
void MSort(ElementType A[], ElementType TmpArry[],
            int Left,int Right){
    int Center;

    if(Left < Right){
        Center = (Left + Right) / 2;
        MSort(A, TmpArry, Left, Center);
        MSort(A, TmpArry, Center + 1, Right);
        Merge(A,TmpArry,Left,Center + 1,Right);
    }
}

template<typename ElementType>
void MergeSort(ElementType A[],int N){
    ElementType *TmpArry;

    TmpArry = _Malloc(N * sizeof(ElementType));
    if(TmpArry != NULL){
        MSort(A,TmpArry,0,N-1);
        _Free(TmpArry);
    }
    else{
        return;
    }
}

template <class T>
class Stack
{
public:
    struct Node;
    typedef struct Node *PtrToNode;
    typedef PtrToNode Stack_t;
    struct Node{
        T           Element;
        PtrToNode   Next;
    };
    int IsEmpty(Stack_t S){
        return S->Next = NULL;
    }
    Stack_t CreateStack(void){
        Stack_t S;
        S = _Malloc1(sizeof(struct Node));
        if(S == NULL){
            return (Stack_t)-1;
        }
        S->Next = NULL;
        MakeEmpty(S);
        return S;
    }

    void Pop(Stack_t S){
        PtrToNode FirstCell;
        if(IsEmpty(S)){
            return;
        }else{
            FirstCell = S->Next;
            S->Next = S->Next->Next;
            _Free(FirstCell);
        }
    }

    void Push(T X,Stack_t S){
        PtrToNode TmpCell;

        TmpCell = _Malloc1(sizeof(struct Node));
        if(TmpCell == NULL){
            return;
        }else{
            TmpCell->Element = X;
            TmpCell->Next = S->Next;
            S->Next = TmpCell;
        }
    }

    T Top(Stack_t S){
        if(!IsEmpty(S))
            return S->Next->Element;
        return 0;
    }

    void MakeEmpty(Stack_t S){
        if(S == NULL){
            return;
        }else{
            while (!IsEmpty(S))
            {
                Pop(S);
            }
            
        }
    }
};
#else
    #define swap(a,b,t)   t tmp = a;a = b; b = tmp;    
#endif

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
