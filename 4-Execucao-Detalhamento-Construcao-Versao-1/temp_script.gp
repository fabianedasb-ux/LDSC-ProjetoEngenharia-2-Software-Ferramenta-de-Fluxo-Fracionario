set title 'Curva de Fluxo Fracionario (Buckley-Leverett)'
set xlabel 'Saturacao de Agua (Sw)'
set ylabel 'Fluxo Fracionario de Agua (Fw)'
set grid
set key top left
set datafile separator ','
plot 'temp_data.csv' with lines title 'Curva Fw'
pause -1 'Pressione Enter para fechar'
