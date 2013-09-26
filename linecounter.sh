#!/bin/bash

newpanoptlines=`cat include/*.h include/*/*.h include/*/*/*.h src/*.cpp src/*/*.cpp src/*/*/*.cpp | wc -l`
newpanopttesterlines=`cat test/*.cpp test/*.h test/framework/*.cpp test/framework/*.h | wc -l`
totallines=`expr $newpanoptlines + $newpanopttesterlines`

echo "NewPanOpt lines: $newpanoptlines" 
echo "NewPanOptTester lines: $newpanopttesterlines"
echo "Total line count is: $totallines" 