UNIVERIDADE FEDERAL DE ITAJUBÁ
Disciplina COM240 - Redes de Computadores
Professor Bruno Guazzelli Batista

Alunos e Matrículas:
Cristian Stoenica 2020011023
Gustavo Salles 2020005750
Natália Mattos 2020006856

Instruções para compilação e execução
Na pasta cliente_envia:
  - Compilar - gcc cliente_envia.c - o cliente_envia
Na pasta cliente_recebe:
  - Compilar - gcc cliente_recebe.c - o cliente_recebe
Na pasta servidor:
  - Compilar - gcc server.c - o server

Isso irá criar arquivos .exe nas pastas deles, pode abrir com o comando ./nomeArquivo, por exemplo:
  - Executar - ./cliente_envia 
  - Executar - ./cliente_recebe 
  - Executar - ./server

  Lembre-se de estar no terminal da pasta designada. 

Trabalho de implementação da transferência de arquivos entre sistemas finais remotos na linguagem C.
Implementado para Linux.

Para o desenvolvimento deste trabalho foi preciso implementar:
  - Soma de Verificação;
  - Número de Sequencia;
  - Temporizador;
  - Pacote de Reconhecimento.



