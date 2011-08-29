#!/bin/bash

make && ./main | awk 'BEGIN {print "plot \"-\" with lines;\n"} {print $1} END{print "e\npause mouse keypress  \"Hit return to continue\""}' | gnuplot
