#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define TAMBUFFER 512
#define PORT 8000

void encerra(char *s)
{
  perror(s);
  exit(1);
}

void removeCFromString(char *string, char c)
{
  int i;
  for (i = 0; i < strlen(string); i++)
  {
    if (string[i] == c)
    {
      string[i] = '\0';
    }
  }
}

void getIpForFile(char *requestedFileName, char *rtrIp)
{
  char ip[16], fileName[32];
  int i;

  FILE *file = fopen("bd.txt", "rt");

  while ((fscanf(file, "%s %s\n", &ip, &fileName)) != EOF)
  {

    if ((strcmp(fileName, requestedFileName)) == 0)
    {
      fclose(file);

      strcpy(rtrIp, ip);
      return;
    }
  }

  strcpy(rtrIp, "0.0.0.0");
  fclose(file);
}

void startServer()
{
  struct sockaddr_in si_me, si_other;

  int s;
  int slen = sizeof(si_other), recv_len;
  char buf[TAMBUFFER];

  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    encerra("socket");
  }

  memset((char *)&si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
  {
    encerra("bind");
  }

  char *returnIp;
  returnIp = malloc(16);

  while (1)
  {
    printf("\nEsperando requisicoes\n");
    fflush(stdout);

    bzero(buf, TAMBUFFER);

    if ((recv_len = recvfrom(s, buf, TAMBUFFER, 0, (struct sockaddr *)&si_other, &slen)) == -1)
    {
      encerra("Erro recebendo o nome do arquivo\n");
    }

    removeCFromString(buf, '\n');

    printf("Uma requisicao foi recebida de %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    printf("Nome do arquivo: %s\n", buf);

    getIpForFile(buf, returnIp);

    printf("Arquivo disponivel em %s\n", returnIp);

    if (sendto(s, returnIp, strlen(returnIp), 0, (struct sockaddr *)&si_other, slen) == -1)
    {
      encerra("Erro enviando endereco IP\n");
    }
  }

  free(returnIp);

  close(s);
}

int main()
{
  printf("Servidor");
  printf("\n");

  startServer();

  return 0;
}