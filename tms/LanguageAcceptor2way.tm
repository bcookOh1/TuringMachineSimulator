// LanguageAcceptor2way.tm
variant: two_way
description: language acceptor Sudkamp p266
start: qs
final: {q1,q2,q3,q4,q5}
tape_symbols: {B,#}
alphabet: {a,b}
input_example: aaaabba
transition_function_table_begin:
delta |   B    |   a    |   b    |   #   
qs    | qt,B,L |   -    |   -    |   -   
qt    | q0,#,R |   -    |   -    |   -   
q0    | q1,B,R |   -    |   -    | qe,#,R
q1    |   -    | q1,a,R | q2,b,L | qe,#,R
q2    | q3,B,L | q3,a,L |   -    | qe,#,R
q3    | q4,B,L | q4,a,L |   -    | qe,#,R
q4    | q5,B,L | q5,a,L |   -    | qe,#,R
q5    |   -    |   -    |   -    | qe,#,R
qe    |   -    |   -    |   -    |   - 
transition_function_table_end:

