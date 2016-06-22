#include <stdio.h>

#include "ImageReconstruct.h"


// Constructs and initiates a queue for a given maximum number of 
// elements - initSize.
XYQueueC::XYQueueC(int initSize)
:size(initSize)
//------------------------------
{
    if(size!=0)
        xy = new (XY[size]);
    else
        xy = NULL;
    endIdx=begIdx=actualSize=0;
}

XYQueueC::~XYQueueC()
//-------------------
{
    if(xy!=NULL)
        delete[] xy;
}


// Append a new XY structure at the end of the queue.
// Throws an error if the queue is full.
int XYQueueC::Put(int x,int y)
//----------------------------
{
    if(actualSize<size) {
        xy[endIdx].x=(short)x;
        xy[endIdx].y=(short)y;
        endIdx=(endIdx+1)%size;
        actualSize++;
        return 1;
    }
    else {
        //Throw("XYQueue::Put(int,int):write to full queue");
        return 0;
    }
}


// Retrieves and removes the first item from the queue.
// Throws an error if the queue is empty.
XY XYQueueC::Get()
//----------------
{
    int retIdx = begIdx;
    if(actualSize>0) {
        begIdx=(begIdx+1)%size;
        actualSize--;
    }
    else {
     //  Throw("XYQueue::Get():read from empty queue");
    }
    return xy[retIdx];
}


// Retrieves the i-th XY structure from the queue, but
// does not remove it.
XY XYQueueC::Peek(int i)
//----------------
{
    if(i>=actualSize) {
   //    Throw("XYQueue::Peek(int):index out of range");
    }
    return xy[(begIdx+i)%size];
}


//========================================================================
//Here begins the implementation of the priority queue class ValXYQueueC
//========================================================================

//Inits the priority queue for initRange queue of size given in array initSize
ValXYQueueC::ValXYQueueC(int initRange,int * initSize)
:range(initRange)
//-------------------------------------------------------
{
    firstNonEmpty=range;
    xyQueue = new (XYQueueC*[range]);
    for(unsigned int i=0;i<range;i++) {
        xyQueue[i]=new XYQueueC(initSize[i]);
    }
}

ValXYQueueC::~ValXYQueueC()
//-------------------------
{
    for(unsigned int i=0;i<range;i++) {
        delete xyQueue[i];
    }
    delete[] xyQueue;
}


// Appends a new item {x,y} to the queue for value val
int ValXYQueueC::Put(Pixel32 val,int x,int y)
//-------------------------------------------
{
    if(val>=range) {
//        Throw("ValXYQueueC::Put(Pixel32,int,int):invalid index");
        return 0;
    }
    if(xyQueue[val]->Put(x,y)) {
        if(val<firstNonEmpty) firstNonEmpty=val;
        return 1;
    }
    else {
    //    Throw("ValXYQueueC::Put(Pixel32,int,int):write to full queue");
        return 0;
    }
}


// Retrieves the first item in the priority queue - items are retrieved in
// ascending order
XY ValXYQueueC::Get()
//-------------------
{
    XY rxy={0,0};
    if(!Empty()) {
        rxy = xyQueue[firstNonEmpty]->Get();
        if(xyQueue[firstNonEmpty]->Empty()) {
            for(++firstNonEmpty;firstNonEmpty<range &&
                    xyQueue[firstNonEmpty]->Empty();firstNonEmpty++);
        }
    }
    else {
    //    Throw("ValXYQueueC::Get():read from empty queue");
    }
    return rxy;
}
