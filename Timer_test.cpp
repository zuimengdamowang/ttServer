#include "src/Timer.h"


//客户端执行函数
void client_fun(struct client_data *user_data);

void sig_func(int signo); //信号处理函数

sort_timer_list *myList=new sort_timer_list();

int main(int argc,char* argv[])
{
    //创建两个客户端
    struct client_data client1,client2;
    bzero(&client1,sizeof(client1));
    bzero(&client1,sizeof(client1));

    time_t current_time = time(NULL);

    //创建定时器1
    util_timer *timer1 = new util_timer();
    timer1->expire = current_time + 3;
    timer1->cb_func = client_fun;
    timer1->user_data = &client1;
    timer1->user_data->timer = timer1;
    bcopy("I am client1", timer1->user_data->recv_buf, 13);

    //创建定时器2
    util_timer *timer2 = new util_timer();
    timer2->expire = current_time + 4;
    timer2->cb_func = client_fun;
    timer2->user_data = &client2;
    timer2->user_data->timer = timer2;
    bcopy("I am client2", timer2->user_data->recv_buf, 13);

    //将两个定时器加入链表中
    myList->add_timer(timer1);
    myList->add_timer(timer2);

    //为SIGALRM信号绑定处理函数
    signal(SIGALRM, sig_func);

    //设置一个struct itimerval定时器
    struct itimerval tick;
    tick.it_value.tv_sec = 2;  //struct itimerval定时器首次在20秒之后到期启动
    tick.it_value.tv_usec = 0;
    tick.it_interval.tv_sec  = 1; //struct itimerval定时器第一次到时之后，每隔10秒到期一次
    tick.it_interval.tv_usec = 0;
 
    //开启一个struct itimerval定时器，种类为ITIMER_REAL，触发的信号为SIGALRM
    int ret = setitimer(ITIMER_REAL, &tick, NULL);
 
    if ( ret != 0)
    {
        printf("setitimer error:%s \n", strerror(errno) );
        exit(EXIT_FAILURE);
    }

    //pause：进程挂起，在接收信号，并从信号处理函数中返回时才取消挂起
    while(1)
        pause();
    
    exit(EXIT_SUCCESS);
}

void client_fun(struct client_data *user_data)
{
    printf("%s\n",user_data->recv_buf);
}

void sig_func(int signo)
{
    //在SIGALRM信号处理函数中轮询链表是否有定时任务到期可以执行
    myList->tick();
}