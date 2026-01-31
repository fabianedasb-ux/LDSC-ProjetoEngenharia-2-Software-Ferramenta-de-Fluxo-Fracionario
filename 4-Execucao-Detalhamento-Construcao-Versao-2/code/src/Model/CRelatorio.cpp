/**
 * @file CRelatorio.cpp
 * @brief Implementação da classe de geração de relatórios com diagnósticos físicos.
 */

#include "CRelatorio.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <QString>
#include <QTextDocument>
#include <QPdfWriter>
#include <QDate>

CRelatorio::CRelatorio() {
    limpar();
}

CRelatorio::~CRelatorio() {}

void CRelatorio::limpar() {
    _textoRelatorio = "<html><head><style>";
    _textoRelatorio += "body { font-family: 'Segoe UI', Arial, sans-serif; color: #333; }";
    _textoRelatorio += "h1 { color: #2E86C1; text-align: center; }";
    _textoRelatorio += "h2 { color: #1F618D; border-bottom: 2px solid #2E86C1; padding-bottom: 5px; margin-top: 20px; }";
    _textoRelatorio += "table { width: 100%; border-collapse: collapse; margin-top: 10px; }";
    _textoRelatorio += "th, td { border: 1px solid #ddd; padding: 10px; text-align: left; }";
    _textoRelatorio += "th { background-color: #EBF5FB; color: #1B4F72; }";
    _textoRelatorio += ".warning { color: #A93226; font-weight: bold; background-color: #FDEDEC; padding: 10px; border: 1px solid #A93226; }";
    _textoRelatorio += ".success { color: #1D8348; font-weight: bold; }";
    _textoRelatorio += "</style></head><body>";

    _textoRelatorio += "<h1>Relatório de Simulação de Fluxo Bifásico</h1>";
    _textoRelatorio += "<p align='center'><b>Software Educacional Buckley-Leverett - Solução Analítica</b></p>";
    _textoRelatorio += "<hr>";
}

// 1. Dados de Entrada (Atualizado com unidades SI e Campo)
void CRelatorio::gerarCabecalho(double comp, double area, double phi, double k, double angulo, double mio, double miw) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);

    ss << "<h2>1. Parâmetros do Sistema</h2>";
    ss << "<table>";
    ss << "<tr><th>Propriedade</th><th>Valor</th><th>Unidade</th></tr>";
    ss << "<tr><td>Comprimento do Reservatório (L)</td><td>" << comp << "</td><td>m</td></tr>";
    ss << "<tr><td>Área da Seção Transversal (A)</td><td>" << area << "</td><td>m²</td></tr>";
    ss << "<tr><td>Porosidade (&phi;)</td><td>" << phi * 100.0 << "</td><td>%</td></tr>";
    ss << "<tr><td>Permeabilidade Absoluta (k)</td><td>" << k << "</td><td>mD</td></tr>";
    ss << "<tr><td>Inclinação (&alpha;)</td><td>" << angulo << "</td><td>graus</td></tr>";
    ss << "<tr><td>Viscosidade da Água (&mu;<sub>w</sub>)</td><td>" << miw << "</td><td>cP</td></tr>";
    ss << "<tr><td>Viscosidade do Óleo (&mu;<sub>o</sub>)</td><td>" << mio << "</td><td>cP</td></tr>";
    ss << "</table>";

    _textoRelatorio += ss.str();
}

// 2. NOVO: Diagnóstico de Engenharia (Rapoport-Leas e Adimensionais)
void CRelatorio::registrarDiagnosticoFisico(double nrl, double m, double ng) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);

    ss << "<h2>2. Diagnóstico de Forças Adimensionais</h2>";

    // Alerta de Rapoport-Leas
    if (nrl < 3.0) {
        ss << "<p class='warning'>AVISO: Número de Rapoport-Leas (" << nrl << ") abaixo do limite crítico (3.0). "
           << "A pressão capilar pode causar dispersão significativa não considerada neste modelo analítico.</p>";
    } else {
        ss << "<p class='success'>Critério de Rapoport-Leas atingido (" << nrl << " &ge; 3). "
           << "Efeitos capilares desprezíveis (Buckley-Leverett válido).</p>";
    }

    ss << "<table>";
    ss << "<tr><th>Indicador Adimensional</th><th>Valor</th><th>Descrição</th></tr>";
    ss << "<tr><td>Razão de Mobilidade (M)</td><td>" << m << "</td><td>Forças Viscosas</td></tr>";
    ss << "<tr><td>Número de Gravidade (N<sub>g</sub>)</td><td>" << ng << "</td><td>Efeito da Inclinação/Densidade</td></tr>";
    ss << "</table>";

    _textoRelatorio += ss.str();
}

// 3. Resultados de Welge (Rankine-Hugoniot)
void CRelatorio::registrarResultadoWelge(double swFrente, double swMedia, double tempoRuptura) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);

    ss << "<h2>3. Solução de Buckley-Leverett (Choque)</h2>";
    ss << "<table>";
    ss << "<tr><th>Variável de Saída</th><th>Valor Calculado</th><th>Unidade</th></tr>";
    ss << "<tr><td>Saturação na Frente de Choque (S<sub>wf</sub>)</td><td>" << swFrente << "</td><td>adimensional</td></tr>";
    ss << "<tr><td>Saturação Média (S<sub>w,med</sub>)</td><td>" << swMedia << "</td><td>adimensional</td></tr>";
    ss << "<tr><td>Tempo de Breakthrough (t<sub>D,bt</sub>)</td><td>" << tempoRuptura << "</td><td>PVI</td></tr>";
    ss << "</table>";

    _textoRelatorio += ss.str();
}

void CRelatorio::registrarEficiencia(double tempo, double fatorRecuperacao) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);

    ss << "<h2>4. Performance da Injeção</h2>";
    ss << "<p><b>Tempo Atual de Simulação:</b> " << tempo << " PVI</p>";
    ss << "<p><b>Fator de Recuperação (FR):</b> <span class='success'>" << (fatorRecuperacao * 100.0) << " %</span></p>";

    ss << "<br><hr><p style='font-size:small; color:gray; text-align:right;'>Gerado em: "
       << QDate::currentDate().toString("dd/MM/yyyy").toStdString() << "</p>";

    _textoRelatorio += ss.str();
}

bool CRelatorio::exportarParaPDF(const std::string& caminho) {
    std::string htmlFinal = _textoRelatorio + "</body></html>";
    QTextDocument documento;
    documento.setHtml(QString::fromStdString(htmlFinal));

    QPdfWriter printer(QString::fromStdString(caminho));
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(20, 20, 20, 20));

    documento.print(&printer);
    return true;
}
