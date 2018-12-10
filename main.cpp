//
// Created by szymsid on 08.12.18.
//

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include "queue_fifo.h"
#include "sem.h"

using namespace std;

Queue_FIFO *queue;
Semaphore mutex(1,"Critical section");
Semaphore evenProducer(1, "Even producer");
Semaphore oddProducer(0, "Odd producer");
Semaphore evenConsumer(0, "Even consumer");
Semaphore oddConsumer(0, "Odd consumer");
int *countOfEvenProducers;
int *countOfOddProducers;
int *countOfEvenConsumers;
int *countOfOddConsumers;

void * produceEvenNumber(void * args);
void * produceOddNumber(void * args);
void * consumeEvenNumber(void * args);
void * consumeOddNumber(void * args);

int main(){

    int segment_id;
    int* shared_memory;
    const size_t shared_segment_size = sizeof(Queue_FIFO) + 4*sizeof(int);


    pthread_t evenProducerThread;
    pthread_t oddProducerThread;
    pthread_t evenConsumerThread;
    pthread_t oddConsumerThread;

    segment_id = shm_open("buffer&globals", O_CREAT | O_RDWR, 0666);
    ftruncate(segment_id, shared_segment_size);
    shared_memory = (int*) mmap(nullptr,shared_segment_size, PROT_READ | PROT_WRITE, MAP_SHARED, segment_id, 0);

    queue = (Queue_FIFO*) shared_memory +4 ;
    countOfEvenConsumers = shared_memory;
    countOfOddConsumers = shared_memory + 1;
    countOfEvenProducers = shared_memory + 2;
    countOfOddProducers = shared_memory + 3;

    *countOfEvenConsumers = 0;
    *countOfEvenProducers = 0;
    *countOfOddConsumers = 0;
    *countOfOddProducers = 0;


    srand(time(nullptr));

    queue->setDefault();

    fork();
    fork();
    fork();

    pthread_create(&evenProducerThread, nullptr, produceEvenNumber, (void *) queue);
    pthread_create(&oddProducerThread, nullptr, produceOddNumber, (void *) queue);
    pthread_create(&evenConsumerThread, nullptr, consumeEvenNumber, (void *) queue);
    pthread_create(&oddConsumerThread, nullptr, consumeOddNumber, (void *) queue);

    pthread_t evenProducerThread2;
    pthread_t oddProducerThread2;
    pthread_t evenConsumerThread2;
    pthread_t oddConsumerThread2;

    pthread_t oddProducerThread3;
    pthread_t evenConsumerThread3;
    pthread_t oddConsumerThread3;

    pthread_t oddProducerThread4;
    pthread_t evenConsumerThread4;

    pthread_t evenConsumerThread5;

    pthread_create(&evenProducerThread2, nullptr, produceEvenNumber, (void *) queue);
    pthread_create(&oddProducerThread2, nullptr, produceOddNumber, (void *) queue);
    pthread_create(&evenConsumerThread2, nullptr, consumeEvenNumber, (void *) queue);
    pthread_create(&oddConsumerThread2, nullptr, consumeOddNumber, (void *) queue);

    pthread_create(&oddProducerThread3, nullptr, produceOddNumber, (void *) queue);
    pthread_create(&evenConsumerThread3, nullptr, consumeEvenNumber, (void *) queue);
    pthread_create(&oddConsumerThread3, nullptr, consumeOddNumber, (void *) queue);

    pthread_create(&oddProducerThread4, nullptr, produceOddNumber, (void *) queue);
    pthread_create(&evenConsumerThread4, nullptr, consumeEvenNumber, (void *) queue);

    pthread_create(&evenConsumerThread5, nullptr, consumeEvenNumber, (void *) queue);



    pthread_join(evenProducerThread, nullptr);
    pthread_join(oddProducerThread, nullptr);
    pthread_join(evenConsumerThread, nullptr);
    pthread_join(oddConsumerThread, nullptr);

    pthread_join(evenProducerThread2, nullptr);
    pthread_join(oddProducerThread2, nullptr);
    pthread_join(evenConsumerThread2, nullptr);
    pthread_join(oddConsumerThread2, nullptr);

    pthread_join(oddProducerThread3, nullptr);
    pthread_join(evenConsumerThread3, nullptr);
    pthread_join(oddConsumerThread3, nullptr);

    pthread_join(oddProducerThread4, nullptr);
    pthread_join(evenConsumerThread4, nullptr);

    pthread_join(evenConsumerThread5, nullptr);


    mutex.clean();
    oddConsumer.clean();
    oddProducer.clean();
    evenConsumer.clean();
    evenProducer.clean();
    munmap(nullptr, sizeof(Queue_FIFO));
    return 0;
}

