#include "Gnuplot.h"
#include <iostream>
#include <fstream>
#include <cstdlib> // Para system()

// TODO: Documentar com JAVADOC/Doxygen
void Gnuplot::plotarCurva(const std::map<double, double>& dados, const std::string& titulo) {
    std::cout << "DEBUG: Chamando Gnuplot...\n";

    // Nomes dos arquivos temporários
    std::string tempDados = "temp_data.csv";
    std::string tempScript = "temp_script.gp";

    // --- 1. Salvar dados no arquivo .csv ---
    std::ofstream arqDados(tempDados);
    if (!arqDados.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar arquivo de dados temporario.\n";
        return;
    }
    arqDados << "# Sw, Fw\n";
    for (const auto& par : dados) {
        arqDados << par.first << ", " << par.second << "\n";
    }
    arqDados.close();

    // --- 2. Criar script do Gnuplot ---
    std::ofstream arqScript(tempScript);
    if (!arqScript.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar script Gnuplot temporario.\n";
        return;
    }
    arqScript << "set title '" << titulo << "'\n";
    arqScript << "set xlabel 'Saturacao de Agua (Sw)'\n";
    arqScript << "set ylabel 'Fluxo Fracionario de Agua (Fw)'\n";
    arqScript << "set grid\n";
    arqScript << "set key top left\n";
    arqScript << "set datafile separator ','\n";
    arqScript << "plot '" << tempDados << "' with lines title 'Curva Fw'\n";
    arqScript << "pause -1 'Pressione Enter para fechar'\n"; // Mantém a janela aberta
    arqScript.close();

    // --- 3. Executar Gnuplot ---
    // Este comando supõe que 'gnuplot' está no PATH do sistema
    std::string comando = "gnuplot " + tempScript;
    std::system(comando.c_str());

    // --- 4. (Opcional) Limpar arquivos temporários ---
    // std::remove(tempDados.c_str());
    // std::remove(tempScript.c_str());
}
