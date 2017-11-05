#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// materials 1,2,3
//tools 1,2,3
//products A,B,C

//CONSTANT
#define INPUT_BUFFER_SIZE 10
//#define NUMBER_OF_TOOLS 3

//struct
typedef struct{
	int front;
	int rear;
	int count;
	int materials_used[3];
	int lastsecond;
} input_buffer;

typedef struct{
	int front;
	int rear;
	int count;
	int amount_product[3];
} output_buffer;
/*
typedef struct{
	int tool_1
	int tool_2
	int tool_3
//	int tools_availble
} tool;
*/
//Global
pthread_mutex_t input_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t output_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tool_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t operator_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t pause_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_full_wait_for_generate = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_generator_wakeup = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_tool_1_availble = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_tool_2_availble = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_tool_3_availble = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_wait_for_produce = PTHREAD_COND_INITIALIZER;

input_buffer* input_queue;
int* input_ary;
output_buffer* output_queue;
char* output_ary;
int* toolkit;
bool isPause = false;
int deadlock = 0;
int operator_wait = 0;
int number_operators = 3; //default 3 operators;




//input_queue methods begin;
void insert_material(input_buffer* buf,int materials){
	if(buf->count != INPUT_BUFFER_SIZE){
		buf->materials_used[materials - 1]++;
		if(buf->count > 1) buf->lastsecond = *(input_ary + buf->rear);
		if(buf->rear == INPUT_BUFFER_SIZE - 1) buf->rear = -1;
		buf->rear++;
		*(input_ary + buf->rear) = materials;
		buf->count++;
	}
}


int get_material(input_buffer* buf,int* ary){
	if(buf->count != 0){
		if(buf->front == 10) buf->front = 0;
		buf->count--;
		int temp = *(ary + buf->front);
		buf->front++;
		
		return temp;
	}
	return 2;
}

bool isfull(input_buffer* buf) {return buf->count == 10;}


void print_input_queue(input_buffer* buf, int* ary){
	printf("Input buffer (front_rear):  ");
	int front = buf->front;
	int rear = buf->rear;
	if(front <= rear)
		for(int i = front;i <= rear;i++)
			printf("%d ", *(ary + i));
	else{
		for(int i = front; i < INPUT_BUFFER_SIZE;i++) printf("%d ",*(ary + i));
		for(int i = 0; i <= rear; i++) printf("%d ",*(ary + i));
	}
	printf("\n");
}

//input_queue methods end;

//output_queue methods begin;
void insert_product(output_buffer* buf,char* ary,char product){
	buf->rear++;
	*(ary + buf->rear) = product;
	buf->count++;
	switch(product){
		case 'A':
			buf->amount_product[0]++;
			break;
		case 'B':
			buf->amount_product[1]++;
			break;
		case 'C':
			buf->amount_product[2]++;
			break;

	}
}

//output_queue methods end;



//pause control;
void* myPause(){
	char cmd;
	while(true){
		//pthread_mutex_lock(&pause_mutex);
		cmd = getchar();
		if(cmd == 'p') {isPause = true;
			pthread_mutex_lock(&input_mutex);
			printf("material 1: %d, material 2: %d, material 3: %d\n",input_queue->materials_used[0],input_queue->materials_used[1],input_queue->materials_used[2]);
			print_input_queue(input_queue,input_ary);
			pthread_mutex_unlock(&input_mutex);
		
			pthread_mutex_lock(&output_mutex);
			printf("product A: %d, product B: %d, product C: %d\n",output_queue->amount_product[0],output_queue->amount_product[1],output_queue->amount_product[2]);
			printf("Output buffer (front -> rear):  ");
			for(int i = 0; i < output_queue->count; i++){
				printf("%c  ", *(output_ary + i));
			}
			printf("\n\n\n\n");
			pthread_mutex_unlock(&output_mutex);
		}
		else if(cmd == 'r') isPause = false;
	}
	pthread_exit(0);
}

