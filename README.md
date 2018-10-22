# Montador e Ligador
O trabalho consiste em um montador para um assembly hipotético e um ligador de até quatro módulos para a matéria de Software Básico, da Universidade de Brasília.

## Compilar e  Executar
Os arquivos montador.cpp e ligador.cpp encontram-se na pasta src.
Para compilar, basta dar o comando
```
make
```
Também é possível compilar separadamente com
```
make montador
make ligador
```
Para executar, basta passar o nome do arquivo a ser montado/ligado, sem a extensão
```
./montador <nome do arquivo>
./ligador <nome dos aquivos>
```
Tenha em vista que o nome da saída do ligador será o nome do primeiro arquivo passado.

## Autores
+ Gabriel Levi - 16/0006490
+ Léo Moraes - 160011795
