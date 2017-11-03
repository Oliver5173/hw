#include<pthread.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

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
} input_buffer;


//Global
pthread_mutex_t input_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t generator_c1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t generator_c2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t generator_c3 = PTHREAD_COND_INITIALIZER;
input_buffer* input_queue;





void insert_material(input_buffer* buf, int materials){
	if(buf->count != INPUT_BUFFER_SIZE){
		if(buf->rear == -INPUT_BUFFER_SIZE) buf->rear = 0; 
		buf->queue[--(buf->rear)] = materials
		buf->count++;
	}
}

int get_material(input_buffer* buf){
	if(buf->count != 0){
		if(buf->front < -INPUT_BUFFER_SIZE) buf->front = 0;
		buf->count--;
		buf->used++;
		return buf->queue[(buf->front)--]
	}
	return 0;
}

bool isfull(input_buffer* buf) return buf->count == 10;




void* generator1(void *args){
	while(true){
		pthread_cond_wait(&generator_c1,&input_mutex);
		pthread_mutex_lock(&input_mutex);
		insert_material(input_queue,1);
		pthread_cond_signal(&generator_c2);
		pthread_mutex_unlock(&input_mutex);

		while(isfull(input_queue)) usleep(500);
	}
}
void* generator2(void *args){
	while(true){
		pthread_cond_wait(&generator_c2,&input_mutex);
		pthread_mutex_lock(&input_mutex);
		insert_material(input_queue,2);
		pthread_cond_signal(&generator_c3);
		pthread_mutex_unlock(&input_mutex);

		while(isfull(input_queue)) usleep(500);
	}
}
void* generator3(void *args){
	while(true){
		pthread_cond_wait(&generator_c3,&input_mutex);
		pthread_mutex_lock(&input_mutex);
		insert_material(input_queue,3);
		pthread_cond_signal(&generator_c1);
		pthread_mutex_unlock(&input_mutex);

		while(isfull(input_queue)) usleep(500);
	}
}

int main(void) {
	//initialize input queue;
	input_queue = (input_buffer*)malloc(sizeof(input_buffer*));
	input_buffer->front = -1;
	input_buffer->rear = -INPUT_BUFFER_SIZE;
	input_buffer->count = 0;
	input_buffer->used = 0;

	pthread_t generator_p1,generator_p2,generator_p3;

	pthread_create(&generator_p1,NULL,generator1,NULL);
	pthread_create(&generator_p1,NULL,generator2,NULL);
	pthread_create(&generator_p1,NULL,generator3,NULL);
	pthread_join(generator_p1,NULL);
	pthread_join(generator_p2,NULL);
	pthread_join(generator_p3,NULL);


	//destroy mutex and cond;
	pthread_mutex_destroy(&input_mutex);
	pthread_cond_destroy(&generator_c1);
	pthread_cond_destroy(&generator_c2);
	pthread_cond_destroy(&generator_c3);

	

	return 0;
}