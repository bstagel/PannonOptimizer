#!/bin/bash

FILELIST='..\filelist_Ratiotest_study.txt'

echo Ratiotest study started!

cd release
./NewPanOptSolver.exe -p
cd ..

for SET in {100..101}
do
	echo "Ratiotest test $SET begins."
	rm release/simplex.PAR
	cp Ratiotest_study/simplex_$SET.PAR release/simplex.PAR
	cd release
	./NewPanOptSolver.exe -fl $FILELIST -o result_$SET.txt 
	cd ..
	echo "Ratiotest test $SET ended."
done

echo Ratiotest study ended!