//generators begin;
void* generator_first(){
	int materials = 1;
	while(true){
		pthread_mutex_lock(&input_mutex);

		while(input_queue->count > 0 && (*(input_ary + input_queue->rear) == materials || input_queue->lastsecond == materials) || isfull(input_queue)){
			//input queue is full;
			if(isfull(input_queue)) pthread_cond_wait(&cond_full_wait_for_generate,&input_mutex);
			//bad input sequence;
			else{
				pthread_cond_signal(&cond_full_wait_for_generate);
				pthread_cond_signal(&cond_generator_wakeup);
				pthread_cond_wait(&cond_generator_wakeup,&input_mutex);
			}
		}
		insert_material(input_queue,materials);
		pthread_cond_signal(&cond_generator_wakeup);
		pthread_cond_signal(&cond_producer);
		pthread_mutex_unlock(&input_mutex);
		
		while(isPause) usleep(500);
	}
	pthread_exit(0);
}

void* generator_second(){
	int materials = 2;
	while(true){
		pthread_mutex_lock(&input_mutex);
		while(input_queue->count > 0 && (*(input_ary + input_queue->rear) == materials || input_queue->lastsecond == materials) || isfull(input_queue)){
			if(isfull(input_queue)) pthread_cond_wait(&cond_full_wait_for_generate,&input_mutex);
			else{
				pthread_cond_signal(&cond_full_wait_for_generate);
				pthread_cond_signal(&cond_generator_wakeup);
				pthread_cond_wait(&cond_generator_wakeup,&input_mutex);
			}
		}

		insert_material(input_queue,materials);
		pthread_cond_signal(&cond_generator_wakeup);
		//for empty input queue;
		pthread_cond_signal(&cond_producer);
		pthread_mutex_unlock(&input_mutex);

		while(isPause) usleep(500);
	}
	pthread_exit(0);
}

void* generator_third(){
	int materials = 3;
	while(true){
		pthread_mutex_lock(&input_mutex);
		while(input_queue->count > 0 && (*(input_ary + input_queue->rear) == materials || input_queue->lastsecond == materials) || isfull(input_queue)){
			if(isfull(input_queue)) pthread_cond_wait(&cond_full_wait_for_generate,&input_mutex);
			else{
				pthread_cond_signal(&cond_full_wait_for_generate);
				pthread_cond_signal(&cond_generator_wakeup);
				pthread_cond_wait(&cond_generator_wakeup,&input_mutex);
			}
		}

		insert_material(input_queue,materials);
		pthread_cond_signal(&cond_generator_wakeup);
		pthread_cond_signal(&cond_producer);
		pthread_mutex_unlock(&input_mutex);

		while(isPause) usleep(500);
	}
	pthread_exit(0);
}
//generators end;

char product_map(int a,int b){
	int sum = a +b;
	if(sum == 3) return 'A';
	else if(sum == 4) return 'B';
	else return 'C';
}
bool can_produce(char product){
	int product_A = output_queue->amount_product[0];
	int product_B = output_queue->amount_product[1];
	int product_C = output_queue->amount_product[2];
	switch(product){
		case 'A':
			if(abs(product_A + 1 - product_B) < 10 && abs(product_A + 1 - product_C) < 10) return true;
		case 'B':
			if(abs(product_B + 1 - product_A) < 10 && abs(product_B + 1 - product_C) < 10) return true;
		case 'C':
			if(abs(product_C + 1 - product_A) < 10 && abs(product_C + 1 - product_B) < 10) return true;
	}
	return false;

}

