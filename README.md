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
and then into:  
13.  
.2.  
..1  

(Note: only the sum of the 8-neighbors at the time of placing matters.)

Game ends when no more stones can be placed in accordance with the rules. The objective is to maximize the highest number present on the board (in other words, place the maximum number of numbered stones given n brown stones).

## OEIS

The sequence https://oeis.org/A337663 tracks the maximum value of stones that can be placed for the n-stone game. Each subsequent term takes ~200x longer to compute. Only up to a(6) is known as of 19 Jan 2022.

## The code

Approach based on *Hugo van der Sanden*'s approach at https://codegolf.stackexchange.com/questions/212160/extend-the-most-recent-nice-oeis-sequence-stepping-stone-puzzle-on-a-grid/212164#212164