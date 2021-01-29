#include <iostream>

#include <pthread.h>

#include <stdio.h>

#include <unistd.h>

#include <string>

#include <stdlib.h>

#include <queue>

#include <semaphore.h>

using namespace std;



#define NUM_THREADS 10

#define MEMORY_SIZE 150



struct node

{

    int id;

    int size;

};





queue<node> myqueue; // shared que

pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER; // mutex

pthread_t server; // server thread handle

sem_t semlist[NUM_THREADS]; // thread semaphores



int thread_message[NUM_THREADS]; // thread memory information

char  memory[MEMORY_SIZE]; // memory size

bool checker =true;

int new_count=0;







void my_malloc(int thread_id, int size)

{

    //This function will add the struct to the queue

    node new_node;

    new_node.id=thread_id;

    new_node.size=size;

    myqueue.push(new_node);

}



void * server_function(void *)

{





       while (checker) {



           pthread_mutex_lock(&sharedLock);





           if((myqueue.size()) > 0)

           {



               int Mem_size = MEMORY_SIZE;

               int size = myqueue.front().size;

               if(size + new_count > Mem_size )

               {





                   thread_message[myqueue.front().id] = -1; //not enough memory = -1



               }

               else if ( size + new_count <= Mem_size){



                   thread_message[myqueue.front().id] =size;



                   new_count += size;





               }

               myqueue.pop();

               sem_post(&semlist[myqueue.front().id]);



           }



          pthread_mutex_unlock(&sharedLock);





       }





    return NULL;

    //This function should grant or decline a thread depending on memory size.



}

int c= 0;

void * thread_function(void * id)

{

    //This function will create a random size, and call my_malloc

    //Block

    //Then fill the memory with id's or give an error prompt

    int * id_2 = (int*) id;

    //srand (time(NULL) + *id_2);

    int randSize = rand() % (MEMORY_SIZE/6);







    my_malloc(*id_2,randSize);



    sem_wait(&semlist[*id_2]);





    if (thread_message[ *id_2] != -1)

    {

        pthread_mutex_lock(&sharedLock);



        for (int j = 0; j < randSize; j++, c++)

                   {



                           memory[c] = char(*id_2+'0');



                   }

                    pthread_mutex_unlock(&sharedLock);



    }

    else if(thread_message[ *id_2] == -1)

        {



            printf("thread[%d]: Not enough memory\n" ,*id_2);



        }







    return NULL;

}



void init()

{

    pthread_mutex_lock(&sharedLock);

    //lock

    for(int i = 0; i < NUM_THREADS; i++) //initialize semaphores

    {

        sem_init(&semlist[i],0,0);}

    for (int i = 0; i < MEMORY_SIZE; i++)

        //initialize memory

      {char zero = 'I'; memory[i] = zero;}



       pthread_create(&server,NULL,server_function,NULL); //start server



    pthread_mutex_unlock(&sharedLock); //unlock

}







void dump_memory()

{

 // You need to print the whole memory array here.

    for (int i = 0; i < MEMORY_SIZE; i++) {

            cout << memory[i];

        }

}



int main (int argc, char *argv[])

 {



     //You need to create a thread ID array here

     pthread_t threads[NUM_THREADS];

    int thread_id[NUM_THREADS];

     for (int i = 0; i < NUM_THREADS; i++) {

         pthread_t temp;

             threads[i] = temp;

             thread_id[i] = i;

         }



     init();    // call init



     //You need to create threads with using thread ID array, using pthread_create()

     for (int j = 0; j < NUM_THREADS; j++) {

             pthread_create( &threads[j], NULL, thread_function, (void *) & thread_id[j]);

         }



     //You need to join the threads

     for (int k = 0; k < NUM_THREADS; k++) {

             pthread_join(threads[k], NULL);

         }



     dump_memory(); // this will print out the memory



     printf("\nMemory Indexes:\n" );

     for (int i = 0; i < NUM_THREADS; i++)

     {

         printf("[%d]" ,thread_message[i]); // this will print out the memory indexes

     }

     checker =false;

     printf("\nTerminating...\n");

 }
