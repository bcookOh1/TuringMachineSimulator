// BinaryAddition.tm
variant: standard
description: binary addition
start: q0
tape_symbols: {B,#,X,Y,C,+,=}
alphabet: {0,1}
input_example: 101+111
transition_function_table_begin:
delta |   B     |   0     |   1     |   +     |   =     |   X     |   Y     |   C     |   U     |   V     |   #     
q0    | q2,B,R  |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -      // start
q1    |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -    
q2    | q3,=,R  | q2,0,R  | q2,1,R  | q2,+,R  |   -     |   -     |   -     |   -     |   -     |   -     |   -      // add = to end 
q3    | q4,#,L  |    -    |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -      // add # to end 
q4    | q5,B,R  | q4,0,L  | q4,1,L  | q4,+,L  | q4,=,L  |   -     |   -     |   -     | q5,U,R  | q5,V,R  | q4,#,L   // back to left B,U,or V
q5    |   -     | q6,U,R  | q6,V,R  | q7,+,L  |   -     |   -     |   -     |   -     | q5,U,R  | q5,V,R  |   -      // find next 0/1 or done 
q6    | q4,#,L  | q6,0,R  | q6,1,R  | q6,+,R  | q6,=,R  |   -     |   -     |   -     | q6,U,R  | q6,V,R  | q6,#,R   // add # to end 
q7    | q9,B,R  |   -     |   -     |   -     |   -     |   -     |   -     |   -     | q7,0,L  | q7,1,L  |   -      // replace U/V with 0/1
q8    |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -    
q9    |   -     | q9,0,R  | q9,1,R  | q10,+,L |   -     |  q9,X,R |   -     |   -     |   -     |   -     |   -      // goto +
q10   | q27,B,R | q11,X,R | q21,X,R |   -     |   -     | q10,X,L |   -     |   -     |   -     |   -     |   -      // get 0/1 in 1st num
q11   |   -     | q11,0,R | q11,1,R | q11,+,R | q12,=,L | q11,X,R | q11,Y,R |   -     |   -     |   -     |   -      // goto =
q12   |   -     | q13,Y,R | q17,Y,R |   -     |   -     |   -     | q12,Y,L |   -     |   -     |   -     |   -      // get 0/1 in 2nd num
q13   | q14,B,L | q13,0,R | q13,1,R |   -     | q13,=,R |   -     | q13,Y,R | q13,C,R |   -     |   -     | q13,#,R  // goto right B
q14   |   -     | q14,0,L | q14,1,L |   -     |    -    |   -     |   -     | q15,1,L |   -     |   -     | q15,0,L  // set 0 in rightmost #
q15   | q9,B,R  | q15,0,L | q15,1,L | q15,+,L | q15,=,L | q15,X,L | q15,Y,L | q15,C,L |   -     |   -     | q15,#,L  // goto left B
q16   |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -    
q17   | q18,B,L | q17,0,R | q17,1,R | q17,+,R | q17,=,R | q17,X,R | q17,Y,R | q17,C,R |   -     |   -     | q17,#,R  // goto right B
q18   |   -     | q18,0,L | q18,1,L |   -     |   -     |   -     |   -     | q25,0,L |   -     |   -     | q15,1,L  // set 1 in rightmost #
q19   |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -       
q20   |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -    
q21   |   -     | q21,0,R | q21,1,R | q21,+,R | q22,=,L | q21,X,R | q21,Y,R |   -     |   -     |   -     |   -      // goto =
q22   |   -     | q17,Y,R | q23,Y,R |   -     |   -     |   -     | q22,Y,L |   -     |   -     |   -     |   -      // get 0/1 in 2nd num 
q23   | q24,B,L | q23,0,R | q23,1,R | q23,+,R | q23,=,R | q23,X,R | q23,Y,R | q23,C,R |   -     |   -     | q23,#,R  // goto right B
q24   |   -     | q24,0,L | q24,1,L |   -     |   -     |   -     |   -     | q25,1,L |   -     |   -     | q25,0,L  // set 0 in rightmost # or 1 in C
q25   |   -     |   -     |   -     |   -     | q15,=,L |   -     |   -     |   -     |   -     |   -     | q15,C,L  // set the carry was C
q26   |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |    -
q27   | q28,B,L | q27,0,R | q27,1,R | q27,+,R | q27,=,R | q27,X,R | q27,Y,R | q27,1,R |   -     |   -     | q27,0,R  // fix C or #
q28   |   -     | q28,0,L | q28,1,L | q28,+,L | q28,=,L | q28,X,L | q28,Y,L | q28,1,L |   -     |   -     | q28,0,L  // goto left B and halt
q29   |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -     |   -    
transition_function_table_end:

