#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// materials 1,2,3
//tools x,y,z
//products A,B,C

//CONSTANT
#define INPUT_BUFFER_SIZE 10

//struct
typedef struct{
	int queue[INPUT_BUFFER_SIZE];
	int front;
	int rear;
	int count;
	int used;
	int lastsecond;
} input_buffer;



//Global
pthread_mutex_t input_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_full_wait_for_generate = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_generator_wakeup = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_tool_availble = PTHREAD_COND_INITIALIZER;
input_buffer* input_queue;
bool isPause = false;




//queue methods begin;
void insert_material(input_buffer* buf, int materials){
	if(buf->count != INPUT_BUFFER_SIZE){
		if(buf->count > 1) buf->lastsecond = buf->queue[buf->rear];
		if(buf->rear == -INPUT_BUFFER_SIZE) buf->rear = 0; 
		buf->queue[--(buf->rear)] = materials;
		buf->count++;
	}
}

int get_material(input_buffer* buf){
	if(buf->count != 0){
		if(buf->front < -INPUT_BUFFER_SIZE) buf->front = 0;
		buf->count--;
		buf->used++;
		return buf->queue[(buf->front)--];
	}
	return 0;
}

bool isfull(input_buffer* buf) {return buf->count == 10;}

//queue methods end;


//generators begin;
void* generator_first(){
	int materials = 1;
	while(true){
		pthread_mutex_lock(&input_mutex);
		while(input_queue->count > 0 && input_queue->queue[input_queue->rear] == materials || isfull(input_queue)){
			if(isfull(input_queue)) pthread_cond_wait(&cond_full_wait_for_generate,&input_mutex);
			else{
				pthread_cond_signal(&cond_full_wait_for_generate);
				pthread_cond_signal(&cond_generator_wakeup);
				pthread_cond_wait(&cond_generator_wakeup,&input_mutex)
			}
		}

		insert_material(input_queue,materials);
		pthread_cond_signal(&cond_generator_wakeup);
		pthread_cond_signal(&cond_producer);
		pthread_mutex_unlock(&input_mutex)

		while(isPause) usleep(500);
	}
}

void* generator_second(){
	int materials = 2;
	while(true){
		pthread_mutex_lock(&input_mutex);
		while(input_queue->count > 0 && input_queue->queue[input_queue->rear] == materials || isfull(input_queue)){
			if(isfull(input_queue)) pthread_cond_wait(&cond_full_wait_for_generate,&input_mutex);
			else{
				pthread_cond_signal(&cond_full_wait_for_generate);
				pthread_cond_signal(&cond_generator_wakeup);
				pthread_cond_wait(&cond_generator_wakeup,&input_mutex)
			}
		}

		insert_material(input_queue,materials);
		pthread_cond_signal(&cond_generator_wakeup);
		pthread_cond_signal(&cond_producer);
		pthread_mutex_unlock(&input_mutex)

		while(isPause) usleep(500);
	}
}

void* generator_third(){
	int materials = 3;
	while(true){
		pthread_mutex_lock(&input_mutex);
		while(input_queue->count > 0 && input_queue->queue[input_queue->rear] == materials || isfull(input_queue)){
			if(isfull(input_queue)) pthread_cond_wait(&cond_full_wait_for_generate,&input_mutex);
			else{
				pthread_cond_signal(&cond_full_wait_for_generate);
				pthread_cond_signal(&cond_generator_wakeup);
				pthread_cond_wait(&cond_generator_wakeup,&input_mutex)
			}
		}

		insert_material(input_queue,materials);
		pthread_cond_signal(&cond_generator_wakeup);
		pthread_cond_signal(&cond_producer);
		pthread_mutex_unlock(&input_mutex)

		while(isPause) usleep(500);
	}
}
//generators end;

