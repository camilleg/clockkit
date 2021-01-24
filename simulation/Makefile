SIM=/tmp/plot.dat
OUT=/tmp/plot.eps
all:
	./simulation.rb > $(SIM)
	gnuplot -c gnuplot.txt $(SIM) $(OUT)
