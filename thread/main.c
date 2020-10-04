#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int buffer[8];
int head = 0;
int tail = 0;
int count = 0;
int maxProduction;
int maxConsumption;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notFull = PTHREAD_COND_INITIALIZER;
pthread_cond_t notEmpty = PTHREAD_COND_INITIALIZER;

void* produce(void* value)
{
    int producerId = *(int*) value;
    int production = producerId * maxProduction;
    int i;

    for(i = 0; i < maxProduction; i++){
        pthread_mutex_lock(&mutex);

        while(count == 8){
            pthread_cond_wait(&notFull, &mutex);
        }
        //producing an int
        //printf("head = %d, tail = %d, producer_%d\n", head,tail,producerId);

        count++;
        buffer[tail] = production;
        printf("producer_%d produced item %d\n", producerId, production);
        tail = (tail + 1) % 8;
        production++;
        pthread_cond_signal(&notEmpty);
        pthread_mutex_unlock(&mutex);
    }
    //printf("maxProduction %d: maxConsumption %d \n\n", maxProduction, maxConsumption);
    pthread_exit(NULL);
}

void* produceWithDelay(void* value)
{
    int producerId = *(int*) value;
    int production = producerId * maxProduction;
    int i;

    for(i = 0; i < maxProduction; i++){
        pthread_mutex_lock(&mutex);

        while(count == 8){
            pthread_cond_wait(&notFull, &mutex);
        }
        //producing an int
        count++;
        buffer[tail] = production;
        printf("producer_%d produced item %d\n", producerId, production);
        tail = (tail + 1) % 8;
        production++;

        pthread_cond_signal(&notEmpty);
        pthread_mutex_unlock(&mutex);
        usleep(500000);

    }
    /*
    printf("Delayed Producer %d Produced  \n", producerId);
    printf("maxProduction %d: maxConsumption %d \n\n", maxProduction, maxConsumption);
    */
    pthread_exit(NULL);
}

void* consume(void* value)
{
    int consumerId = *(int*) value;
    int consumed;
    int i;

    for(i = 0; i < maxConsumption; i++){
        pthread_mutex_lock(&mutex);
        while(count == 0){
            //printf("Consumer %d blocked  \n", consumerId);
            pthread_cond_wait(&notEmpty, &mutex);
        }

        count--;
        consumed = buffer[head];
        printf("consumer_%d consumed item %d \n", consumerId, consumed);
        head = (head + 1) % 8;
        //printBuffer();

        pthread_cond_signal(&notFull);
        pthread_mutex_unlock(&mutex);
    }
    //printf("maxProduction %d: maxConsumption %d \n\n", maxProduction, maxConsumption);
    //printf("consumer %d is DONE\n", consumerId);

    pthread_exit(NULL);
}

void* consumeWithDelay(void* value)
{
    int consumerId = *(int*) value;
    int consumed;
    int i;

    for(i = 0; i < maxConsumption; i++){
        pthread_mutex_lock(&mutex);
        while(count == 0){
            //printf("Consumer %d blocked  \n", consumerId);
            pthread_cond_wait(&notEmpty, &mutex);
        }

        count--;
        consumed = buffer[head];
        printf("consumer_%d consumed item %d \n", consumerId, consumed);

        head = (head + 1) % 8;
        //printBuffer();

        pthread_cond_signal(&notFull);
        pthread_mutex_unlock(&mutex);
        usleep(500000);
    }
    //printf("maxProduction %d: maxConsumption %d \n\n", maxProduction, maxConsumption);
    //printf("consumer %d is DONE\n", consumerId);

    pthread_exit(NULL);
}

void printBuffer()
{
    int i;
    for(i = 0; i < 8; i++)
    {
        printf("%d[%d] : ",i, buffer[i]);
    }
    printf("\n\n");
}

void fillBuffer()
{
    int i;
    for(i = 0; i < 8; i++)
    {
        buffer[i] = -1;
    }
}

int main(int argc, char **argv)
{
    int producerNum = atoi(argv[1]);        //P : change this to argv[1] later
    int consumerNum = atoi(argv[2]);        //C : change this to argv[2] later
    int delay = atoi(argv[4]);              //D :change this to argv[4] later IF == 0 then producer delay, IF == 1 then consumer delay
    maxProduction = atoi(argv[3]);         //I : change this to argv[3] later
    maxConsumption = producerNum * maxProduction / consumerNum;

    pthread_t producers[producerNum];
    pthread_t consumers[consumerNum];

    //printf("In main\n");
    //fillBuffer();
    //printBuffer();

    if(producerNum < 1 || producerNum > 16){
        return 0;
    }
    if(consumerNum < 1 || consumerNum > 16){
        return 0;
    }
    if(consumerNum >= (producerNum * maxProduction)){
        return 0;
    }


    if(delay == 0)
    {
            //printf("Running with Producer Delay  \n\n");

        int i;
        for (i = 0; i < producerNum; i++){
            pthread_create(&producers[i], NULL, produceWithDelay, &i);
            usleep(30);
        }
        for (i = 0; i < consumerNum; i++){
            pthread_create(&consumers[i], NULL, consume, &i);
            usleep(30);
        }

        for (i = 0; i < producerNum; i++){
            pthread_join(producers[i], NULL);
        }
        for (i = 0; i < consumerNum; i++){
            pthread_join(consumers[i], NULL);
        }
    }
    else{
            //printf("Running with Consumer Delay  \n\n");

        int i;
        for (i = 0; i < producerNum; i++){
            pthread_create(&producers[i], NULL, produce, &i);
        }
        for (i = 0; i < consumerNum; i++){
            pthread_create(&consumers[i], NULL, consumeWithDelay, &i);
        }

        for (i = 0; i < producerNum; i++){
            pthread_join(producers[i], NULL);
        }
        for (i = 0; i < consumerNum; i++){
            pthread_join(consumers[i], NULL);
        }
    }
    return 0;
}
