#!/bin/bash
echo "old Makefile still alive : .Makefile.reg";
mv Makefile .Makefile.reg
echo "Generating new Makefile";
echo "OBJS += \\" >> Makefile;
for f in **/*;
do
	echo "kernel/$f \\" | rev | sed s/c/o/ | rev >> Makefile;
done
echo "new Makefile generated";
