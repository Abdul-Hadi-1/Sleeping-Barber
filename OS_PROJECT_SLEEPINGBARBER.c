#include<linux/delay.h>
#include<linux/kthread.h>
#include<linux/sched.h>
#include<linux/time.h>
#include<linux/timer.h>
#include<linux/semaphore.h>
#include<linux/unistd.h>
#include<linux/syscalls.h>
#define max_cust 25

static struct semaphore waiting;
static struct semaphore barber_Chair;
static struct semaphore barber_sleeping;
static struct semaphore barber_seatbelt;
int customer(void *number);
int barber(void *junk);
char our_thread1[8] = "bthread";
char our_thread2[9] = "custhread";
int Done = 0 ;
int customers=5;
asmlinkage long sys_SleepingBarber(void){
struct task_struct *btid;
struct task_struct *tid[15];
int chairs=3;
int customers=5;
int Num[max_cust];
int count=0;
printk("Number of Chairs are %d and Number of customers are %d\n",chairs,customers);
if(customers > max_cust){
printk("Max allowed Customers are %d\n",max_cust);  return 0; }
printk("Sleeping Barber Problem Implementation Using Semaphores:\n");
Done=0;
for (count = 0; count < max_cust; count++) {
Num[count] = count;}
sema_init(&waiting, chairs);
sema_init(&barber_Chair, 1);
sema_init(&barber_sleeping, 1);
sema_init(&barber_seatbelt, 1);
btid = kthread_create(barber,NULL,our_thread1);
if(btid)
wake_up_process(btid);
else
kthread_stop(btid);
for (count = 0; count < customers; count++) {
tid[count] = kthread_create(customer, (void *)&Num[count], our_thread2);

if(tid[count])
wake_up_process(tid[count]);
else
kthread_stop(tid[count]);
}

msleep(10);
Done++;
up(&barber_sleeping);


return 0;
}
int customer(void *number) {
int num = *(int *)number;
num++;
printk("Customer %d left to get a haircut.\n", num);
msleep(5);
printk("Customer %d reached the barber shop.\n", num); 
down(&waiting);
printk("Customer %d entered the waiting room.\n", num); 
down(&barber_Chair);
up(&waiting);
printk("Customer %d asked the sleeping barber for a haircut.\n", num);
up(&barber_sleeping);
down(&barber_seatbelt);
up(&barber_Chair);
printk("Customer %d left the barber shop.\n", num);
Done++;
return 0;
}
int barber(void *junk)  {
while(Done<=customers) {
printk("The barber is sleeping\n");
down(&barber_sleeping);
if(Done<=customers) {
printk("The barber is busy servicing the customer\n");
msleep(4);
printk("The barber has finished servicing the customer.\n");
up(&barber_seatbelt);}
else{
printk("The Barber's Job Is All Done For The Day!'\n");}}
return 0;
}
