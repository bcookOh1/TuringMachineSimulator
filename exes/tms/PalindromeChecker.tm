// PalindromeChecker.tm
variant: standard
description: palindrome checker
start: q0
final: {q10}
tape_symbols: {B,X,Y,Z}
alphabet: {a,b,c}
input_example: bacabbacab
transition_function_table_begin:
delta |   B    |   a    |   b    |   c    |   X    |   Y    |   Z
q0    | q1,B,R |   -    |   -    |   -    |   -    |   -    |   -   
q1    | q9,B,L | q2,X,R | q4,Y,R | q6,Z,R | q9,X,L | q9,Y,L | q9,Z,L 
q2    | q3,B,L | q2,a,R | q2,b,R | q2,c,R | q3,X,L | q3,Y,L | q3,Z,L
q3    |   -    | q8,X,L |   -    |   -    | q9,X,L | q3,Y,L | q3,Z,L
q4    | q5,B,L | q4,a,R | q4,b,R | q4,c,R | q5,X,l | q5,Y,L | q5,Z,L
q5    |   -    |   -    | q8,Y,L |   -    | q5,X,L | q9,Y,L | q5,Z,L
q6    | q7,B,L | q6,a,R | q6,b,R | q6,c,R | q7,X,L | q7,Y,L | q7,Z,L
q7    |   -    |   -    |   -    | q8,Z,L | q7,X,L | q7,Y,L | q9,Z,L
q8    | q1,B,R | q8,a,L | q8,b,L | q8,c,L | q1,X,R | q1,Y,R | q1,Z,R   // move back to left B
q9    | q10,B,L|   -    |   -    |   -    | q9,X,L | q9,Y,L | q9,Z,L   // return to left B
q10   |   -    |   -    |   -    |   -    |   -    |   -    |   -   
transition_function_table_end:

