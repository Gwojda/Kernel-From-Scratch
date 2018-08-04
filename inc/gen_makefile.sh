#!/bin/bash
echo "old Makefile still alive : .Makefile.reg";
mv Makefile .Makefile.reg
echo "Generating new Makefile";
echo "HEADERS += \\" >> Makefile;
for f in *.h;
do
	echo "inc/$f \\" >> Makefile;
done
echo "new Makefile generated";
