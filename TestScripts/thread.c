#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func(void *param);
void *thread_func1(void *param);

int main(void){
	pthread_t thread;
	pthread_t thread1;
	int ret = 0;

	// スレッドの作成
	// int pthread_create(pthread_t * thread, pthread_attr_t * attr, void * (*start_routine)(void *), void * arg)
	// 第４引数に渡した値が，スレッド関数の引数に渡される
	ret = pthread_create(&thread, NULL, thread_func, NULL);
	if (ret != 0) {
		// pthread_createが失敗した場合は0以外の値を返す
		exit(1);
	}

	ret = pthread_create(&thread1, NULL, thread_func1, NULL);
	if (ret != 0) {
		// pthread_createが失敗した場合は0以外の値を返す
		exit(1);
	}

	// スレッドの終了を待機
	// int pthread_join(pthread_t th, void **thread_return);
	ret = pthread_join(thread, NULL);
	if (ret != 0) {
		// pthread_joinが失敗した場合は0以外の値を返す
		exit(1);
	}

	ret = pthread_join(thread, NULL);
	if (ret != 0) {
		// pthread_joinが失敗した場合は0以外の値を返す
		exit(1);
	}

	// // スレッドの終了を待たない
	// // int pthread_detach(pthread_t th);
	// ret = pthread_detach(thread);
	// if (ret != 0) {
	// 	// pthread_detachが失敗した場合は0以外の値を返す
	// 	exit(1);
	// }

	// ret = pthread_detach(thread1);
	// if (ret != 0) {
	// 	// pthread_detachが失敗した場合は0以外の値を返す
	// 	exit(1);
	// }

	return EXIT_SUCCESS;
}

void *thread_func(void *param){
	for (int i = 0; i < 5; i++) {
		printf("Hello world from thread1\n");
		sleep(2);
	}
	return 0;
}

void *thread_func1(void *param){
	for (int i = 0; i < 5; i++) {
		printf("Hello world from thread2\n");
		sleep(1);
	}
	return 0;
}