bool canProduceEven(){
    if(queue->getEvenCount() < 10)
        return true;
    return false;
}

bool canProduceOdd(){
    if(queue->getEvenCount() > queue->getOddCount())
        return true;
    return false;
}

bool canConsumeEven(){
    if(queue->getCount() >= 3 && queue->isFirstEven())
        return true;
    return false;
}

bool canConsumeOdd(){
    if(queue->getCount() >= 7 && !queue->isFirstEven())
        return true;
    return false;
}

void * produceEvenNumber(void * args){

    int even;

    while(true){

        even = rand()%100;
        if(even%2 == 1) even = (even+1)%100;

        mutex.down();
        if(!canProduceEven()) {
            ++(*countOfEvenProducers);
            mutex.up();
            evenProducer.down();
            --(*countOfEvenProducers);
        }
        queue->put(even);
        cout << "Process A1 in action." << endl << even << " has been put." << endl;
        queue->showQueue();
        cout << "Even: " << queue->getEvenCount() << ", odd: " << queue->getOddCount() << endl;
        if(canProduceOdd() && *countOfOddProducers > 0) {
            oddProducer.up();
        }
        else if(canConsumeEven() && *countOfEvenConsumers > 0 ){
            evenConsumer.up();
        }
        else if(canConsumeOdd() >= 7 && *countOfOddConsumers > 0){
            oddConsumer.up();

        }
        else
            mutex.up();

        sleep(1);

    }
}

void * produceOddNumber(void * args){

    int odd;

    while(true){

        odd = rand()%100;
        if(odd%2 == 0) odd = (odd+1)%100;
        mutex.down();
        if(!canProduceOdd()){
            ++(*countOfOddProducers);
            mutex.up();
            oddProducer.down();
            --(*countOfOddProducers);
        }
        queue->put(odd);
        cout << "Process A2 in action." << endl << odd << " has been put." << endl;
        queue->showQueue();
        cout << "Even: " << queue->getEvenCount() << ", odd: " << queue->getOddCount() << endl;
        if(canProduceEven() && *countOfEvenProducers > 0)
            oddProducer.up();
        else if(canConsumeEven()&& *countOfEvenConsumers > 0)
            evenConsumer.up();
        else if(canConsumeOdd() && *countOfOddConsumers > 0)
            oddConsumer.up();
        else
            mutex.up();

        sleep(1);


    }

}

void * consumeEvenNumber(void * args){

    while(true){

        mutex.down();
        if(!canConsumeEven()){
            ++(*countOfEvenConsumers);
            mutex.up();
            evenConsumer.down();
            --(*countOfEvenConsumers);
        }

        cout << "Process B1 in action." << endl << queue->get() << " taken from queue" << endl;
        queue->showQueue();
        cout << "Even: " << queue->getEvenCount() << ", odd: " << queue->getOddCount() << endl;
        if(canProduceEven() && *countOfEvenProducers > 0)
            evenProducer.up();
        else if(canProduceOdd() && *countOfOddProducers > 0)
            oddProducer.up();
        else if(canConsumeOdd() && *countOfOddConsumers > 0)
            oddConsumer.up();
        else
            mutex.up();

        sleep(1);

    }

}

void * consumeOddNumber(void * args){

    while(true){

        mutex.down();
        if(!canConsumeOdd()){
            ++(*countOfOddConsumers);
            mutex.up();
            oddConsumer.down();
            --(*countOfOddConsumers);
        }
        queue->get();
        cout << "Process B2 in action." << endl << queue->get() << " taken from queue" << endl;
        queue->showQueue();
        cout << "Even: " << queue->getEvenCount() << ", odd: " << queue->getOddCount() << endl;
        if(canProduceEven() && *countOfEvenProducers > 0)
            evenProducer.up();
        else if(canProduceOdd() && *countOfOddProducers > 0)
            oddProducer.up();
        else if(canConsumeEven() && countOfOddConsumers > 0)
            evenConsumer.up();
        else
            mutex.up();

        sleep(1);

    }

}
