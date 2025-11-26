#include "Simulador.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // Verifica se o usuário passou o nome do arquivo de entrada
    if (argc < 2) {
        std::cerr << "Erro: Por favor, forneça o nome do arquivo de entrada.\n";
        std::cerr << "Uso: ./fw_calc <caminho_para_arquivo_de_entrada>\n";
        return 1; // Retorna um código de erro
    }

    std::string arquivoEntrada = argv[1];

    try {
        Simulador sim;
        sim.executar(arquivoEntrada);
    } catch (const std::exception& e) {
        std::cerr << "Uma exceção ocorreu: " << e.what() << '\n';
        return 1;
    }

    return 0; // Sucesso
}