void* operator()
{
	int x, y, flag;
	while (1)
	{
		flag = 0;
		pthread_mutex_lock(&tool_mutex);
		while (num_tool<2) pthread_cond_wait(&condt, &tool_mutex);
		num_tool--; /*get one tool*/
		pthread_mutex_unlock(&tool_mutex);
		pthread_mutex_lock(&tool_mutex);
		if (num_tool == 0)
		{
			num_tool++;/*put back*/
			pthread_mutex_unlock(&tool_mutex);
			continue;
		}
		num_tool--;/*get another tool*/
		pthread_mutex_unlock(&tool_mutex);
		pthread_mutex_lock(&input_mutex);
		while (check_q(ibuffer) == 0) pthread_cond_wait(&提醒operator, &input_mutex);
		x = get_q(ibuffer);/*get one material*/
		pthread_cond_signal(&可以生产);
		pthread_mutex_unlock(&input_mutex);
		pthread_mutex_lock(&input_mutex);
		while (check_q(ibuffer) == 0) pthread_cond_wait(&提醒operator, &input_mutex);
		y = get_q(ibuffer);/*get another material*/
		pthread_cond_signal(&可以生产);
		pthread_mutex_unlock(&input_mutex);
		while (x == y) /*same material or same product as before, change one!*/
		{
			pthread_mutex_lock(&input_mutex);
			while (check_q(ibuffer) == 0) pthread_cond_wait(&提醒operator, &input_mutex);
			y = get_q(ibuffer);
			push_q(ibuffer, x);
			pthread_mutex_unlock(&input_mutex);
		}


		/*Now get all things and begin to produce product*/
		usleep(1000 * (10 + rand() % 991));/* need 0.01s to 1s to produce*/
										   /*put tool back*/
		pthread_mutex_lock(&tool_mutex);
		num_tool += 2;
		pthread_cond_signal(&condt);
		pthread_mutex_unlock(&tool_mutex);
		pthread_mutex_lock(&output_mutex);
		/*wait till the product can be put into the output queue*/
		if (!check_outp(x, y)) {
			pthread_mutex_lock(&wait_mutex);
			num_wait++;
			if (num_wait == num_op) /*deadlock*/
			{
				num_deadlock++;
				num_wait--;
				pthread_mutex_lock(&input_mutex);
				/*abandon this product*/
				num_material_used[x]--;
				num_material_used[y]--;
				pthread_mutex_unlock(&input_mutex);
				pthread_mutex_unlock(&wait_mutex);
				pthread_mutex_unlock(&output_mutex);
				continue; /*drop the product*/
			}
			pthread_mutex_unlock(&wait_mutex);
			flag = 1;
		}
		while (!check_outp(x, y)) pthread_cond_wait(&提醒operator0, &output_mutex);
		pthread_mutex_lock(&wait_mutex);
		outp(x, y); /*output*/
		pthread_cond_signal(&提醒operator0);
		pthread_mutex_unlock(&output_mutex);
		if (flag) num_wait--;/*decrease wait number*/
		pthread_mutex_unlock(&wait_mutex);
		while (pause_signal) usleep(100);

	}
	pthread_exit(0);
}

void* printer(){
	pthread_mutex_lock(&input_mutex);
	printf("%d\n", input_queue->count);
	pthread_mutex_unlock(&input_mutex);
	pthread_exit(0);
}


int main(void) {
	//initialize input queue;
	input_queue = (input_buffer*)malloc(sizeof(input_buffer*));
	input_queue->front = -1;
	input_queue->rear = -INPUT_BUFFER_SIZE;
	input_queue->count = 0;
	input_queue->used = 0;
	input_queue->lastsecond = (int)NULL;

	pthread_t generator_p1,generator_p2,generator_p3,printer_p;

	pthread_create(&generator_p1,NULL,generator1,NULL);
	pthread_create(&generator_p1,NULL,generator2,NULL);
	pthread_create(&generator_p1,NULL,generator3,NULL);
	pthread_create(&printer_p, NULL, printer, NULL);
	pthread_join(generator_p1,NULL);
	pthread_join(generator_p2,NULL);
	pthread_join(generator_p3,NULL);
	pthread_join(printer_p,NULL);


	//destroy mutex and cond;
	pthread_mutex_destroy(&input_mutex);
	pthread_cond_destroy(&generator_c1);
	pthread_cond_destroy(&generator_c2);
	pthread_cond_destroy(&generator_c3);

	

	return 0;
}
