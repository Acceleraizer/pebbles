# Pebbles

Stones on an Infinite Chessboard - Numberphile:
https://www.youtube.com/watch?v=m4Uth-EaTZ8

## Rules of the game

We have an infinite chessboard, and each cell can hold 0 or 1 stones.
There are two types of stones:  
1) Brown stones - with value 1  
2) Numbered stones 2, 3, 4, ... with value equal to its number

For the n-stone game, start off with n brown stones. The n-stone game proceeds as follows:  
1) Place the n brown stones on the board.  
2) Place the numbered stones in an empty cell, subject to the following rule: The neighboring 8 cells must sum to the value of the stone

So,  
1..  
...  
..1  
can be turned into:  
1..  
.2.  
..1  

## OEIS

The sequence https://oeis.org/A337663 tracks the maximum value of stones that can be placed for the n-stone game. 