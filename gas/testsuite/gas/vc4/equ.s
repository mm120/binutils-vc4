 .data
_start:
.equ lab0, .-_start
 .long lab3
.equ lab1, -(_start - .)
 .long lab2
.equ lab2, (.-_start)
 .long lab1
.equ lab3, ~~(.-_start)
 .long lab0
