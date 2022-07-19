#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define TAMBUFFER 512
#define SEQN 4

#define PORTA_SERVER 8000
#define PORTA_PEER 9000

typedef struct
{
  uint32_t seqn;
  unsigned long checksum;
  char data[TAMBUFFER];
} packet_t;

void encerra(char *s)
{
  perror(s);
  exit(1);
}


unsigned long hash(unsigned char *str)
{
  unsigned long hash = 5381;
  int c;

  while (c = *str++)
    hash = ((hash << 5) + hash) + c;

  return hash;
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

void recebeFile()
{
  struct sockaddr_in addr_server, addr_recv;
  int sTracker, sTrackerlen = sizeof(addr_server);
  int sPeer, sPeerlen = sizeof(addr_recv);
  packet_t packet;
  char fileName[TAMBUFFER];
  char buf[TAMBUFFER];

  printf("Digite o nome do arquivo a ser procurado: ");
  scanf("%s", fileName);

  if ((sTracker = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    encerra("Erro criando o socket\n");
  }

  memset((char *)&addr_server, 0, sizeof(addr_server));

  addr_server.sin_family = AF_INET;
  addr_server.sin_port = htons(PORTA_SERVER);
  addr_server.sin_addr.s_addr = INADDR_ANY;

  if (sendto(sTracker, fileName, strlen(fileName), 0, (struct sockaddr *)&addr_server, sTrackerlen) == -1)
  {
    encerra("Erro enviando o nome do arquivo\n");
  }

  recvfrom(sTracker, (char *)buf, TAMBUFFER, 0, (struct sockaddr *)&addr_server, &sTrackerlen);

  if (strcmp(buf, "0.0.0.0") == 0)
  {
    printf("Nao existe nenhum cliente com o arquivo solicitado\n");
    return;
  }

  if ((sPeer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    encerra("Erro criando o socket\n");
  }

  memset((char *)&addr_recv, 0, sizeof(addr_recv));

  addr_recv.sin_family = AF_INET;
  addr_recv.sin_port = htons(PORTA_PEER);
  addr_recv.sin_addr.s_addr = inet_addr(buf);

  
  struct timeval read_timeout;
  read_timeout.tv_sec = 2;
  read_timeout.tv_usec = 0;

  //a funçao abaixo ira alterar as opções do socket sPeer para o temporizador 
  setsockopt(sPeer, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

  //envia o pedido do arquivo
  if (sendto(sPeer, fileName, strlen(fileName), 0, (struct sockaddr *)&addr_recv, sPeerlen) == -1)
  {
    encerra("Erro enviando a requisicao\n");
  }

  int checksum_error = 0; //a variavel checksum começa com o valor 0

  FILE *fp; //inicia um ponteiro FILE
  char fileDownloadName[TAMBUFFER]; //Define uma variavel para o arquivo 
  sprintf(fileDownloadName, "%s", fileName); //Converte para string o nome do arquivo 

  fp = fopen(fileDownloadName, "w+"); //agora pode criar o arquivo com o nome correto

  //enquanto estiver recebendo dados
  while (recvfrom(sPeer, &packet, sizeof(packet), 0, (struct sockaddr *)&addr_recv, &sPeerlen) > 0)
  {
    unsigned long result;
    result = hash(packet.data);
    //recebe o pacote e aplica a funçao hash para calcular o valor do checksum, depois verifica se esse valor é igual ao checksum enviado com o pacote
    if (packet.checksum != result) //Se diferer é erro
    {
      printf("Numero de sequencia errado %d\n", packet.seqn);
      checksum_error++; //em caso de erro incrementa o valor de checksum_error
    }

    fwrite(packet.data, sizeof(packet.data), 1, fp); //Salva as informações recebidas.
  }
  //depois de receber tudo fecha o arquivo
  fclose(fp);

  //ve se houve erros no checksum
  if (checksum_error > 0)
  {
    printf("Erro na check sum de %d pacotes.\n", checksum_error);
  }
  else
  {
    printf("\nO arquivo foi baixado com sucesso\n");
  }
}


void menu()
{
  int option = 0;

  printf("Cliente Peer");
  printf("\n");

  while (1)
  {
    printf("Digite 1 para requisitar um arquivo e 0 para encerrar o programa");
    printf("\n");
    scanf("%d", &option);

    switch (option)
    {
    case 1:
      recebeFile();
      break;
    case 0:
      exit(0);
      break;
    default:
      printf("Digite uma opcao valida\n");
    }
  }

  return;
}

int main()
{
  menu();
  return 0;
}