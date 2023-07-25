#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#define maxsmokers 3
char* agent_supplies[3] = { "matches & paper","matches & tobacco","tobacco & paper"};
sem_t agent_semaphore;
bool items_supplied[3] = { false, false, false };
sem_t smoker_semaphors[3];
sem_t smoker_items[3];
sem_t smoker_lock;
void* smoker(void* param)
{
int smoker_id = *(int*) param;
int item_id= smoker_id % 3;

for (int i=0;i<maxsmokers;i++)
{
if(item_id==0)
{
printf("\033[1;35mSmoker is waiting returns \033[1;35m%ld\n",syscall(326,smoker_id,1));
}
else if(item_id==1)
{
printf("\033[1;35mSmoker is waiting returns \033[1;35m%ld\n",syscall(326,smoker_id,2));
}
else if(item_id==2)
{
printf("\033[1;35mSmoker is waiting returns \033[1;35m%ld\n",syscall(326,smoker_id,3));
}
sem_wait(&smoker_semaphors[item_id]);

printf("\033[0;32mSmoker is making cigarette returns \033[1;35m%ld\n",syscall(326,smoker_id,4));
sleep(4);

sem_post(&agent_semaphore);
printf("\033[1;36mSmoker is smoking returns \033[1;36m%ld\n",syscall(326,smoker_id,5));
sleep(5);
}
    return NULL;
}
void *chose_smoker(void* param)
{
int smoking_id= *(int*) param;

for (int i=0;i<6;i++)
{

sem_wait(&smoker_items[smoking_id]);
sem_wait(&smoker_lock);

if (items_supplied[(smoking_id + 1) % 3])
{
items_supplied[(smoking_id+ 1) % 3] = false;
sem_post(&smoker_semaphors[(smoking_id + 2) % 3]);
}
else if (items_supplied[(smoking_id + 2) % 3])
{
items_supplied[(smoking_id + 2) % 3] = false;
sem_post(&smoker_semaphors[(smoking_id + 1) % 3]);
}
else
{
items_supplied[smoking_id] = true;
} 
        sem_post(&smoker_lock);
}
        return NULL;
}
void* agent(void* param)
{
int agent_id = *(int*) param;
    int id=(agent_id+1)%3;
for (int i=0;i<maxsmokers;i++)
{
sleep(3);
sem_wait(&agent_semaphore);
sem_post(&smoker_items[agent_id]);
sem_post(&smoker_items[id]);
        if(id==0)
        {
        printf("\033[1;35mAgent is supplying returns \033[1;34m%ld\n",syscall(326,agent_id,6));
}
else if(id==1)
        {
        printf("\033[1;35mAgent is supplying returns \033[1;34m%ld\n",syscall(326,agent_id,7));
}
       else if(id==2)
        {
        printf("\033[1;35mAgent is supplying returns \033[1;34m%ld\n",syscall(326,agent_id,8));
}
}
return NULL;
}
int main(int argc, char* argv[])
{
sem_init(&agent_semaphore,0, 1);
sem_init(&smoker_lock, 0, 1);
int smoker_ids[maxsmokers];
pthread_t smoker_threads[3];
int agent_ids[maxsmokers];
for (int i=0;i<3;++i)
{
pthread_create(&smoker_threads[i], NULL, smoker, &smoker_ids[i]);
}

for (int i=0;i<3;i++)
{
smoker_ids[i] = i;
        if (pthread_create(&smoker_threads[i], NULL, smoker, &smoker_ids[i]) == EAGAIN)
{
perror("Unable to create threads");
return 0;
}
smoker_ids[i] = i;
if (pthread_create(&smoker_threads[i], NULL, chose_smoker, &smoker_ids[i]) == EAGAIN)
{
perror("Unable to create threads");
return 0;
}
agent_ids[i] = i;
if (pthread_create(&smoker_threads[i], NULL, agent, &agent_ids[i]) == EAGAIN)
{
perror("Unable to create threads");
return 0;
}
}
for (int i=0;i<3;++i)
{
pthread_join(smoker_threads[i], NULL);
}
return 0;
}



