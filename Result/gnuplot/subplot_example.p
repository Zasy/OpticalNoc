set terminal push
set terminal png

set output "fig3.png"

set size 1, 1
set multiplot layout 2, 2
set size 0.5, 0.5

set origin 0, 0
plot sin(x)

set origin 0, 0.5
plot cos(x)

set origin 0.5, 0
plot x**2

set origin 0.5, 0.5
plot x**3

unset multiplot

set output
set term pop