void* operator(){
	int material_1,material_2,backup;
	bool flag;
	while (true){
		flag = false;
		//get materials
		pthread_mutex_lock(&input_mutex);
		while (input_queue->count == 0) pthread_cond_wait(&cond_producer, &input_mutex);
		material_1 = get_material(input_queue,input_ary);
		pthread_cond_signal(&cond_full_wait_for_generate);
		pthread_mutex_unlock(&input_mutex);

		pthread_mutex_lock(&input_mutex);
		while (input_queue->count == 0) pthread_cond_wait(&cond_producer, &input_mutex);
		material_2 = get_material(input_queue,input_ary);
		pthread_cond_signal(&cond_full_wait_for_generate);
		pthread_mutex_unlock(&input_mutex);

		while (material_1 == material_2){
			pthread_mutex_lock(&input_mutex);
			while (input_queue->count == 0) pthread_cond_wait(&cond_producer, &input_mutex);
			backup = get_material(input_queue,input_ary);
			insert_material(input_queue,material_2);
			material_2 = backup;
			pthread_mutex_unlock(&input_mutex);
		}


		//get the first tool;
		pthread_mutex_lock(&tool_mutex);
		//tool not availble
		while(*(toolkit + material_1 - 1) == 0){
			switch(material_1){
				case 1:
					pthread_cond_wait(&cond_tool_1_availble,&tool_mutex);
					break;
				case 2:
					pthread_cond_wait(&cond_tool_2_availble,&tool_mutex);
					break;
				case 3:
					pthread_cond_wait(&cond_tool_3_availble,&tool_mutex);
					break;
			}
			pthread_cond_signal(&cond_tool_1_availble);
			pthread_cond_signal(&cond_tool_2_availble);
			pthread_cond_signal(&cond_tool_3_availble);

			break;
		}
		*(toolkit + material_1 - 1) = 0;
		pthread_mutex_unlock(&tool_mutex);


		//get another tool
		pthread_mutex_lock(&tool_mutex);
		//tool not availble
		while(*(toolkit + material_2 - 1) == 0){
			switch(material_1){
				case 1:
					pthread_cond_wait(&cond_tool_1_availble,&tool_mutex);
					break;
				case 2:
					pthread_cond_wait(&cond_tool_2_availble,&tool_mutex);
					break;
				case 3:
					pthread_cond_wait(&cond_tool_3_availble,&tool_mutex);
					break;
			}
			pthread_cond_signal(&cond_tool_1_availble);
			pthread_cond_signal(&cond_tool_2_availble);
			pthread_cond_signal(&cond_tool_3_availble);
			break;
		}
		*(toolkit + material_2 - 1) = 0;
		pthread_mutex_unlock(&tool_mutex);

		usleep(1000*(10+rand()%991));
		pthread_mutex_lock(&output_mutex);
		if (!can_produce(product_map(material_1,material_2))) {
			pthread_mutex_lock(&operator_wait_mutex);
			operator_wait++;
			if (operator_wait == number_operators) {
				deadlock++;
				operator_wait--;
				pthread_mutex_unlock(&operator_wait_mutex);
				pthread_mutex_unlock(&output_mutex);
				continue;
			}
			pthread_mutex_unlock(&operator_wait_mutex);
			flag = true;
		}
		while (!can_produce(product_map(material_1,material_2))) pthread_cond_wait(&cond_wait_for_produce, &output_mutex);
		pthread_mutex_lock(&operator_wait_mutex);

		char product = product_map(material_1,material_2);

		insert_product(output_queue,output_ary,product);

		pthread_mutex_lock(&tool_mutex);
		*(toolkit + material_1 - 1) = 1;
		*(toolkit + material_2 - 1) = 1;
		switch(material_1){
			case 1:
				pthread_cond_signal(&cond_tool_1_availble);
				break;
			case 2:
				pthread_cond_signal(&cond_tool_2_availble);
				break;
			case 3:
				pthread_cond_signal(&cond_tool_3_availble);
				break;
		}
		switch(material_2){
			case 1:
				pthread_cond_signal(&cond_tool_1_availble);
				break;
			case 2:
				pthread_cond_signal(&cond_tool_2_availble);
				break;
			case 3:
				pthread_cond_signal(&cond_tool_3_availble);
				break;
		}

		pthread_mutex_unlock(&tool_mutex);
		

		pthread_cond_signal(&cond_wait_for_produce);
		pthread_mutex_unlock(&output_mutex);
		if (flag) operator_wait--;
		pthread_mutex_unlock(&operator_wait_mutex);
		while (isPause) usleep(500);

	}
	pthread_exit(0);
}

