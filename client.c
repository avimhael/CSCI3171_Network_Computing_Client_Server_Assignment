// Client.c
// Elijah Whynot & Matt Ward
// Network Computing CSCI 3171.03 Winter 2020
// This file asks for user input for the game, and communicates with the server.

#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <ctype.h>
#define MAX 80
#define PORT 3001
#define SA struct sockaddr
int serverConnection, serverSocket;

void socketCreationFailed()
{
  printf("Failed to create socket!\n");
  exit(0);
}

void serverConnectionFailed()
{
  printf("Failed to connect to server!\n");
  exit(0);
}

int connectToServer(char ip[10], int port)
{
  //Create Socket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  serverSocket == -1 ? socketCreationFailed() : printf("Successfully created socket.\n\n");

  //Define and clear server address structure.
  struct sockaddr_in serverAddress;
  bzero(&serverAddress, sizeof(serverAddress));

  //Specify IPV4 family Set Port, Bind IP Interface
  serverAddress.sin_addr.s_addr = inet_addr(ip);
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(PORT);
  //Connect server to client via sockets
  int connection = connect(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
  connection != 0 ? serverConnectionFailed() : printf("Successfully connected to the server!\n");
  return connection;
}

void communicateWithServer()
{
  for (;;)
  {
    printf("Please enter 'S' for Silent or 'B' for Betray ");
    fflush(stdin);
    //Convert to lowercase
    char userDecision;
    scanf(" %c", &userDecision);
    if (userDecision < 97)
    {
      userDecision += 32;
    }
    if (userDecision != 's' && userDecision != 'b')
    {
      printf("Incorrect Decision Entry, please try again.\n");
      fflush(stdin);
    }
    else
    {
      //Pass response to server.
      write(serverSocket, &userDecision, sizeof(userDecision));
      char serverResponse[180];
      read(serverSocket, serverResponse, sizeof(serverResponse));
      printf("%s", serverResponse);
      fflush(stdin);
      break;
    }
  }
}

void prisonersDilemmaApp(int serverSocket)
{
  printf("Welcome to the Prisoner's Dilemma game.\n\nDescription:\nTwo members of a criminal gang are arrested and imprisoned.\nEach prisoner is in solitary confinement with no means of communicating with the other.\nThe prosecutors lack sufficient evidence to convict the pair on the principal charge. \nThey hope to get both sentenced to a year in prison on a lesser charge. \nSimultaneously, the prosecutors offer each prisoner a bargain. \nEach prisoner is given the opportunity either to: \nBetray the other by testifying that the other committed the crime, \nor to cooperate with the other by remaining silent. \nThe offer is: If A and B each betray the other, each of them serves 2 years in prison. \nIf A betrays B but B remains silent, A will be set free and B will serve 3 years in prison \n(and vice versa).\nIf A and B both remain silent, both will only serve 1 year in prison \n(on the lesser charge)\n\n");
  communicateWithServer();

  for (;;)
  {
    printf("Would you like to play again ? Enter: Yes/No ");
    fflush(stdin);
    char userResponse[3];
    int i = 0;
    scanf("%s", userResponse);
    for (i = 0; i <= strlen(userResponse); i++)
    {
      if (userResponse[i] >= 65 && userResponse[i] <= 90)
        userResponse[i] = userResponse[i] + 32;
    }
    //Convert any uppercase in answer to lowecase
    size_t ln = strlen(userResponse) - 1;
    if (*userResponse && userResponse[ln] == '\n')
      userResponse[ln] = '\0';
    if (strcmp("yes", userResponse) != 0 && strcmp("no", userResponse) != 0)
    {
      printf("Incorrect Entry, please try again.\n");
      fflush(stdin);
    }
    else
    {
      if (strcmp("no", userResponse) == 0)
      {
        //Pass response to server
        write(serverSocket, userResponse, sizeof(userResponse));
        printf("\nClient Exit...\n");
        break;
      }
      else
      {
        communicateWithServer();
      }
    }
  }
}

int main()
{
  for (;;)
  {
    int i = 0;
    int p = 0;
    int port;
    char ip[10];

    printf("Prior to beginning Prisoners Dilemma, please enter the server IP address you would like to connect to:\n");
    while ((ip[i++] = getchar()) != '\n')
      ;

    printf("Please enter the port you would like to connect to:\n");
    scanf("%d", &port);
    if (connectToServer(ip, port) == 0)
    {
      break;
    }
    else
    {
      printf("The Port Number or IP Address entered is invalid, please try again.");
    }
  }
  prisonersDilemmaApp(serverSocket);
  close(serverSocket);
}