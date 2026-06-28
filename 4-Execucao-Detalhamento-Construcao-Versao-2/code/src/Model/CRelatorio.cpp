/**
 * @file CRelatorio.cpp
 * @brief Implementação aprimorada da geração de relatórios com embutimento de gráficos.
 * @author Fabiane da Silva Barros
 * @date Janeiro 2026
 */

#include "CRelatorio.h"
#include <sstream>
#include <iomanip>
#include <QTextDocument>
#include <QPdfWriter>
#include <QPageSize>
#include <QBuffer>
#include <QByteArray>
#include <QPageLayout>

// --- Construtor e Destrutor ---
CRelatorio::CRelatorio() {
    limpar();
}

CRelatorio::~CRelatorio() {}

// --- Estruturação do HTML e CSS ---
void CRelatorio::limpar() {
    _textoRelatorio = "<html><head><style>";
    _textoRelatorio += "body { font-family: 'Segoe UI', Arial, sans-serif; color: #2c3e50; line-height: 1.4; padding: 10px; }";
    _textoRelatorio += "h1 { color: #1a5276; text-align: center; font-size: 22pt; margin-bottom: 5px; }";
    _textoRelatorio += "h2 { color: #2e86c1; font-size: 14pt; border-bottom: 2px solid #5dade2; padding-bottom: 4px; margin-top: 25px; margin-bottom: 10px; }";
    _textoRelatorio += "h3 { color: #34495e; font-size: 10pt; margin-top: 5px; margin-bottom: 25px; text-align: center; font-style: italic; }";
    _textoRelatorio += "table { width: 100%; border-collapse: collapse; margin-top: 5px; margin-bottom: 15px; }";
    _textoRelatorio += "th, td { border: 1px solid #b3b6b7; padding: 8px 10px; font-size: 10pt; text-align: left; }";
    _textoRelatorio += "th { background-color: #ebf5fb; color: #1b4f72; font-weight: bold; }";
    _textoRelatorio += ".card-highlight { background-color: #f4f6f7; border-left: 5px solid #2e86c1; padding: 12px; margin-vertical: 15px; }";
    _textoRelatorio += ".warning-box { color: #922b21; background-color: #fadbd8; border: 1px solid #78281f; padding: 10px; font-size: 10pt; font-weight: bold; margin-bottom: 15px; }";
    _textoRelatorio += ".success-box { color: #196f3d; background-color: #d4efdf; border: 1px solid #145a32; padding: 10px; font-size: 10pt; font-weight: bold; margin-bottom: 15px; }";
    _textoRelatorio += ".metric-value { font-family: 'Courier New', monospace; font-weight: bold; color: #1b4f72; }";
    _textoRelatorio += ".center-plot { text-align: center; margin-top: 10px; margin-bottom: 15px; }";
    _textoRelatorio += "</style></head><body>";

    _textoRelatorio += "<h1>Relatório Técnico de Desempenho de Injeção</h1>";
    _textoRelatorio += "<p align='center' style='font-size:11pt; color:#7f8c8d;'><b>Simulador Analítico de Buckley-Leverett — Método das Características</b></p>";
    _textoRelatorio += "<hr size='2' color='#1a5276'>";
}

