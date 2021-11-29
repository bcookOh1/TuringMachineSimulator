// LanguageAcceptorStd.tm
variant: standard
description: language acceptor example 8.2.2
start: q0
final: {q7}
tape_symbols: {B}
alphabet: {a,b,c}
input_example: aabbcc
transition_function_table_begin:
delta |   B    |   a    |   b    |   c    |   X    |   Y    |   Z
q0    | q1,B,R |   -    |   -    |   -    |   -    |   -    |   -   
q1    | q6,B,R | q2,X,R |   -    |   -    |   -    | q5,Y,R |   -   
q2    |   -    | q2,a,R | q3,Y,R |   -    |   -    | q2,Y,R |   -   
q3    |   -    |   -    | q3,b,R | q4,Z,L |   -    |   -    | q3,Z,R
q4    |   -    | q4,a,L | q4,b,L |   -    | q1,X,R | q4,Y,L | q4,Z,L
q5    | q6,B,L |   -    |   -    |   -    |   -    | q5,Y,R | q5,Z,R
q6    | q7,B,R | q6,a,L | q6,b,L | q6,c,L | q6,X,L | q6,Y,L | q6,Z,L
q7    |   -    |   -    |   -    |   -    |   -    |   -    |   -   
transition_function_table_end:

