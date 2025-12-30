#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <string>

/**
 * @class Simulador
 * @brief O orquestrador da aplicação.
 *
 * Esta classe é responsável por controlar o fluxo de execução do programa:
 * ler o arquivo de entrada, instanciar os objetos corretos
 * (fazendo o 'new' dos modelos) e coordenar as chamadas
 * para a calculadora e o plotter.
 */
class Simulador {
public:
    /**
     * @brief Ponto de entrada principal da lógica do simulador.
     * @param arquivoEntrada O caminho (path) para o arquivo de configuração .txt.
     */
    void executar(const std::string& arquivoEntrada);
};

#endif