// --- Geração de Tabelas Analíticas ---
void CRelatorio::gerarCabecalho(double comp, double area, double phi, double k, double angulo, double mio, double miw) {
    std::stringstream ss;

    ss << "<h2>1. Parâmetros Estáticos do Meio Poroso e Fluidos</h2>";
    ss << "<table>";
    ss << "<tr><th width='60%'>Propriedade Operacional / Geométrica</th><th width='20%'>Valor</th><th width='20%'>Unidade</th></tr>";

    // Formatação Padrão (2 casas decimais)
    ss << std::fixed << std::setprecision(2);
    ss << "<tr><td>Comprimento Geométrico do Reservatório (L)</td><td class='metric-value'>" << comp << "</td><td>m</td></tr>";
    ss << "<tr><td>Área da Seção Transversal Hidráulica (A)</td><td class='metric-value'>" << area << "</td><td>m&sup2;</td></tr>";
    ss << "<tr><td>Porosidade (&Phi;)</td><td class='metric-value'>" << phi * 100.0 << "</td><td>%</td></tr>";

    // Formatação Científica (Mantém precisão para valores muito pequenos/grandes)
    ss << std::scientific << std::setprecision(2);
    ss << "<tr><td>Permeabilidade Absoluta da Rocha (k)</td><td class='metric-value'>" << k << "</td><td>m&sup2;</td></tr>";

    // Retorno para Formatação Padrão
    ss << std::fixed << std::setprecision(2);
    ss << "<tr><td>Ângulo de Inclinação Estrutural (&alpha;)</td><td class='metric-value'>" << angulo << "</td><td>graus</td></tr>";

    // Formatação de Viscosidade (3 casas decimais obrigatórias para Pa.s)
    ss << std::fixed << std::setprecision(3);
    ss << "<tr><td>Viscosidade Dinâmica da Água (&mu;<sub>w</sub>)</td><td class='metric-value'>" << miw << "</td><td>Pa.s</td></tr>";
    ss << "<tr><td>Viscosidade Dinâmica do Óleo (&mu;<sub>o</sub>)</td><td class='metric-value'>" << mio << "</td><td>Pa.s</td></tr>";

    ss << "</table>";

    _textoRelatorio += ss.str();
}

void CRelatorio::registrarDiagnosticoFisico(double nrl, double m, double ng) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);

    ss << "<h2>2. Diagnóstico de Forças Balísticas e Adimensionais</h2>";

    if (nrl < 3.0) {
        ss << "<div class='warning-box'>AVISO DE REGIME FLUIDODINÂMICO:<br>"
           << "O Número de Rapoport-Leas calculado (" << nrl << ") está abaixo do limite crítico estabelecido pela literatura (3.0). "
           << "Efeitos capilares de fim de curso (end-effect) podem introduzir dispersão no perfil real de saturação.</div>";
    } else {
        ss << "<div class='success-box'>VALIDAÇÃO DE REGIME:<br>"
           << "Critério macroscópico de Rapoport-Leas satisfeito (" << nrl << " &ge; 3.0). "
           << "As forças viscosas dominam o escoamento, garantindo a aplicabilidade estrita da teoria de Buckley-Leverett.</div>";
    }

    ss << "<table>";
    ss << "<tr><th width='40%'>Grandeza Adimensional</th><th width='20%'>Valor</th><th width='40%'>Significado Físico</th></tr>";
    ss << "<tr><td>Razão de Mobilidade Total (M)</td><td class='metric-value'>" << m << "</td><td>Eficiência do empuxo viscoso do fluido injetado</td></tr>";
    ss << "<tr><td>Número de Gravidade (N<sub>g</sub>)</td><td class='metric-value'>" << ng << "</td><td>Relação entre forças gravitacionais e forças viscosas</td></tr>";
    ss << "</table>";

    _textoRelatorio += ss.str();
}

void CRelatorio::registrarResultadoWelge(double swFrente, double swMedia, double tempoRuptura, double edBt) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);

    ss << "<h2>3. Indicadores Críticos do Instante de Ruptura (Breakthrough)</h2>";
    ss << "<div class='card-highlight'>";
    ss << "Valores analíticos extraídos via Condição de Entropia de Oleinik e Tangente de Welge:";

    ss << "<table>";
    ss << "<tr><th width='60%'>Parâmetro de Desempenho (Frente de Choque)</th><th width='20%'>Valor</th><th width='20%'>Unidade</th></tr>";
    ss << "<tr><td>Saturação de Água na Frente de Choque (S<sub>wf</sub>)</td><td class='metric-value'>" << swFrente << "</td><td>adimensional</td></tr>";
    ss << "<tr><td>Saturação Média de Água na Ruptura (<span style='text-decoration: overline;'>S</span><sub>w,bt</sub>)</td><td class='metric-value'>" << swMedia << "</td><td>adimensional</td></tr>";
    ss << "<tr><td>Tempo Adimensional de Ruptura (t<sub>D,bt</sub>)</td><td class='metric-value'>" << tempoRuptura << "</td><td>PVI</td></tr>";

    ss << std::setprecision(2);
    ss << "<tr><td style='font-weight: bold; color: #1a5276;'>Eficiência de Deslocamento no BT (E<sub>d,bt</sub>)</td><td class='metric-value' style='color:#196f3d; font-size:11pt;'>" << edBt * 100.0 << "</td><td style='font-weight: bold; color: #196f3d;'>%</td></tr>";
    ss << "</table>";
    ss << "</div>";

    _textoRelatorio += ss.str();
}

