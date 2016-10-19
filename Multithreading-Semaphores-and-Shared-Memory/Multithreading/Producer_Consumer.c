#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int N, front, rear, count;
int buff[101], read[15];
pthread_t tid[15];

pthread_mutex_t TMutex = PTHREAD_MUTEX_INITIALIZER;

void *reading(void *arg) {

  int ind, i;
  pthread_t id = pthread_self();
  for (i = 0; i < N; i++) {
    if (pthread_equal(id, tid[i])) {
      ind = i;
      break;
    }
  }

  while (1) {
    pthread_mutex_lock(&TMutex);
    if (read[ind] == 0 && buff[rear] != 0) {
      printf("Value = %d, read by reader no: %d\n", buff[rear], ind);
      read[ind] = 1;
      count += 1;
      printf("Current count: %d\n", count);
      if (count == N) {
        memset(read, 0, sizeof(read));
        count = 0;
        printf("Count has been reset\n");
        buff[rear] = 0;
        rear = (rear + 1) % N;
      }
    }
    pthread_mutex_unlock(&TMutex);
  }
  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {

  int i, err;
  srand(time(NULL));
  front = -1;
  rear = count = 0;
  printf("Enter the number of readers: ");
  scanf("%d", &N);

  for (i = 0; i < N; i++) {
    err = pthread_create(&tid[i], NULL, reading, NULL);
  }

  // writer program
  while (1) {
    pthread_mutex_lock(&TMutex);
    if (buff[(front + 1) % N] == 0) {
      printf("writing\n");
      front = (front + 1) % N;
      buff[front] = rand() % 23 + 1;
      printf("Front: %d\n", front);
    }
    pthread_mutex_unlock(&TMutex);
  }
  
  pthread_exit(NULL);
  return 0;
}
