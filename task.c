#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main() {
  int ShmID;
  int *ShmPTR;
  pid_t pid;
  int BankAccount = 0;
  int Turn = 0;
  
  // Seed random for sleeping calls.
  srand(time(NULL)); 
  // Create ShmId for two integers. 
  ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
  
  if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
  }
  
  ShmPTR = (int *) shmat(ShmID, NULL, 0);
  if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
  }
  
  // Assign pointers for each of the integer variables.
  ShmPTR[0] = BankAccount;
  ShmPTR[1] = Turn;
  
  int i;
  pid = fork();
  if (pid < 0) {
    printf("*** fork error (server) ***\n");
    exit(1);
  } else if (pid > 0) {
    for (i = 0; i < 25; i++) {
      sleep(rand() % 6); // sleep for 0-5 seconds inclusive.
      int account = ShmPTR[0];
      
      while (ShmPTR[1] != 0) {} // Loop while Turn != 0.
      
      if (account <= 100) {
        int balance = rand() % 101; // Randomly generate money to deposit between 0 and 100 inclusive.
        
        if ((balance % 2) == 0) {
          account += balance;
          printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
          ShmPTR[0] = account;
        } else {
          printf("Dear old Dad: Doesn't have any money to give\n");
        }
      } else {
        printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
      }
      ShmPTR[1] = 1;
    }
  } else {
    for(i = 0; i < 25; i++) {
      sleep(rand() % 5);
      int account = ShmPTR[0];
      
      while (ShmPTR[1] != 1) {} // Loop while Turn != 1
      
      int balance = rand() % 51; // generate a balance between 0 and 50 inclusive.
      printf("Poor Student needs $%d\n", balance);
      
      if (balance <= account) {
        account -= balance;
        printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
        ShmPTR[0] = account;
      } else {
        printf("Poor Student: Not Enough Cash ($%d)\n", account );
      }
      ShmPTR[1] = 0;
    }
  }
}