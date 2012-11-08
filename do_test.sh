make
VAL=valgrind
VAL=time
${VAL} ./gas/as-new -o test1.o test1.s
${VAL} ./gas/as-new -o test2.o test2.s
${VAL} ./ld/ld-new -o testx test1.o test2.o
${VAL} ./binutils/objdump -S testx
