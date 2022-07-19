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

//definindo estutura do pacote com numero de sequencia de 32 bits sem sinal, checksum e o dado que seria enviado em data
typedef struct
{
  uint32_t seqn; 
  unsigned long checksum; 
  char data[TAMBUFFER]; 
} packet_t;

//Função que exibe o erro e da um exit
void encerra(char *s)
{
  perror(s);
  exit(1);
}

// Função hash
unsigned long hash(unsigned char *str)
{
  unsigned long hash = 5381;
  int c;

  while (c = *str++)
    hash = ((hash << 5) + hash) + c; //descola 5 casas soma hash

  return hash;
}

//Recebe uma o endereço da string, e um char c
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

//Função para enviar arquivo
void enviaArquivo()
{
  struct sockaddr_in addr_envia, addr_recv; //cria strusct para o cliente envia e o recebe.
  int s; 
  int slen = sizeof(addr_recv), recv_len; 
  char buffer[TAMBUFFER]; 
  packet_t packet; 
  char file_name[TAMBUFFER]; 

  //Inicia o socket para linux e verifica se deu erro para encerrar, senao procede
  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    encerra("Erro criando o socket\n");
  }

  //Usa função memset para adicionar o 0 na frente da string repetindo pelo tam da msg
  memset((char *)&addr_envia, 0, sizeof(addr_envia));
  
  //setando valores para o addr_envia
  addr_envia.sin_family = AF_INET; 
  addr_envia.sin_port = htons(PORTA_PEER); 
  addr_envia.sin_addr.s_addr = htonl(INADDR_ANY); 
  
  

  //função bind que vai associar o sockt a porta definida
  if (bind(s, (struct sockaddr *)&addr_envia, sizeof(addr_envia)) == -1)
  {
    encerra("Erro na bind\n");
  }
  // definindo ponteiro pro ip
  char *returnIp; 
  returnIp = malloc(16);
  
  //Loop para aguardar msg
  while (1)
  {
    printf("\nEsperando requisicoes\n");
    fflush(stdout); //essa função ira decarrecar os dados do buffer limpando e gravando em stdout

      bzero(file_name, sizeof(file_name));

    //Calcula o tamanho do arquivo recibido com a funçao recvfrom, e ve se deu erro
    if ((recv_len = recvfrom(s, file_name, TAMBUFFER, 0, (struct sockaddr *)&addr_recv, &slen)) == -1)
    {
      encerra("Erro recebendo o nome do arquivo\n");
    }
    
    //chama a funçao para remover os \n do nome do arquivo
    removeCFromString(file_name, '\n');

    //Cria o ponteiro File e abre o arquivo como leitura
    FILE *fp; 
    fp = fopen(file_name, "r+"); 
    
    
    //Verifica se é possivel abrir o arquivo
    if (!fp) 
    {
      printf("Nao foi possivel abrir o arquivo '%s'.\n", file_name);
      continue;
    }

    
    fseek(fp, 0, SEEK_END); //funçao para mover o ponteiro pro final
		size_t buffer_size = ftell(fp); //pega o tamanho do arquivo usando a posição atual do ponteiro
		fseek(fp, 0, SEEK_SET);  //altera o a posição do ponteiro pro inicio

    //deixa o buffer_size valendo o tamanho do buffer ou o tamanho maximo pré definido (caso ultrapasse)
    if(buffer_size > TAMBUFFER) {
      buffer_size = TAMBUFFER;
    }

    
    uint32_t seqn;
    seqn = 0;

    
    printf("Enviando o arquivo '%s'\n", file_name);

    //loop para ler o buffer enquanto foi maior que 0
    while (fread(buffer, buffer_size, 1, fp) > 0)
    {
      unsigned long result; 
      result = hash(buffer); //envia os dados do buffer para a função hash e salva em result
      
      packet.seqn = seqn;  //Seta o valor de sequencia do pacote
      packet.checksum = result; //salva o valor vindo da função hash pro checksum
      memset(packet.data, 0, TAMBUFFER); 
      memcpy(packet.data, buffer, buffer_size);  //Seta a mensagem que sera enviada, copiando do buffer pro packet.data

      //Começa enviando o pacote, se der erro chama a funçao que ira imprimir o erro e encerrar programa
      if (sendto(s, &packet, sizeof(packet), 0, (struct sockaddr *)&addr_recv, slen) == -1)
      {
        encerra("Erro ao enviar o pacote!\n");
      }
      
      seqn++; //incrementa na sequencia do pacote
    }
    fclose(fp); //fecha arquivo
    printf("O arquivo foi enviado: %s\n", file_name);
  }
  
  free(returnIp); //libera o espaço armazenado pro ip
  
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