This is a Ruby simulation of the distributed timestamp algorithm
described in the paper "Synchronous data collection from diverse
hardware" by Camille Goudeseune and Braden Kowitz,
available formerly at http://zx81.isl.uiuc.edu/camilleg/dsceu04.pdf,
now ;;;; in this repo.

Type "make" to run the simulation and generate a plot.
The simulation needs Ruby 2.5.
The plotting needs gnuplot 3.7, though you could also use
a spreadsheet's charting tools on the textfile plot.dat.

To alter constants of the simulation, edit simulation.rb.
