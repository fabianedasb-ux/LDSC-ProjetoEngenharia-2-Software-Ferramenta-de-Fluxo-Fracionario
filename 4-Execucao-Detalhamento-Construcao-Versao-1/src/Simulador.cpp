#include "Simulador.h"
#include "CalculadoraFluxoFracionario.h"
#include "CurvasPermeabilidadeTabelada.h"
#include "CurvasPermeabilidadeCorey.h"
#include "Gnuplot.h"

#include <iostream>
#include <fstream>   // Para ler arquivos (ifstream)
#include <sstream>   // Para processar strings (stringstream)
#include <stdexcept> // Para lançar erros (runtime_error)
#include <map>

/**
 * @brief Executa a simulação completa.
 * * Este método orquestra todo o processo:
 * 1. Lê o arquivo de entrada para definir os parâmetros.
 * 2. Instancia o modelo de permeabilidade correto (Tabelado ou Corey).
 * 3. Delega o carregamento de dados detalhados para o modelo.
 * 4. Instancia a calculadora.
 * 5. Gera a curva de fluxo fracionário.
 * 6. Chama o Gnuplot para exibir o resultado.
 * * @param arquivoEntrada O caminho para o arquivo de configuração .txt.
 */
void Simulador::executar(const std::string& arquivoEntrada) {
    std::cout << "Iniciando simulador...\n";

    // --- 1. Leitura e Parsing do Arquivo de Entrada ---

    double mu_o = -1.0;
    double mu_w = -1.0;
    std::string tipoModelo;
    ICurvasPermeabilidade* modelo = nullptr;

    std::ifstream arq(arquivoEntrada);
    if (!arq.is_open()) {
        throw std::runtime_error("Erro fatal: Nao foi possivel abrir o arquivo: " + arquivoEntrada);
    }

    std::cout << "Lendo arquivo de configuracao: " << arquivoEntrada << "\n";
    std::string linha;
    std::string palavraChave;

    while (std::getline(arq, linha)) {
        // Ignora linhas vazias ou comentários
        if (linha.empty() || linha[0] == '#') {
            continue;
        }

        std::stringstream ss(linha);
        ss >> palavraChave;

        if (palavraChave == "VISC_OLEO") {
            ss >> mu_o;
        } else if (palavraChave == "VISC_AGUA") {
            ss >> mu_w;
        } else if (palavraChave == "MODELO_KR") {
            ss >> tipoModelo;
        }
    }

    // --- 2. Validação e Instanciação do Modelo ---

    if (mu_o <= 0 || mu_w <= 0) {
        throw std::runtime_error("Erro: Viscosidades do oleo ou da agua nao definidas no arquivo.");
    }

    if (tipoModelo == "TABELADO") {
        std::cout << "Modelo selecionado: TABELADO\n";
        modelo = new CurvasPermeabilidadeTabelada();
    } else if (tipoModelo == "COREY") {
        std::cout << "Modelo selecionado: COREY\n";
        modelo = new CurvasPermeabilidadeCorey();
    } else {
        throw std::runtime_error("Erro: MODELO_KR nao reconhecido. Use TABELADO ou COREY.");
    }

    // --- 3. Delegar Carregamento de Dados ---
    // O modelo agora lê o *mesmo* arquivo para pegar seus dados específicos
    modelo->carregarDados(arquivoEntrada);

    // --- 4. Criar Calculadora (Injeção de Dependência) ---
    CalculadoraFluxoFracionario calc(mu_o, mu_w, modelo);

    // --- 5. Gerar Curva ---
    std::cout << "Calculando curva...\n";
    std::map<double, double> curva = calc.gerarCurvaCompleta(0.01); // passo de 1%

    // --- 6. Plotar ---
    std::cout << "Plotando resultados...\n";
    Gnuplot::plotarCurva(curva, "Curva de Fluxo Fracionario (Buckley-Leverett)");

    // --- 7. Limpeza da Memória ---
    delete modelo;
    modelo = nullptr;

    std::cout << "Simulacao concluida.\n";
}
