#ifndef __QUEUE_H
#define __QUEUE_H

typedef unsigned char Pixel8;
typedef unsigned short Pixel16;
typedef signed short Pixel16S;
typedef unsigned long Pixel32;

struct XY {
    short   x;
    short   y;

//    inline XY& operator= (const XY& xy) { x=xy.x;y=xy.y };
};

class XYQueueC  {
    XY      *xy;
    int     begIdx;
    int     endIdx;
    int     size;
    int     actualSize;
        
    public:
    //-----

    XYQueueC(int);
    ~XYQueueC();

    int     Put(int,int);
    XY      Get();
    XY      Peek(int); // NEW
    int     Empty() { return (actualSize==0); }
    int     Size() { return actualSize; }
};

class  ValXYQueueC {
    XYQueueC    **xyQueue;
    unsigned int         range; // int
    unsigned int         firstNonEmpty;
        
    public:
    //-----

    ValXYQueueC(int,int *);
    ~ValXYQueueC();
    
    int         Put(Pixel32,int,int);
    XY          Get();
    int         Empty() { return (firstNonEmpty==range); }
};

#endif // __QUEUE_H

