//
// Created by szymsid on 09.12.18.
//

#ifndef LABT3_QUEUE_FIFO_H
#define LABT3_QUEUE_FIFO_H

#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
using namespace std;
#define N 20

class Queue_FIFO{
    int buff[N];
    int firstIndex;
    int lastIndex;
    int evenCount;
    int oddCount;

public:
    Queue_FIFO(){
        firstIndex = 0;
        lastIndex = 0;
        evenCount = 0;
        oddCount = 0;

        for(int i = 0; i < 20 ; i++)
            buff[i] = 0;
    }

    void setDefault(){
        firstIndex = 0;
        lastIndex = 0;
        evenCount = 0;
        oddCount = 0;

        for(int i = 0; i < 20 ; i++)
            buff[i] = 0;
    }

    void put(int a){
        if(evenCount + oddCount != N){
            if(evenCount + oddCount == 0)
                buff[lastIndex] = a;
            else
                buff[(lastIndex =(++lastIndex)%N)] = a;
            if(a%2 == 0) evenCount++;
            else oddCount++;

        }
        else cout << "Buffer overflow.\n";
    }

    int get(){
        if(evenCount + oddCount == 0) return -1;
        int a = buff[firstIndex];
        firstIndex = (++firstIndex)%N;
        if(a%2 == 0) evenCount--;
        else oddCount--;
        return a;
    }

    int getEvenCount(){
        return evenCount;
    }

    int getOddCount(){
        return oddCount;
    }

    int getCount(){
        return  evenCount + oddCount;
    }

    bool isFirstEven(){
        if(evenCount + oddCount == 0) return -1;
        if(buff[firstIndex]%2 == 0)
            return 1;
        return 0;
    }

    void showQueue(){
        cout << "Current queue: [ ";
        int i = firstIndex;
        if(i == lastIndex){
            cout << "]" << endl;
            return;
        }
        while(i != lastIndex){
            cout << buff[i] << " ";
            i = (i + 1)%N;
        }

        cout << "]" << endl;
    }
};

#endif //LABT3_QUEUE_FIFO_H