void CRelatorio::registrarEficiencia(double tempo, double fatorRecuperacao) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);

    ss << "<h2>4. Status da Janela de Injeção Computada</h2>";
    ss << "<table>";
    ss << "<tr><th width='60%'>Parâmetro Dinâmico Atual</th><th width='20%'>Valor Computado</th><th width='20%'>Unidade</th></tr>";
    ss << "<tr><td>Volume Acumulado Injetado (PVI)</td><td class='metric-value'>" << tempo << "</td><td>Volumes Porosos</td></tr>";
    ss << "<tr><td>Eficiência de Deslocamento Atual (E<sub>d</sub>)</td><td class='metric-value' style='color:#1a5276;'>" << (fatorRecuperacao * 100.0) << "</td><td>% (Óleo Original)</td></tr>";
    ss << "</table>";

    _textoRelatorio += ss.str();
}

// --- Processamento de Imagens e Exportação I/O ---
void CRelatorio::registrarGraficos(const QPixmap& pixFluxo, const QPixmap& pixSaturacao, const QPixmap& pixEficiencia) {
    _textoRelatorio += "<h2>5. Curvas e Perfis Analíticos Gerados</h2>";

    // Expressão Lambda para conversão Binária -> Base64
    auto pixmapToBase64Html = [](const QPixmap& pix) -> std::string {
        if (pix.isNull()) return "";
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        pix.save(&buffer, "PNG");
        return bytes.toBase64().toStdString();
    };

    std::string b64Fluxo = pixmapToBase64Html(pixFluxo);
    std::string b64Sat = pixmapToBase64Html(pixSaturacao);
    std::string b64Ef = pixmapToBase64Html(pixEficiencia);

    _textoRelatorio += "<table width='100%' border='0' style='border:none;'>";

    if (!b64Fluxo.empty()) {
        _textoRelatorio += "<tr><td style='border:none;' class='center-plot'>";
        _textoRelatorio += "<img src='data:image/png;base64," + b64Fluxo + "' width='500'/><br>";
        _textoRelatorio += "<h3>Figura 5.1: Curva de Fluxo Fracionário (fw) e Reta Tangente de Welge</h3>";
        _textoRelatorio += "</td></tr>";
    }

    if (!b64Sat.empty()) {
        _textoRelatorio += "<tr><td style='border:none;' class='center-plot'>";
        _textoRelatorio += "<img src='data:image/png;base64," + b64Sat + "' width='500'/><br>";
        _textoRelatorio += "<h3>Figura 5.2: Perfil Espacial de Saturação de Água (Sw vs xD)</h3>";
        _textoRelatorio += "</td></tr>";
    }

    if (!b64Ef.empty()) {
        _textoRelatorio += "<tr><td style='border:none;' class='center-plot'>";
        _textoRelatorio += "<img src='data:image/png;base64," + b64Ef + "' width='500'/><br>";
        _textoRelatorio += "<h3>Figura 5.3: Histórico de Eficiência de Deslocamento Microscópico (Ed vs PVI)</h3>";
        _textoRelatorio += "</td></tr>";
    }

    _textoRelatorio += "</table>";
}

void CRelatorio::adicionarNota(const std::string& notaHtml) {
    _textoRelatorio += "<h2>6. Notas e Observações Adicionais</h2>";
    _textoRelatorio += "<p>" + notaHtml + "</p>";
}

bool CRelatorio::exportarParaPDF(const std::string& caminho) {
    std::string htmlFinal = _textoRelatorio + "</body></html>";

    QTextDocument documento;
    documento.setHtml(QString::fromStdString(htmlFinal));

    QPdfWriter printer(QString::fromStdString(caminho));
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    documento.print(&printer);
    return true;
}