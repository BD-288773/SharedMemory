//
// Created by szymsid on 09.12.18.
//

#ifndef LABT3_SEM_H
#define LABT3_SEM_H

#include <semaphore.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

using namespace std;

class Semaphore{
    sem_t *semaphore;
    unsigned int initValue;

public:
    Semaphore(unsigned int iVal, const char* name){

        initValue = iVal;
        int segment_id = shm_open(name, O_CREAT | O_RDWR, 0666);
        ftruncate(segment_id, sizeof(sem_t));
        semaphore = (sem_t*) mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, segment_id, 0);
        if(sem_init(semaphore,1,initValue) == -1){
            cout << "Unable to create semaphore. Aborting." << endl;
            exit(-1);
        }

        cout << "Created " << name << "with init value " << iVal << endl;

    }
    void up(){
        sem_post(semaphore);
    }

    void down(){
        sem_wait(semaphore);
    }
    void clean(){
        sem_destroy(semaphore);
    }
};

#endif //LABT3_SEM_H
