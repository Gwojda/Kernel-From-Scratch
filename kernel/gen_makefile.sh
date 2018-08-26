#!/bin/bash
echo "old Makefile still alive : .Makefile.reg";
mv Makefile .Makefile.reg
echo "Generating new Makefile";
echo "OBJS += \\" >> Makefile;
for f in `find . -name "*.c"`;
do
	echo "kernel/`echo "$f" | cut -c 3-` \\" | rev | sed s/c/o/ | rev >> Makefile;
done
for f in  `find . -name "*.asm"`;
do
	echo "kernel/`echo "$f" | cut -c 3-` \\" | rev | sed s/msa/o/ | rev >> Makefile;
done

echo "new Makefile generated";
