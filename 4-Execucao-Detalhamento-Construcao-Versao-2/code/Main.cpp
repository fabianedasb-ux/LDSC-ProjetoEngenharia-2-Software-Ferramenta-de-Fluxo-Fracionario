/**
 * @file main.cpp
 * @brief Ponto de entrada (Entry Point) da aplicação do Simulador de Buckley-Leverett.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "src/View/Mainwindow.h"
#include <QApplication>

/**
 * @brief Função principal que inicializa o framework Qt e a interface gráfica.
 * * @param argc Número de argumentos de linha de comando.
 * @param argv Vetor de matriz de caracteres (argumentos).
 * @return Código de saída do laço de eventos do Qt (0 indica encerramento seguro).
 */
int main(int argc, char *argv[])
{
    // 1. Inicializa o gerenciador de recursos, fontes e eventos do Qt
    QApplication a(argc, argv);

    // 2. Instancia a janela principal da Camada de Apresentação
    MainWindow w;

    // 3. Exibe a janela ocupando a tela inteira (maximizada) por padrão para melhor ergonomia
    w.showMaximized();

    // 4. Transfere o controle de execução para o laço de eventos (Event Loop) do SO
    return a.exec();
}