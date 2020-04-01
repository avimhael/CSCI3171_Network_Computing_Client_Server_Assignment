// Server.c
// Elijah Whynot & Matt Ward 
// Network Computing CSCI 3171.03 Winter 2020
// This file sets up a server and computes logic for the prisoner's dilemma game

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SERVERBACKLOG 20


// This function performs initial setup on the server and asks the user what IP/port they want to use
int setUpServerAndReturnSocket()
{

  //Specify IPV4 family, Set Port to User Selected Port, Bind IP Interfaces
  struct sockaddr_in serverAddress;
  int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;

  for (;;)
  {
    int port;
    printf("Please enter the port you would like to connect to:\n");
    scanf("%d", &port);
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0)
    {
      printf("Binding the socket %i to the IP address failed! \n", serverSocket);
      exit(0);
    }
    else
    {
      printf("Successfully bound the socket %i to an IP address! \n", serverSocket);
    }

    if (listen(serverSocket, SERVERBACKLOG) != 0)
    {
      printf("Server failed to start on port %i!\n", port);
    }
    else
    {
      printf("Server started successfully and is listening on port %i!\n", port);
      break;
    }
  }
  return serverSocket;
}

// This function computes the logic for the prisoner's dilemma game
char *compareChoices(char clientInput)
{
  char serverRandomizedResponse = rand() % 2 == 0 ? 'b' : 's';
  char *response;
  response = malloc(sizeof(char) * 100);

  strcat(response, "Server Response: ");

  if (serverRandomizedResponse == 'b' && clientInput == 'b')
  // Client and Server have both betrayed
  {
    strcat(response, "You have both betrayed! You each get 2 years in jail!");
  }
  else if (serverRandomizedResponse == 's' && clientInput == 's')
  //Client and Server have both remained silent
  {
    strcat(response, "You have both remained silent! You each get 1 year in jail!");
  }
  else if (serverRandomizedResponse == 'b' && clientInput == 's')
  // Server has betrayed, Client has remained silent
  {
    strcat(response, "The server has betrayed you even though you remained silent! You get a 3 year jail sentence!");
  }
  else if (serverRandomizedResponse == 's' && clientInput == 'b')
  // Server has remained silent, Client has betrayed
  {
    strcat(response, "You have betrayed the server while they remained silent! You are set free and they get 3 years in jail!");
  }
  strcat(response, "\n");
  return response;
}

// This function connects to the client in order to pass back information
int connectToClient(int serverSocket)
{

  struct sockaddr_in requestingClientAddress;
  socklen_t requestingClientAddressSize = sizeof(requestingClientAddress);
  int serverConnectionSocket = accept(serverSocket, (struct sockaddr *)&requestingClientAddress, &requestingClientAddressSize);

  if (serverConnectionSocket < 0)
  {
    printf("Connection to client via socket %i failed!", serverSocket);
    exit(0);
  }
  printf("Connection to client via socket successful!\n");
  return serverConnectionSocket;
}

int main(void)
{
  int serverSocket = setUpServerAndReturnSocket();
  int serverConnectionSocket = connectToClient(serverSocket);
  for (;;)
  {
    char clientInput[80];
    bzero(clientInput, 80);

    read(serverConnectionSocket, clientInput, sizeof(clientInput));

    if (strcmp("no", clientInput) == 0)
    {
      printf("Exit command recieved.\nClosing connection.\nServer Exit...\n");
      close(serverSocket);
      exit(0);
    }
    else
    {
      char *response = compareChoices(clientInput[0]);
      char serverResponse[180];
      bzero(serverResponse, 180);
      strcat(serverResponse, response);
      write(serverConnectionSocket, serverResponse, sizeof(serverResponse));
    }
  }
}
