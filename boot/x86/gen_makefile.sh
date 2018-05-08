#!/bin/bash
echo "old Makefile still alive : .Makefile.reg";
mv Makefile .Makefile.reg
echo "Generating new Makefile";
echo "OBJS += \\" >> Makefile;
for f in **/*;
do
	echo "$f \\" | rev | sed s/msa/o/ | rev >> Makefile;
done
echo "new Makefile generated";
