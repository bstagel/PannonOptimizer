#!/usr/bin/perl -w

$file = shift;
open(BE, $file);
while ($sor=<BE>)
{	
	if (index($sor,"ENTRY") != -1){
		print "$sor";
	}else{
		if (index($sor,"Total iterations") != -1){
				print "$sor";
		}
	}
}