#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event.h"
#include "record.h"
#include "simtime.h"

#define ARRIVAL    1
#define DEPARTURE  2
#define service_time 0.1

long seme = 14123451;

Event *event_list = NULL;
Record *queue=NULL;
Record *in_service=NULL;
double lambda,mu;
int total_users;
double cumulative_time_user;
Time total_delay,last_event_time;
int number_of_samples;
Time current_time;

extern double negexp(double,long *);

void schedule(int type, Time time)
{
   Event *ev;
   ev = new_event();
   ev->type = type;
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
{ Time delta;
  Record *rec;
  delta = negexp(1.0/lambda,&seme);
  schedule(ARRIVAL,current_time+delta);
  rec = new_record();
  rec->arrival = current_time;
  cumulative_time_user+=total_users*(current_time-last_event_time);
  total_users++;
  if (in_service == NULL)
    {
      in_service = rec;
      schedule(DEPARTURE,current_time+service_time);
    }
  else
    in_list(&queue,rec);
  return;
}

void departure(void)
{
  Record *rec;
  rec = in_service;
  in_service = NULL;
  cumulative_time_user+=total_users*(current_time-last_event_time);
  total_users--;
  number_of_samples++;
  total_delay+=current_time - rec->arrival;
  release_record(rec);
  if (queue!=NULL)
   {
    in_service = out_list(&queue);
    schedule(DEPARTURE,current_time+service_time);
   }
  return;
}
double avg_delay()
{
    double ro=lambda*service_time;
    return service_time+service_time*ro/(2-2*ro);
}


void results(void)
{
 cumulative_time_user+=total_users*(current_time-last_event_time);
 printf("Final time %f\n",current_time);
 printf("Number of services %d\n",number_of_samples);
 printf("Average delay   %f\n",total_delay/number_of_samples);
 printf("Theoretical average delay %f\n",avg_delay());
 printf("Average number of users %f\n",cumulative_time_user/current_time);
 printf("Theoretical average number of users   %f\n",avg_delay()*lambda);
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

 current_time = 0.0;
 total_users = 0;
 printf("Insert arrival rate, lambda: ");
 get_input("%lf",&lambda);
 printf("lambda = %f\n",lambda);
 printf("Insert serive rate, mu: ");
 get_input("%lf",&mu);
 printf("mu     = %f\n",mu);
 printf("Insert simulation time: ");
 get_input("%lf",&maximum);
 printf("Max Time  = %f\n",maximum);

 schedule(ARRIVAL,negexp(1.0/lambda,&seme));

 while (current_time<maximum)
  {
    ev = get_event(&event_list);
    last_event_time = current_time;
    current_time = ev->time;
    switch (ev->type)
     {
       case ARRIVAL:   arrival();
		      break;
       case DEPARTURE: departure();
		      break;
       default:       printf("This is awful!!!\n");
		      exit(1);
     }
    release_event(ev);
  }
 results();
}
