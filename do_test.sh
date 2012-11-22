make -j12
#VAL="valgrind --leak-check=full --log-fd=1"
#VAL=time
VAL=
${VAL} ./gas/as-new -o test1.o test1.s
${VAL} ./gas/as-new -o test2.o test2.s
echo ===== DUMP =====
${VAL} ./binutils/objdump -S test1.o
${VAL} ./binutils/objdump -r test1.o
${VAL} ./binutils/objdump -S test2.o
${VAL} ./binutils/objdump -r test2.o
echo ===== LD =====
${VAL} ./ld/ld-new -o testx test1.o test2.o
${VAL} ./binutils/objdump -S testx
