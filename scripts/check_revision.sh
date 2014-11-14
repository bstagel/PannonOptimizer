echo Revision check started!

./NewPanOptSolver -p
./NewPanOptSolver \
	-Global.debug_level 0 \
	-Global.Export.enable true \
	-Global.Export.filename Revision_check.txt \
	-Global.Export.type REVISION_CHECK \
	-Ratiotest.Expand.divider 1000 \
	-d $1
	
echo Revision check end!