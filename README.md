
# Table of Contents

1.  [Ferramenta de Fluxo Fracionário.](#org1eaa9d2)
    1.  [Projeto de Engenharia de Software desenvolvido para a disciplina de Programação Prática. O software calcula e plota curvas de fluxo fracionário ($f_w$) considerando modelos de permeabilidade relativa.](#org2ded969)
    2.  [https://github.com/fabianedasb-ux/LDSC-ProjetoEngenharia-2-Software-Ferramenta-de-Fluxo-Fracionario](#org573b69e)
2.  [Nome do Software:](#orgff96ba7)
3.  [Resumo e/ou informação extra:](#orge5d5471)
4.  [Pré-requisitos](#orge328ba8)
    1.  [Instalação](#org1ff8da3)
    2.  [Compilação](#orgd11d994)
    3.  [Execução](#orgb3036db)
5.  [Versão:](#orgdad78bf)
6.  [Data:](#org2ee532a)
7.  [Autor de Contato:](#org40e9791)
8.  [Lista dos Autores:](#org7f96839)
9.  [Áreas de pesquisa vinculadas:](#orgb22f11d)
10. [Vinculo com:](#org2765e78)
11. [Paradigmas:](#orgfa5c6d6)
12. [Tipo de Interface:](#orgaf09628)
13. [Plataformas Suportadas:](#org134d802)
14. [Linguagens Utilizadas:](#org42a807a)
15. [Bibliotecas Utilizadas (dependências):](#org83f4195)
16. [Softwares utilizados (dependências)](#org0e8f5e0)
17. [Tipo de Licença:](#org7f482bb)
18. [Novas Versões](#orgd7fb28f)
19. [Links:](#orgf215e55)
20. [Vinculo com:](#org198f964)
21. [Todo](#orgccb7d11)



<a id="org1eaa9d2"></a>

# Ferramenta de Fluxo Fracionário.


<a id="org2ded969"></a>

## Projeto de Engenharia de Software desenvolvido para a disciplina de Programação Prática. O software calcula e plota curvas de fluxo fracionário ($f_w$) considerando modelos de permeabilidade relativa.


<a id="org573b69e"></a>

## <https://github.com/fabianedasb-ux/LDSC-ProjetoEngenharia-2-Software-Ferramenta-de-Fluxo-Fracionario>


<a id="orgff96ba7"></a>

# Nome do Software:

-   Ferramenta de Fluxo Fracionário
-   SOFTWARE EDUCACIONAL COM INTERFACE INTERATIVA PARA IMPLEMENTAÇÃO COMPUTACIONAL DO MÉTODO DAS CARACTERÍSTICAS - SOLUÇÃO ANALÍTICA DE BUCKLEY-LEVERETT


<a id="orge5d5471"></a>

# Resumo e/ou informação extra:

-   Funcionalidades:
    -   Cálculo da curva de fluxo fracionário (Buckley-Leverett).
    -   Suporte a ****Modelo Tabelado**** (interpolação linear).
    -   Suporte a ****Modelo de Corey**** (correlação analítica).
    -   Geração automática de gráficos via ****Gnuplot****.
    -   Validação com dados da literatura (Tarek Ahmed).


<a id="orge328ba8"></a>

# Pré-requisitos

-   Compilador C++ (g++)
-   GNU Make
-   Gnuplot (versão 1)
-   Qt e QCustomPlot  (versão 2)


<a id="org1ff8da3"></a>

## Instalação

Baixar e descompactar o arquivo:
LDSC-ProjetoEngenharia-2-Software-Ferramenta-de-Fluxo-Fracionario/4-Execucao-Detalhamento-Construcao-Versao-2/code/Instalador<sub>Simulador.zip</sub>


<a id="orgd11d994"></a>

## Compilação

No terminal, execute:
make


<a id="orgb3036db"></a>

## Execução

Windows rode o arquivo:
Simulador.exe

Para rodar a validação com dados de Tarek Ahmed:
./bin/fw<sub>calctest</sub>/Teste-Tarek-1cp.in


<a id="orgdad78bf"></a>

# Versão:

-   2.3.


<a id="org2ee532a"></a>

# Data:

-   2026/8 - Agosto de 2026.


<a id="org40e9791"></a>

# Autor de Contato:

-   Fabiane da Silva Barros <fabianedas.b@gmail.com>
-   André Duarte Bueno <andreduartebueno@gmail.com> [orientador]


<a id="org7f96839"></a>

# Lista dos Autores:

-   Fabiane da Silva Barros <fabianedas.b@gmail.com>
-   André Duarte Bueno <andreduartebueno@gmail.com> <bueno@lenep.uenf.br> [orientador]


<a id="orgb22f11d"></a>

# Áreas de pesquisa vinculadas:

-   Modelagem Matemática Computacional
-   Engenharia de Reservatório
-   Petrofísica


<a id="org2765e78"></a>

# Vinculo com:

-   [Trabalho da disciplina Projeto de Software Aplicado à Engenharia](https://sites.google.com/view/professorandreduartebueno/ensino/projeto-de-software-aplicado).
-   A metodologia utilizada é descrita [aqui](https://github.com/ldsc/LDSC-ProjetoEngenharia-0-Metodologia-Instrucoes-Etapas).
-   Uma lista de outros projetos desenvolvidos no LDSC é disponibilizado [aqui](https://github.com/ldsc/LDSC-ProjetoEngenharia-0-Metodologia-Instrucoes-Etapas).


<a id="orgfa5c6d6"></a>

# Paradigmas:

-   POO - Programação Orientada a Objeto (códigos em C++)


<a id="orgaf09628"></a>

# Tipo de Interface:

-   Terminal e gráfica (gnuplot) - versão 1.
-   Gráfica (Qt, QCustomPlot) - versão 2.


<a id="org134d802"></a>

# Plataformas Suportadas:

-   Windows e GNU/Linux.


<a id="org42a807a"></a>

# Linguagens Utilizadas:

-   C++.


<a id="org83f4195"></a>

# Bibliotecas Utilizadas (dependências):

-   CGnuplot - <https://github.com/ldsc/CGnuplot>
-   QCustomPlot - <https://www.qcustomplot.com/>


<a id="org0e8f5e0"></a>

# Softwares utilizados (dependências)

-   gnuplot <http://www.gnuplot.info/>


<a id="org7f482bb"></a>

# Tipo de Licença:

-   GPL


<a id="orgd7fb28f"></a>

# Novas Versões

-   Entre em contato com o Prof. André Duarte Bueno <bueno@lenep.uenf.br>
    para que o mesmo gere um novo projeto no github do ldsc para que você (e sua equipe) possam trabalhar na nova versão.
-   Se este projeto tem como número de versão 2.0 a nova versão deverá ser 3.0 e assim sucessivamente.
-   Os nomes dos autores originais deve ser preservada na nova versão, ou seja, adicione a informação do número da versão, os nomes dos membros da nova equipe, mas jamais apague os nomes dos autores originais (isto seria plágio).
-   Na descrição do projeto informe que trata-se de uma nova versão, que tem como base a versão anterior e DESTAQUE COM CLAREZA E OBJETIVIDADE QUAIS AS NOVIDADES DA NOVA VERSÃO.


<a id="orgf215e55"></a>

# Links:

-   [Site do Professor André Duarte Bueno](https://sites.google.com/view/professorandreduartebueno/).
-   [Site do LDSC no github](https://github.com/ldsc).
-   [Site do LENEP](https://uenf.br/cct/lenep).


<a id="org198f964"></a>

# Vinculo com:

-   Em alguns casos a versão 1 foi desenvolvida utilizando a metodologia antiga. Nestes casos é necessário copiar os dados da nova versão para a versão antiga. De forma que as próximas versões devem usar a metodologia descrita [aqui](https://github.com/ldsc/LDSC-ProjetoEngenharia-0-Metodologia-Instrucoes-Etapas).
    Havendo dúvidas consulte o professor.
-   [Trabalho da disciplina Projeto de Software Aplicado à Engenharia](https://sites.google.com/view/professorandreduartebueno/ensino/projeto-de-software-aplicado).
-   Uma lista de outros projetos desenvolvidos no LDSC é disponibilizado [aqui](https://github.com/ldsc/LDSC-ProjetoEngenharia-0-Metodologia-Instrucoes-Etapas).


<a id="orgccb7d11"></a>

# Todo

Projeto de Engenharia de Software desenvolvido para a disciplina de Programação Prática. O software calcula e plota curvas de fluxo fracionário ($f_w$) considerando modelos de permeabilidade relativa.

