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


void enviaArquivo()
{
  struct sockaddr_in addr_envia, addr_recv; 
  int s; 
  int slen = sizeof(addr_recv), recv_len; 
  char buffer[TAMBUFFER]; 
  packet_t packet; 
  char file_name[TAMBUFFER]; 

  
  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    encerra("Erro criando o socket\n");
  }

  
  memset((char *)&addr_envia, 0, sizeof(addr_envia));
  
  addr_envia.sin_family = AF_INET; 
  addr_envia.sin_port = htons(PORTA_PEER); 
  addr_envia.sin_addr.s_addr = htonl(INADDR_ANY); 
  
  

  
  if (bind(s, (struct sockaddr *)&addr_envia, sizeof(addr_envia)) == -1)
  {
    encerra("Erro na bind\n");
  }

  char *returnIp; 
  returnIp = malloc(16);


  
  while (1)
  {
    printf("\nEsperando requisicoes\n");
    fflush(stdout);

    
    
    if ((recv_len = recvfrom(s, file_name, TAMBUFFER, 0, (struct sockaddr *)&addr_recv, &slen)) == -1)
    {
      encerra("Erro recebendo o nome do arquivo\n");
    }
    
    removeCFromString(file_name, '\n');

    FILE *fp; 
    fp = fopen(file_name, "r+"); 
    
    
    
    if (!fp) 
    {
      printf("Nao foi possivel abrir o arquivo '%s'.\n", file_name);
      continue;
    }

    
    fseek(fp, 0, SEEK_END); 
		size_t buffer_size = ftell(fp); 
		fseek(fp, 0, SEEK_SET); 

    
    if(buffer_size > TAMBUFFER) {
      buffer_size = TAMBUFFER;
    }

    
    uint32_t seqn;
    seqn = 0;

    
    printf("Enviando o arquivo '%s'\n", file_name);

    
    while (fread(buffer, buffer_size, 1, fp) > 0)
    {
      unsigned long result;
      result = hash(buffer); 
      
      packet.seqn = seqn; 
      packet.checksum = result; 
      memset(packet.data, 0, TAMBUFFER); 
      memcpy(packet.data, buffer, buffer_size); 

      
      if (sendto(s, &packet, sizeof(packet), 0, (struct sockaddr *)&addr_recv, slen) == -1)
      {
        encerra("Erro ao enviar o pacote!\n");
      }
      
      seqn++;
    }
    fclose(fp);
    printf("O arquivo foi enviado: %s\n", file_name);
  }
  
  free(returnIp);
  
  close(s);
}

void menu()
{
  int option = 0;

  printf("Cliente Envia");
  printf("\n");

  while (1)
  {
    printf("Digite 1 para semear um arquivo e 0 para encerrar o programa");
    printf("\n");
    scanf("%d", &option);

    switch (option)
    {
    case 1:
      enviaArquivo();
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