#!/bin/bash

newpanoptlines_src=`find src/ -name '*.cpp' -exec cat '{}' \; | wc -l` 
newpanoptlines_asm=`find src/ -name '*.asm' -exec cat '{}' \; | wc -l` 
newpanoptlines_h=`find include/ -name '*.h' -exec cat '{}' \; | wc -l`
newpanoptlines=$((newpanoptlines_src + newpanoptlines_h + newpanoptlines_asm))

newpanopttesterlines_src=`find test/ -name '*.cpp' -exec cat '{}' \; | wc -l`
newpanopttesterlines_h=`find test/ -name '*.h' -exec cat '{}' \; | wc -l`

newpanopttesterlines=$((newpanopttesterlines_src + newpanopttesterlines_h))
#newpanoptlines=`cat include/*.h include/*/*.h include/*/*/*.h src/*.cpp src/*/*.cpp src/*/*/*.cpp | wc -l`
#newpanopttesterlines=`cat test/*.cpp test/*.h test/framework/*.cpp test/framework/*.h | wc -l`
totallines=`expr $newpanoptlines + $newpanopttesterlines`

echo "NewPanOpt lines: $newpanoptlines" 
echo "NewPanOptTester lines: $newpanopttesterlines"
echo "Total line count is: $totallines" 