void* printer(){
	while(1){
		while(isPause) usleep(500);
		printf("DeadBlocks:%d\n",deadlock);
		pthread_mutex_lock(&input_mutex);
		printf("material 1: %d, material 2: %d, material 3: %d\n",input_queue->materials_used[0],input_queue->materials_used[1],input_queue->materials_used[2]);
		print_input_queue(input_queue,input_ary);
		pthread_mutex_unlock(&input_mutex);
		
		pthread_mutex_lock(&output_mutex);
		printf("product A: %d, product B: %d, product C: %d\n",output_queue->amount_product[0],output_queue->amount_product[1],output_queue->amount_product[2]);
		printf("Output buffer (front -> rear):  ");
		for(int i = 0; i <= output_queue->count; i++){
			printf("%c  ", *(output_ary + i));
		};
		printf("\n\n\n\n");
		pthread_mutex_unlock(&output_mutex);
		
		sleep(1);
	}
	pthread_exit(0);
}



int main(void) {
	//initialize input queue;

	input_queue = (input_buffer*)malloc(sizeof(input_buffer));
	input_queue->front = 0;
	input_queue->rear = -1;
	input_queue->count = 0;
	input_queue->materials_used[0] = 0;
	input_queue->materials_used[1] = 0;
	input_queue->materials_used[2] = 0;
	input_queue->lastsecond = -1;
	input_ary = (int*)malloc(INPUT_BUFFER_SIZE*sizeof(int));


	//initialize output queue;
	output_queue = (output_buffer*)malloc(sizeof(output_queue));
	output_queue->rear = -1;
	output_queue->front = 0;
	output_queue->count = 0;
	output_queue->amount_product[0] = 0;
	output_queue->amount_product[1] = 0;
	output_queue->amount_product[2] = 0;
	output_ary = (char*)malloc(sizeof(char) * 1000);

	//initialize toolkit,0 represents unavailble,otherwise availble;
	toolkit = (int*)malloc(3*sizeof(int));
	*toolkit = 1;
	*(toolkit+1) = 1;
	*(toolkit+2) = 1;
	printf("\033[2J\033[1;1H");
	printf("Enter p(low_case) to pause, Enter r(low_case) to resume\n");
	printf("Does not supply signal method, you have to press \"ENTER\"\n");
	printf("How many operators(1-20) :");
	scanf("%d",&number_operators);
	pthread_t operators_p[20];
	for(int i = 0; i <= number_operators; i++)
		pthread_create(&operators_p[i],NULL,operator,NULL);

	pthread_t generator_p1,generator_p2,generator_p3,printer_p,pause_p;
	pthread_create(&pause_p,NULL,myPause,NULL);
	pthread_create(&generator_p1,NULL,generator_first,NULL);
	pthread_create(&generator_p2,NULL,generator_second,NULL);
	pthread_create(&generator_p3,NULL,generator_third,NULL);
	pthread_create(&printer_p, NULL, printer, NULL);
	pthread_join(pause_p,NULL);

	pthread_join(generator_p1,NULL);
	pthread_join(generator_p2,NULL);
	pthread_join(generator_p3,NULL);
	for(int i = 0; i <= number_operators; i++) pthread_join(operators_p[i],NULL);
	
	pthread_join(printer_p,NULL);

	

	


	//destroy mutex and cond;
	pthread_mutex_destroy(&input_mutex);
	pthread_mutex_destroy(&output_mutex);
	pthread_mutex_destroy(&tool_mutex);
	pthread_mutex_destroy(&operator_wait_mutex);
	pthread_cond_destroy(&cond_full_wait_for_generate);
	pthread_cond_destroy(&cond_generator_wakeup);
	pthread_cond_destroy(&cond_producer);
	pthread_cond_destroy(&cond_tool_1_availble);
	pthread_cond_destroy(&cond_tool_2_availble);
	pthread_cond_destroy(&cond_tool_3_availble);
	pthread_cond_destroy(&cond_wait_for_produce);

	

	return 0;
}
