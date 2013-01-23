make -j12
#VAL="valgrind --leak-check=full --log-fd=1"
#VAL=time
VAL=
${VAL} ./gas/as-new -o test1.o test1.s
${VAL} ./gas/as-new -o test2.o test2.s
${VAL} ./gas/as-new -o test5.o test5.s
${VAL} ./gas/as-new -o uart01.o uart01.s
${VAL} ./gas/as-new -o uart02.o uart02.s
./ld/ld-new uart02.o -o uart02_out.o
echo ===== DUMP =====
${VAL} ./binutils/objdump -S test1.o
${VAL} ./binutils/objdump -r test1.o
${VAL} ./binutils/objdump -S test2.o
${VAL} ./binutils/objdump -r test2.o
${VAL} ./binutils/objdump -S test5.o
${VAL} ./binutils/objdump -r test5.o
${VAL} ./binutils/objdump -S uart01.o
${VAL} ./binutils/objdump -r uart01.o
${VAL} ./binutils/objdump -S uart02.o
${VAL} ./binutils/objdump -r uart02.o
${VAL} ./binutils/objdump -S uart02_out.o
${VAL} ./binutils/objdump -r uart02_out.o
echo ===== LD =====
${VAL} ./ld/ld-new -o testx test1.o test2.o
${VAL} ./binutils/objdump -S testx
