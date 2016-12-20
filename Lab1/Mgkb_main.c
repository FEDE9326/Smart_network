#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event.h"
#include "record.h"
#include "simtime.h"
#include <math.h>


#define ARRIVAL    1
#define DEPARTURE  2
#define service_time 0.1
#define K 5
#define QUEUE_LENGTH  300

long seme = 14123451;

Event *event_list = NULL;
Record *queue=NULL;
// in service è un vettore di record lungo k che indica i server
Record *in_service[K];
double lambda,mu;
int total_users;
int user_in_service;
double cumulative_time_user;
Time total_delay,last_event_time;
int number_of_samples;
Time current_time;
int busy_server[K];
int sum_busy_server;
int num_of_user;
int dropped_customer;

extern double negexp(double,long *);

void schedule(int type, Time time,int server)
{
   Event *ev;
   ev = new_event();
   ev->type = type;
   if(type==DEPARTURE){
        ev->param1=server;
   }
   ev->time = time;

   insert_event(&event_list,ev);
   return;
}

/*
**  Function : void get_input(char *format,void *variable)
**  Return   : None
**  Remarks  : To be used instead of scanf. 'format' is a scanf format,
**             'variable' the pointer to the variable in which the input
**             is written.
*/
void get_input(char *format,void *variable)
{
    static char linebuffer[255];
    char *pin;

    fgets(linebuffer, 255, stdin);	/*  Gets a data from stdin without  */
    pin = strrchr (linebuffer, '\n');	/*  flushing problems		    */
    if (pin!=NULL) *pin = '\0';

    sscanf(linebuffer,format,variable);	/* Read only the beginning of the   */
					/* line, letting the user to write  */
					/* all the garbage he prefer on the */
					/* input line.			    */
}

void arrival(void)
{
  Time delta;
  Record *rec;
  delta = negexp(1.0/lambda,&seme);
  schedule(ARRIVAL,current_time+delta,-1);
  rec = new_record();
  rec->arrival = current_time;
  cumulative_time_user+=total_users*(current_time-last_event_time);
  total_users++;
  num_of_user++;
  if(total_users+K<QUEUE_LENGTH)
  {

  if(total_users>=K)
  {
      sum_busy_server+=K;
  }
  else
  {
      sum_busy_server+=total_users;
  }
  int i=0;

  while(in_service[i]!=NULL)
  {
    i++;
  }


  if (i<K)
  {
     in_service[i] = rec;
     busy_server[i]++;
     schedule(DEPARTURE,current_time+service_time,i);
  }
  else
  {
    in_list(&queue,rec);

  }
  }else{
    total_users--;
    dropped_customer++;

  }

  return;
}

void departure(int param1)
{
  Record *rec;
  rec = in_service[param1];
  in_service[param1] = NULL;

  cumulative_time_user+=total_users*(current_time-last_event_time);
  total_users--;
  number_of_samples++;
  total_delay+=current_time - rec->arrival;
  release_record(rec);
  //printf("Partito da %d\n",param1);
  if (queue!=NULL)
   {
    in_service[param1] = out_list(&queue);
    busy_server[param1]++;
    schedule(DEPARTURE,current_time+service_time,param1);
    //printf("Assegnato dalla coda in server %d\n",param1);
   }
  return;
}

void print_server(){
int i;
printf("Statistics about busy servers:\n");
for(i=0;i<K;i++)
    printf("\tServer %d: %i\n",i+1,busy_server[i]);

return;
}

void results(void)
{
 cumulative_time_user+=total_users*(current_time-last_event_time);
 printf("Final time %f\n",current_time);
 printf("Number of services %d\n",number_of_samples);
 printf("Average delay   %f\n",total_delay/number_of_samples);
 printf("Average number of users %f\n",cumulative_time_user/current_time);
 printf("Average number of busy server %f\n",(float)sum_busy_server/(num_of_user-dropped_customer));
 printf("Number of losses %d\n",dropped_customer);
 printf("Probability of dropped customer (Queue length: %d) %f\n", QUEUE_LENGTH, (float) dropped_customer/num_of_user);
 print_server();
 //printf("Theoretical average number of users   %f\n",avg_delay()*lambda);
 /*
 PASTA = ? ---> media temporale uguale media con poisson
 numeo server+=k o total*(curr-last)
 mettere queue lenght estremi
 metter k =1 mm1
 */
 exit(0);
}

int main()
{

 Event *ev;
 Time maximum;
 cumulative_time_user =0.0;
 total_delay=0.0;
 last_event_time=0.0;
 number_of_samples=0.0;
 user_in_service=0;
 current_time = 0.0;
 total_users = 0;
 sum_busy_server=0;
 num_of_user=0;
 dropped_customer=0;
 printf("Insert arrival rate, lambda: ");
 get_input("%lf",&lambda);
 printf("lambda = %f\n",lambda);
// printf("Insert serive rate, mu: ");
// get_input("%lf",&mu);
// printf("mu     = %f\n",mu);
 printf("Insert simulation time: ");
 get_input("%lf",&maximum);
 printf("Max Time  = %f\n",maximum);
 int c;
 for(c=0;c<K;c++)
 {
     //in_service[c]=malloc(sizeof(Record*));
     in_service[c]=NULL;
 }

 schedule(ARRIVAL,negexp(1.0/lambda,&seme),-1);

 while (current_time<maximum)
  {
    ev = get_event(&event_list);
    last_event_time = current_time;
    current_time = ev->time;
    switch (ev->type)
     {
       case ARRIVAL:   arrival();
		      break;
       case DEPARTURE: departure(ev->param1);
		      break;
       default:       printf("This is awful!!!\n");
		      exit(1);
     }
    release_event(ev);
  }
 results();

 return 0;
}

