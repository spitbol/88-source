; COMPRESSED SPITBOL ERROR MESSAGES 03/18/00 17:52:11
;

ERRORS	segment	WORD PUBLIC 'DATA'

	PUBLIC	_errors
_errors	DB	0

;    1  "Addition left operand is not numeric"
	DB	166
	DB	31
	DB	0

;    2  "Addition right operand is not numeric"
	DB	166
	DB	185
	DB	0

;    3  "Addition caused integer overflow"
	DB	166
	DB	18
	DB	0

;    4  "Affirmation operand is not numeric"
	DB	"Affirmation "
	DB	3
	DB	8
	DB	0

;    5  "Alternation right operand is not pattern"
	DB	229
	DB	21
	DB	3
	DB	28
	DB	0

;    6  "Alternation left operand is not pattern"
	DB	229
	DB	"left "
	DB	3
	DB	28
	DB	0

;    7  "Compilation error encountered during execution"
	DB	"Compilation "
	DB	150
	DB	" encountered "
	DB	225
	DB	"execution"
	DB	0

;    8  "Concatenation left operand is not a string or pattern"
	DB	206
	DB	"left "
	DB	135
	DB	0

;    9  "Concatenation right operand is not a string or pattern"
	DB	206
	DB	21
	DB	135
	DB	0

;   10  "Negation operand is not numeric"
	DB	"Negation "
	DB	3
	DB	8
	DB	0

;   11  "Negation caused integer overflow"
	DB	"Negation "
	DB	18
	DB	0

;   12  "Division left operand is not numeric"
	DB	161
	DB	31
	DB	0

;   13  "Division right operand is not numeric"
	DB	161
	DB	185
	DB	0

;   14  "Division caused integer overflow"
	DB	161
	DB	18
	DB	0

;   15  "Exponentiation right operand is not numeric"
	DB	23
	DB	185
	DB	0

;   16  "Exponentiation left operand is not numeric"
	DB	23
	DB	31
	DB	0

;   17  "Exponentiation caused integer overflow"
	DB	23
	DB	18
	DB	0

;   18  "Exponentiation result is undefined"
	DB	23
	DB	"result "
	DB	22
	DB	"undefined"
	DB	0

;   19  ""
	DB	0

;   20  "Goto evaluation failure"
	DB	136
	DB	"evaluation failure"
	DB	0

;   21  "Function called by name returned a value"
	DB	165
	DB	"called "
	DB	207
	DB	"a value"
	DB	0

;   22  "Undefined function called"
	DB	183
	DB	13
	DB	"called"
	DB	0

;   23  "Goto operand is not a natural variable"
	DB	136
	DB	3
	DB	"a natural variable"
	DB	0

;   24  "Goto operand in direct goto is not code"
	DB	136
	DB	"operand "
	DB	201
	DB	"direct go"
	DB	217
	DB	22
	DB	20
	DB	"code"
	DB	0

;   25  "Immediate assignment left operand is not pattern"
	DB	"Immediate "
	DB	129
	DB	0

;   26  "Multiplication left operand is not numeric"
	DB	134
	DB	31
	DB	0

;   27  "Multiplication right operand is not numeric"
	DB	134
	DB	185
	DB	0

;   28  "Multiplication caused integer overflow"
	DB	134
	DB	18
	DB	0

;   29  "Undefined operator referenced"
	DB	183
	DB	"operat"
	DB	167
	DB	"referenced"
	DB	0

;   30  "Pattern assignment left operand is not pattern"
	DB	172
	DB	129
	DB	0

;   31  "Pattern replacement right operand is not a string"
	DB	172
	DB	188
	DB	0

;   32  "Subtraction left operand is not numeric"
	DB	138
	DB	31
	DB	0

;   33  "Subtraction right operand is not numeric"
	DB	138
	DB	185
	DB	0

;   34  "Subtraction caused integer overflow"
	DB	138
	DB	18
	DB	0

;   35  "Unexpected failure in -NOFAIL mode"
	DB	239
	DB	"failure "
	DB	201
	DB	"-NOFAIL mode"
	DB	0

;   36  "Goto ABORT with no preceding error"
	DB	136
	DB	"ABORT "
	DB	131
	DB	0

;   37  "Goto CONTINUE with no preceding error"
	DB	251
	DB	131
	DB	0

;   38  "Goto undefined label"
	DB	136
	DB	236
	DB	0

;   39  "External function argument is not a string"
	DB	156
	DB	4
	DB	0

;   40  "External function argument is not integer"
	DB	156
	DB	133
	DB	0

;   41  "FIELD function argument is wrong datatype"
	DB	242
	DB	13
	DB	2
	DB	22
	DB	"wrong datatype"
	DB	0

;   42  "Attempt to change value of protected variable"
	DB	"Attempt "
	DB	217
	DB	"change "
	DB	158
	DB	193
	DB	"protected variable"
	DB	0

;   43  "ANY evaluated argument is not a string"
	DB	"ANY "
	DB	244
	DB	0

;   44  "BREAK evaluated argument is not a string"
	DB	"BREAK "
	DB	244
	DB	0

;   45  "BREAKX evaluated argument is not a string"
	DB	"BREAKX "
	DB	244
	DB	0

;   46  "Expression does not evaluate to pattern"
	DB	234
	DB	184
	DB	0

;   47  "LEN evaluated argument is not integer"
	DB	254
	DB	249
	DB	0

;   48  "LEN evaluated argument is negative or too large"
	DB	254
	DB	250
	DB	0

;   49  "NOTANY evaluated argument is not a string"
	DB	"NOTANY "
	DB	244
	DB	0

;   50  "POS evaluated argument is not integer"
	DB	245
	DB	249
	DB	0

;   51  "POS evaluated argument is negative or too large"
	DB	245
	DB	250
	DB	0

;   52  "RPOS evaluated argument is not integer"
	DB	211
	DB	249
	DB	0

;   53  "RPOS evaluated argument is negative or too large"
	DB	211
	DB	250
	DB	0

;   54  "RTAB evaluated argument is not integer"
	DB	224
	DB	249
	DB	0

;   55  "RTAB evaluated argument is negative or too large"
	DB	224
	DB	250
	DB	0

;   56  "SPAN evaluated argument is not a string"
	DB	"SPAN "
	DB	244
	DB	0

;   57  "TAB evaluated argument is not integer"
	DB	248
	DB	249
	DB	0

;   58  "TAB evaluated argument is negative or too large"
	DB	248
	DB	250
	DB	0

;   59  "ANY argument is not a string or expression"
	DB	"ANY "
	DB	247
	DB	0

;   60  "APPLY first arg is not natural variable name"
	DB	"APPLY "
	DB	7
	DB	143
	DB	0

;   61  "ARBNO argument is not pattern"
	DB	"ARBNO "
	DB	1
	DB	28
	DB	0

;   62  "ARG second argument is not integer"
	DB	"ARG "
	DB	208
	DB	0

;   63  "ARG first argument is not program function name"
	DB	"ARG "
	DB	7
	DB	1
	DB	155
	DB	0

;   64  "ARRAY first argument is not integer or string"
	DB	160
	DB	7
	DB	133
	DB	" "
	DB	167
	DB	179
	DB	0

;   65  "ARRAY first argument lower bound is not integer"
	DB	160
	DB	7
	DB	2
	DB	"lower "
	DB	169
	DB	0

;   66  "ARRAY first argument upper bound is not integer"
	DB	160
	DB	7
	DB	2
	DB	"upper "
	DB	169
	DB	0

;   67  "ARRAY dimension is zero,negative or out of range"
	DB	160
	DB	147
	DB	0

;   68  "ARRAY size exceeds maximum permitted"
	DB	160
	DB	146
	DB	0

;   69  "BREAK argument is not a string or expression"
	DB	"BREAK "
	DB	247
	DB	0

;   70  "BREAKX argument is not a string or expression"
	DB	"BREAKX "
	DB	247
	DB	0

;   71  "CLEAR argument is not a string"
	DB	"CLEAR "
	DB	4
	DB	0

;   72  "CLEAR argument has null variable name"
	DB	"CLEAR "
	DB	240
	DB	0

;   73  "COLLECT argument is not integer"
	DB	"COLLECT "
	DB	133
	DB	0

;   74  "CONVERT second argument is not a string"
	DB	"CONVERT "
	DB	180
	DB	0

;   75  "DATA argument is not a string"
	DB	176
	DB	4
	DB	0

;   76  "DATA argument is null"
	DB	176
	DB	159
	DB	0

;   77  "DATA argument is missing a left paren"
	DB	176
	DB	148
	DB	0

;   78  "DATA argument has null datatype name"
	DB	176
	DB	137
	DB	153
	DB	0

;   79  "DATA argument is missing a right paren"
	DB	176
	DB	237
	DB	0

;   80  "DATA argument has null field name"
	DB	176
	DB	137
	DB	"field "
	DB	24
	DB	0

;   81  "DEFINE first argument is not a string"
	DB	142
	DB	168
	DB	0

;   82  "DEFINE first argument is null"
	DB	142
	DB	7
	DB	159
	DB	0

;   83  "DEFINE first argument is missing a left paren"
	DB	142
	DB	7
	DB	148
	DB	0

;   84  "DEFINE first argument has null function name"
	DB	142
	DB	255
	DB	0

;   85  "Null arg name or missing ) in DEFINE first arg."
	DB	"Null "
	DB	218
	DB	24
	DB	" "
	DB	167
	DB	130
	DB	") "
	DB	201
	DB	142
	DB	7
	DB	"arg."
	DB	0

;   86  "DEFINE function entry point is not defined label"
	DB	142
	DB	191
	DB	0

;   87  "DETACH argument is not appropriate name"
	DB	"DETACH "
	DB	27
	DB	0

;   88  "DUMP argument is not integer"
	DB	"DUMP "
	DB	133
	DB	0

;   89  "DUMP argument is negative or too large"
	DB	"DUMP "
	DB	171
	DB	0

;   90  "DUPL second argument is not integer"
	DB	"DUPL "
	DB	208
	DB	0

;   91  "DUPL first argument is not a string or pattern"
	DB	"DUPL "
	DB	168
	DB	" "
	DB	167
	DB	28
	DB	0

;   92  "EJECT argument is not a suitable name"
	DB	196
	DB	26
	DB	0

;   93  "EJECT file does not exist"
	DB	196
	DB	174
	DB	0

;   94  "EJECT file does not permit page eject"
	DB	196
	DB	152
	DB	"page eject"
	DB	0

;   95  "EJECT caused non-recoverable output error"
	DB	196
	DB	162
	DB	0

;   96  "ENDFILE argument is not a suitable name"
	DB	149
	DB	26
	DB	0

;   97  "ENDFILE argument is null"
	DB	149
	DB	159
	DB	0

;   98  "ENDFILE file does not exist"
	DB	149
	DB	174
	DB	0

;   99  "ENDFILE file does not permit endfile"
	DB	149
	DB	152
	DB	"endfile"
	DB	0

;  100  "ENDFILE caused non-recoverable output error"
	DB	149
	DB	162
	DB	0

;  101  "EQ first argument is not numeric"
	DB	"EQ "
	DB	209
	DB	0

;  102  "EQ second argument is not numeric"
	DB	"EQ "
	DB	205
	DB	0

;  103  "EVAL argument is not expression"
	DB	"EVAL "
	DB	1
	DB	"expression"
	DB	0

;  104  "EXIT first argument is not suitable integer or string"
	DB	223
	DB	202
	DB	0

;  105  "EXIT action not available in this implementation"
	DB	223
	DB	"action "
	DB	20
	DB	"available "
	DB	201
	DB	"th"
	DB	22
	DB	"implementation"
	DB	0

;  106  "EXIT action caused irrecoverable error"
	DB	223
	DB	151
	DB	0

;  107  "FIELD second argument is not integer"
	DB	242
	DB	208
	DB	0

;  108  "FIELD first argument is not datatype name"
	DB	242
	DB	7
	DB	1
	DB	153
	DB	0

;  109  "GE first argument is not numeric"
	DB	"GE "
	DB	209
	DB	0

;  110  "GE second argument is not numeric"
	DB	"GE "
	DB	205
	DB	0

;  111  "GT first argument is not numeric"
	DB	"GT "
	DB	209
	DB	0

;  112  "GT second argument is not numeric"
	DB	"GT "
	DB	205
	DB	0

;  113  "INPUT third argument is not a string"
	DB	233
	DB	128
	DB	0

;  114  "Inappropriate second argument for INPUT"
	DB	19
	DB	9
	DB	2
	DB	"f"
	DB	167
	DB	"INPUT"
	DB	0

;  115  "Inappropriate first argument for INPUT"
	DB	19
	DB	7
	DB	2
	DB	"f"
	DB	167
	DB	"INPUT"
	DB	0

;  116  "Inappropriate file specification for INPUT"
	DB	186
	DB	"INPUT"
	DB	0

;  117  "INPUT file cannot be read"
	DB	233
	DB	199
	DB	"read"
	DB	0

;  118  "LE first argument is not numeric"
	DB	"LE "
	DB	209
	DB	0

;  119  "LE second argument is not numeric"
	DB	"LE "
	DB	205
	DB	0

;  120  "LEN argument is not integer or expression"
	DB	254
	DB	253
	DB	0

;  121  "LEN argument is negative or too large"
	DB	254
	DB	171
	DB	0

;  122  "LEQ first argument is not a string"
	DB	"LEQ "
	DB	168
	DB	0

;  123  "LEQ second argument is not a string"
	DB	"LEQ "
	DB	180
	DB	0

;  124  "LGE first argument is not a string"
	DB	"LGE "
	DB	168
	DB	0

;  125  "LGE second argument is not a string"
	DB	"LGE "
	DB	180
	DB	0

;  126  "LGT first argument is not a string"
	DB	"LGT "
	DB	168
	DB	0

;  127  "LGT second argument is not a string"
	DB	"LGT "
	DB	180
	DB	0

;  128  "LLE first argument is not a string"
	DB	"LLE "
	DB	168
	DB	0

;  129  "LLE second argument is not a string"
	DB	"LLE "
	DB	180
	DB	0

;  130  "LLT first argument is not a string"
	DB	"LLT "
	DB	168
	DB	0

;  131  "LLT second argument is not a string"
	DB	"LLT "
	DB	180
	DB	0

;  132  "LNE first argument is not a string"
	DB	"LNE "
	DB	168
	DB	0

;  133  "LNE second argument is not a string"
	DB	"LNE "
	DB	180
	DB	0

;  134  "LOCAL second argument is not integer"
	DB	"LOCAL "
	DB	208
	DB	0

;  135  "LOCAL first arg is not a program function name"
	DB	"LOCAL "
	DB	7
	DB	218
	DB	22
	DB	20
	DB	"a "
	DB	155
	DB	0

;  136  "LOAD second argument is not a string"
	DB	140
	DB	180
	DB	0

;  137  "LOAD first argument is not a string"
	DB	140
	DB	168
	DB	0

;  138  "LOAD first argument is null"
	DB	140
	DB	7
	DB	159
	DB	0

;  139  "LOAD first argument is missing a left paren"
	DB	140
	DB	7
	DB	148
	DB	0

;  140  "LOAD first argument has null function name"
	DB	140
	DB	255
	DB	0

;  141  "LOAD first argument is missing a right paren"
	DB	140
	DB	7
	DB	237
	DB	0

;  142  "LOAD function does not exist"
	DB	140
	DB	13
	DB	"does "
	DB	20
	DB	"exist"
	DB	0

;  143  "LOAD function caused input error during load"
	DB	140
	DB	13
	DB	"caused input "
	DB	150
	DB	" "
	DB	225
	DB	"load"
	DB	0

;  144  "LPAD third argument is not a string"
	DB	"LPAD "
	DB	128
	DB	0

;  145  "LPAD second argument is not integer"
	DB	"LPAD "
	DB	208
	DB	0

;  146  "LPAD first argument is not a string"
	DB	"LPAD "
	DB	168
	DB	0

;  147  "LT first argument is not numeric"
	DB	"LT "
	DB	209
	DB	0

;  148  "LT second argument is not numeric"
	DB	"LT "
	DB	205
	DB	0

;  149  "NE first argument is not numeric"
	DB	"NE "
	DB	209
	DB	0

;  150  "NE second argument is not numeric"
	DB	"NE "
	DB	205
	DB	0

;  151  "NOTANY argument is not a string or expression"
	DB	"NOTANY "
	DB	247
	DB	0

;  152  "OPSYN third argument is not integer"
	DB	178
	DB	182
	DB	0

;  153  "OPSYN third argument is negative or too large"
	DB	178
	DB	"third "
	DB	171
	DB	0

;  154  "OPSYN second arg is not natural variable name"
	DB	178
	DB	9
	DB	143
	DB	0

;  155  "OPSYN first arg is not natural variable name"
	DB	178
	DB	7
	DB	143
	DB	0

;  156  "OPSYN first arg is not correct operator name"
	DB	178
	DB	164
	DB	0

;  157  "OUTPUT third argument is not a string"
	DB	227
	DB	128
	DB	0

;  158  "Inappropriate second argument for OUTPUT"
	DB	19
	DB	9
	DB	246
	DB	0

;  159  "Inappropriate first argument for OUTPUT"
	DB	19
	DB	7
	DB	246
	DB	0

;  160  "Inappropriate file specification for OUTPUT"
	DB	186
	DB	231
	DB	0

;  161  "OUTPUT file cannot be written to"
	DB	227
	DB	199
	DB	"written to"
	DB	0

;  162  "POS argument is not integer or expression"
	DB	245
	DB	253
	DB	0

;  163  "POS argument is negative or too large"
	DB	245
	DB	171
	DB	0

;  164  "PROTOTYPE argument is not valid object"
	DB	"PROTOTYPE "
	DB	1
	DB	"valid object"
	DB	0

;  165  "REMDR second argument is not numeric"
	DB	197
	DB	205
	DB	0

;  166  "REMDR first argument is not numeric"
	DB	197
	DB	209
	DB	0

;  167  "REMDR caused integer overflow"
	DB	197
	DB	18
	DB	0

;  168  "REPLACE third argument is not a string"
	DB	200
	DB	128
	DB	0

;  169  "REPLACE second argument is not a string"
	DB	200
	DB	180
	DB	0

;  170  "REPLACE first argument is not a string"
	DB	200
	DB	168
	DB	0

;  171  "Null or unequally long 2nd, 3rd args to REPLACE"
	DB	"Null "
	DB	167
	DB	"unequally long 2nd, 3rd args "
	DB	217
	DB	"REPLACE"
	DB	0

;  172  "REWIND argument is not a suitable name"
	DB	163
	DB	26
	DB	0

;  173  "REWIND argument is null"
	DB	163
	DB	159
	DB	0

;  174  "REWIND file does not exist"
	DB	163
	DB	174
	DB	0

;  175  "REWIND file does not permit rewind"
	DB	163
	DB	152
	DB	"rewind"
	DB	0

;  176  "REWIND caused non-recoverable error"
	DB	163
	DB	12
	DB	150
	DB	0

;  177  "REVERSE argument is not a string"
	DB	"REVERSE "
	DB	4
	DB	0

;  178  "RPAD third argument is not a string"
	DB	"RPAD "
	DB	128
	DB	0

;  179  "RPAD second argument is not integer"
	DB	"RPAD "
	DB	208
	DB	0

;  180  "RPAD first argument is not a string"
	DB	"RPAD "
	DB	168
	DB	0

;  181  "RTAB argument is not integer or expression"
	DB	224
	DB	253
	DB	0

;  182  "RTAB argument is negative or too large"
	DB	224
	DB	171
	DB	0

;  183  "TAB argument is not integer or expression"
	DB	248
	DB	253
	DB	0

;  184  "TAB argument is negative or too large"
	DB	248
	DB	171
	DB	0

;  185  "RPOS argument is not integer or expression"
	DB	211
	DB	253
	DB	0

;  186  "RPOS argument is negative or too large"
	DB	211
	DB	171
	DB	0

;  187  "SETEXIT argument is not label name or null"
	DB	"SET"
	DB	223
	DB	1
	DB	189
	DB	" "
	DB	24
	DB	" "
	DB	167
	DB	"null"
	DB	0

;  188  "SPAN argument is not a string or expression"
	DB	"SPAN "
	DB	247
	DB	0

;  189  "SIZE argument is not a string"
	DB	"SIZE "
	DB	4
	DB	0

;  190  "STOPTR first argument is not appropriate name"
	DB	"STOPTR "
	DB	7
	DB	27
	DB	0

;  191  "STOPTR second argument is not trace type"
	DB	"STOPTR "
	DB	230
	DB	0

;  192  "SUBSTR third argument is not integer"
	DB	214
	DB	182
	DB	0

;  193  "SUBSTR second argument is not integer"
	DB	214
	DB	208
	DB	0

;  194  "SUBSTR first argument is not a string"
	DB	214
	DB	168
	DB	0

;  195  "TABLE argument is not integer"
	DB	"TABLE "
	DB	133
	DB	0

;  196  "TABLE argument is out of range"
	DB	"TABLE "
	DB	204
	DB	0

;  197  "TRACE fourth arg is not function name or null"
	DB	241
	DB	"fourth "
	DB	218
	DB	22
	DB	20
	DB	13
	DB	24
	DB	" "
	DB	167
	DB	"null"
	DB	0

;  198  "TRACE first argument is not appropriate name"
	DB	241
	DB	7
	DB	27
	DB	0

;  199  "TRACE second argument is not trace type"
	DB	241
	DB	230
	DB	0

;  200  "TRIM argument is not a string"
	DB	"TRIM "
	DB	4
	DB	0

;  201  "UNLOAD argument is not natural variable name"
	DB	"UN"
	DB	140
	DB	1
	DB	25
	DB	0

;  202  "Input from file caused non-recoverable error"
	DB	"Input "
	DB	228
	DB	141
	DB	12
	DB	150
	DB	0

;  203  "Input file record has incorrect format"
	DB	"Input "
	DB	141
	DB	"record has incorrect format"
	DB	0

;  204  "Memory overflow"
	DB	"Memory "
	DB	14
	DB	0

;  205  "String length exceeds value of MAXLNGTH keyword"
	DB	"String "
	DB	221
	DB	0

;  206  "Output caused file overflow"
	DB	"Output caused "
	DB	141
	DB	14
	DB	0

;  207  "Output caused non-recoverable error"
	DB	"Output "
	DB	12
	DB	150
	DB	0

;  208  "Keyword value assigned is not integer"
	DB	173
	DB	226
	DB	0

;  209  "Keyword in assignment is protected"
	DB	173
	DB	201
	DB	"assignment "
	DB	22
	DB	"protected"
	DB	0

;  210  "Keyword value assigned is negative or too large"
	DB	173
	DB	232
	DB	0

;  211  "Value assigned to keyword ERRTEXT not a string"
	DB	194
	DB	212
	DB	0

;  212  "Syntax error: Value used where name is required"
	DB	6
	DB	194
	DB	"used where "
	DB	24
	DB	" "
	DB	22
	DB	"required"
	DB	0

;  213  "Syntax error: Statement is too complicated."
	DB	6
	DB	243
	DB	22
	DB	"too complicated."
	DB	0

;  214  "Bad label or misplaced continuation line"
	DB	"Bad "
	DB	189
	DB	" "
	DB	167
	DB	"misplaced continuation line"
	DB	0

;  215  "Syntax error: Undefined or erroneous entry label"
	DB	6
	DB	192
	DB	0

;  216  "Syntax error: Missing END line"
	DB	139
	DB	"END line"
	DB	0

;  217  "Syntax error: Duplicate label"
	DB	6
	DB	238
	DB	0

;  218  "Syntax error: Duplicated goto field"
	DB	6
	DB	"Duplicated go"
	DB	217
	DB	"field"
	DB	0

;  219  "Syntax error: Empty goto field"
	DB	6
	DB	"Empty go"
	DB	217
	DB	"field"
	DB	0

;  220  "Syntax error: Missing operator"
	DB	139
	DB	"operator"
	DB	0

;  221  "Syntax error: Missing operand"
	DB	139
	DB	"operand"
	DB	0

;  222  "Syntax error: Invalid use of left bracket"
	DB	145
	DB	"left "
	DB	216
	DB	0

;  223  "Syntax error: Invalid use of comma"
	DB	145
	DB	"comma"
	DB	0

;  224  "Syntax error: Unbalanced right parenthesis"
	DB	215
	DB	"parenthesis"
	DB	0

;  225  "Syntax error: Unbalanced right bracket"
	DB	215
	DB	216
	DB	0

;  226  "Syntax error: Missing right paren"
	DB	139
	DB	21
	DB	"paren"
	DB	0

;  227  "Syntax error: Right paren missing from goto"
	DB	6
	DB	"Right paren "
	DB	130
	DB	228
	DB	"goto"
	DB	0

;  228  "Syntax error: Right bracket missing from goto"
	DB	6
	DB	"Right "
	DB	216
	DB	" "
	DB	130
	DB	228
	DB	"goto"
	DB	0

;  229  "Syntax error: Missing right array bracket"
	DB	139
	DB	21
	DB	"array "
	DB	216
	DB	0

;  230  "Syntax error: Illegal character"
	DB	6
	DB	"Illegal character"
	DB	0

;  231  "Syntax error: Invalid numeric item"
	DB	6
	DB	"Invalid "
	DB	8
	DB	" item"
	DB	0

;  232  "Syntax error: Unmatched string quote"
	DB	6
	DB	"Unmatched "
	DB	179
	DB	" quote"
	DB	0

;  233  "Syntax error: Invalid use of operator"
	DB	145
	DB	"operator"
	DB	0

;  234  "Syntax error: Goto field incorrect"
	DB	6
	DB	136
	DB	"field incorrect"
	DB	0

;  235  "Subscripted operand is not table or array"
	DB	"Subscripted "
	DB	3
	DB	"table "
	DB	167
	DB	"array"
	DB	0

;  236  "Array referenced with wrong number of subscripts"
	DB	"Array "
	DB	195
	DB	"wrong number "
	DB	193
	DB	"subscripts"
	DB	0

;  237  "Table referenced with more than one subscript"
	DB	"Table "
	DB	195
	DB	"more than one subscript"
	DB	0

;  238  "Array subscript is not integer"
	DB	"Array "
	DB	210
	DB	0

;  239  "Indirection operand is not name"
	DB	"Indirection "
	DB	3
	DB	24
	DB	0

;  240  "Pattern match right operand is not pattern"
	DB	172
	DB	"match "
	DB	21
	DB	3
	DB	28
	DB	0

;  241  "Pattern match left operand is not a string"
	DB	172
	DB	203
	DB	0

;  242  "Function return from level zero"
	DB	165
	DB	"return "
	DB	228
	DB	"level zero"
	DB	0

;  243  "Function result in NRETURN is not name"
	DB	165
	DB	177
	DB	0

;  244  "Statement count exceeds value of STLIMIT keyword"
	DB	243
	DB	220
	DB	0

;  245  "Translation/execution time expired"
	DB	"Translation/execution time expired"
	DB	0

;  246  "Stack overflow"
	DB	"Stack "
	DB	14
	DB	0

;  247  "Invalid control statement"
	DB	"Invalid control statement"
	DB	0

;  248  "Attempted redefinition of system function"
	DB	"Attempted redefinition "
	DB	193
	DB	"system function"
	DB	0

;  249  "Expression evaluated by name returned value"
	DB	234
	DB	11
	DB	207
	DB	"value"
	DB	0

;  250  "Insufficient memory to complete dump"
	DB	"Insufficient memory "
	DB	217
	DB	"complete dump"
	DB	0

;  251  "Keyword operand is not name of defined keyword"
	DB	173
	DB	222
	DB	0

;  252  "Error on printing to interactive channel"
	DB	"Err"
	DB	167
	DB	"on printing "
	DB	217
	DB	"interactive channel"
	DB	0

;  253  "Print limit exceeded on standard output channel"
	DB	"Print limit exceeded on standard output channel"
	DB	0

;  254  "Erroneous argument for HOST"
	DB	252
	DB	2
	DB	"f"
	DB	167
	DB	"HOST"
	DB	0

;  255  "Error during execution of HOST"
	DB	"Err"
	DB	167
	DB	225
	DB	"execution "
	DB	193
	DB	"HOST"
	DB	0

;  256  "SORT/RSORT 1st arg not suitable ARRAY or TABLE"
	DB	175
	DB	"1st "
	DB	218
	DB	20
	DB	"suitable "
	DB	160
	DB	167
	DB	"TABLE"
	DB	0

;  257  "Erroneous 2nd arg in SORT/RSORT of vector"
	DB	252
	DB	"2nd "
	DB	218
	DB	201
	DB	175
	DB	193
	DB	"vector"
	DB	0

;  258  "SORT/RSORT 2nd arg out of range or non-integer"
	DB	175
	DB	198
	DB	0

;  259  "FENCE argument is not pattern"
	DB	"FENCE "
	DB	1
	DB	28
	DB	0

;  260  "Conversion array size exceeds maximum permitted"
	DB	"Conversion array "
	DB	146
	DB	0

;  261  "Addition caused real overflow"
	DB	166
	DB	15
	DB	0

;  262  "Division caused real overflow"
	DB	161
	DB	15
	DB	0

;  263  "Multiplication caused real overflow"
	DB	134
	DB	15
	DB	0

;  264  "Subtraction caused real overflow"
	DB	138
	DB	15
	DB	0

;  265  "External function argument is not real"
	DB	156
	DB	1
	DB	"real"
	DB	0

;  266  "Exponentiation caused real overflow"
	DB	23
	DB	15
	DB	0

;  267  ""
	DB	0

;  268  "Inconsistent value assigned to keyword PROFILE"
	DB	"Inconsistent "
	DB	158
	DB	29
	DB	"PROFILE"
	DB	0

;  269  ""
	DB	0

;  270  ""
	DB	0

;  271  ""
	DB	0

;  272  ""
	DB	0

;  273  ""
	DB	0

;  274  "Value assigned to keyword FULLSCAN is zero"
	DB	194
	DB	29
	DB	"FULLSCAN "
	DB	22
	DB	"zero"
	DB	0

;  275  ""
	DB	0

;  276  ""
	DB	0

;  277  ""
	DB	0

;  278  ""
	DB	0

;  279  ""
	DB	0

;  280  ""
	DB	0

;  281  "CHAR argument not integer"
	DB	"CHAR "
	DB	2
	DB	20
	DB	10
	DB	0

;  282  "CHAR argument not in range"
	DB	"CHAR "
	DB	2
	DB	20
	DB	201
	DB	"range"
	DB	0

;  283  ""
	DB	0

;  284  "Excessively nested INCLUDE files"
	DB	"Excessively nested INCLUDE files"
	DB	0

;  285  "INCLUDE file cannot be opened"
	DB	"INCLUDE "
	DB	199
	DB	"opened"
	DB	0

;  286  "Function call to undefined entry label"
	DB	165
	DB	190
	DB	0

;  287  "Value assigned to keyword MAXLNGTH is too small"
	DB	194
	DB	29
	DB	187
	DB	22
	DB	"too small"
	DB	0

;  288  "EXIT second argument is not a string"
	DB	223
	DB	180
	DB	0

;  289  "INPUT channel currently in use"
	DB	233
	DB	170
	DB	0

;  290  "OUTPUT channel currently in use"
	DB	227
	DB	170
	DB	0

;  291  "SET first argument is not a suitable name"
	DB	213
	DB	7
	DB	26
	DB	0

;  292  "SET first argument is null"
	DB	213
	DB	7
	DB	159
	DB	0

;  293  "Inappropriate second argument to SET"
	DB	19
	DB	9
	DB	2
	DB	217
	DB	"SET"
	DB	0

;  294  "Inappropriate third argument to SET"
	DB	19
	DB	"third "
	DB	2
	DB	217
	DB	"SET"
	DB	0

;  295  "SET file does not exist"
	DB	213
	DB	174
	DB	0

;  296  "SET file does not permit setting file pointer"
	DB	213
	DB	152
	DB	"setting "
	DB	141
	DB	"pointer"
	DB	0

;  297  "SET caused non-recoverable I/O error"
	DB	213
	DB	12
	DB	"I/O "
	DB	150
	DB	0

;  298  "External function argument is not file"
	DB	156
	DB	1
	DB	"file"
	DB	0

;  299  "Internal logic error: Unexpected PPM branch"
	DB	"Internal logic "
	DB	150
	DB	": "
	DB	239
	DB	"PPM branch"
	DB	0

;  300  ""
	DB	0

;  301  "ATAN argument not numeric"
	DB	"ATAN "
	DB	144
	DB	0

;  302  "CHOP argument not numeric"
	DB	"CHOP "
	DB	144
	DB	0

;  303  "COS argument not numeric"
	DB	"COS "
	DB	144
	DB	0

;  304  "EXP argument not numeric"
	DB	"EXP "
	DB	144
	DB	0

;  305  "EXP produced real overflow"
	DB	"EXP "
	DB	30
	DB	0

;  306  "LN argument not numeric"
	DB	"LN "
	DB	144
	DB	0

;  307  "LN produced real overflow"
	DB	"LN "
	DB	30
	DB	0

;  308  "SIN argument not numeric"
	DB	"SIN "
	DB	144
	DB	0

;  309  "TAN argument not numeric"
	DB	"TAN "
	DB	144
	DB	0

;  310  "TAN produced real overflow or argument is out of range"
	DB	"TAN "
	DB	30
	DB	" "
	DB	167
	DB	204
	DB	0

;  311  "Exponentiation of negative base to non-integral power"
	DB	23
	DB	193
	DB	"negative base "
	DB	217
	DB	"non-integral power"
	DB	0

;  312  "REMDR caused real overflow"
	DB	197
	DB	15
	DB	0

;  313  "SQRT argument not numeric"
	DB	"SQRT "
	DB	144
	DB	0

;  314  "SQRT argument negative"
	DB	"SQRT "
	DB	2
	DB	"negative"
	DB	0

;  315  "LN argument negative"
	DB	"LN "
	DB	2
	DB	"negative"
	DB	0

;  316  "BACKSPACE argument is not a suitable name"
	DB	154
	DB	26
	DB	0

;  317  "BACKSPACE file does not exist"
	DB	154
	DB	174
	DB	0

;  318  "BACKSPACE file does not permit backspace"
	DB	154
	DB	152
	DB	"backspace"
	DB	0

;  319  "BACKSPACE caused non-recoverable error"
	DB	154
	DB	12
	DB	150
	DB	0

;  320  "User interrupt"
	DB	"User interrupt"
	DB	0

;  321  "Goto SCONTINUE with no preceding error"
	DB	235
	DB	131
	DB	0

;  322  "COS argument is out of range"
	DB	"COS "
	DB	204
	DB	0

;  323  "SIN argument is out of range"
	DB	"SIN "
	DB	204
	DB	0

;  324  ""
	DB	0

;  325  ""
	DB	0

;  326  "Calling external function - bad argument type"
	DB	181
	DB	"bad "
	DB	2
	DB	"type"
	DB	0

;  327  "Calling external function - not found"
	DB	181
	DB	20
	DB	"found"
	DB	0

;  328  "LOAD function - insufficient memory"
	DB	140
	DB	13
	DB	"- insufficient memory"
	DB	0

;  329  "Requested MAXLNGTH too large"
	DB	"Requested "
	DB	187
	DB	"too large"
	DB	0

;  330  "DATE argument is not integer"
	DB	"DATE "
	DB	133
	DB	0

;  331  "Goto SCONTINUE with no user interrupt"
	DB	235
	DB	"with no user interrupt"
	DB	0

;  332  "Goto CONTINUE with error in failure goto"
	DB	251
	DB	"with "
	DB	150
	DB	" "
	DB	201
	DB	"failure goto"
	DB	0

	PUBLIC	_phrases
_phrases	DB	0

;    1  "argument is not "
	DB	2
	DB	22
	DB	20
	DB	0

;    2  "argument "
	DB	"argument "
	DB	0

;    3  "operand is not "
	DB	"operand "
	DB	22
	DB	20
	DB	0

;    4  "argument is not a string"
	DB	1
	DB	"a "
	DB	179
	DB	0

;    5  "is negative or too large"
	DB	22
	DB	"negative "
	DB	167
	DB	"too large"
	DB	0

;    6  "Syntax error: "
	DB	"Syntax "
	DB	150
	DB	": "
	DB	0

;    7  "first "
	DB	"first "
	DB	0

;    8  "numeric"
	DB	"numeric"
	DB	0

;    9  "second "
	DB	"second "
	DB	0

;   10  "integer"
	DB	"integer"
	DB	0

;   11  "evaluated "
	DB	"evaluated "
	DB	0

;   12  "caused non-recoverable "
	DB	"caused non-recoverable "
	DB	0

;   13  "function "
	DB	"function "
	DB	0

;   14  "overflow"
	DB	"overflow"
	DB	0

;   15  "caused real overflow"
	DB	"caused real "
	DB	14
	DB	0

;   16  "file does not "
	DB	141
	DB	"does "
	DB	20
	DB	0

;   17  " or expression"
	DB	" "
	DB	167
	DB	"expression"
	DB	0

;   18  "caused integer overflow"
	DB	"caused "
	DB	10
	DB	" "
	DB	14
	DB	0

;   19  "Inappropriate "
	DB	"Inappropriate "
	DB	0

;   20  "not "
	DB	"not "
	DB	0

;   21  "right "
	DB	"right "
	DB	0

;   22  "is "
	DB	"is "
	DB	0

;   23  "Exponentiation "
	DB	"Exponentiation "
	DB	0

;   24  "name"
	DB	"name"
	DB	0

;   25  "natural variable name"
	DB	"natural variable "
	DB	24
	DB	0

;   26  "argument is not a suitable name"
	DB	1
	DB	"a suitable "
	DB	24
	DB	0

;   27  "argument is not appropriate name"
	DB	1
	DB	"appropriate "
	DB	24
	DB	0

;   28  "pattern"
	DB	"pattern"
	DB	0

;   29  "assigned to keyword "
	DB	"assigned "
	DB	217
	DB	219
	DB	" "
	DB	0

;   30  "produced real overflow"
	DB	"produced real "
	DB	14
	DB	0

;   31  "left operand is not numeric"
	DB	"left "
	DB	3
	DB	8
	DB	0

;  128  "third argument is not a string"
	DB	"third "
	DB	4
	DB	0

;  129  "assignment left operand is not pattern"
	DB	"assignment left "
	DB	3
	DB	28
	DB	0

;  130  "missing "
	DB	"missing "
	DB	0

;  131  "with no preceding error"
	DB	"with no preceding "
	DB	150
	DB	0

;  132  "out of range"
	DB	"out "
	DB	193
	DB	"range"
	DB	0

;  133  "argument is not integer"
	DB	1
	DB	10
	DB	0

;  134  "Multiplication "
	DB	"Multiplication "
	DB	0

;  135  "operand is not a string or pattern"
	DB	3
	DB	"a "
	DB	179
	DB	" "
	DB	167
	DB	28
	DB	0

;  136  "Goto "
	DB	"Go"
	DB	217
	DB	0

;  137  "argument has null "
	DB	2
	DB	"has null "
	DB	0

;  138  "Subtraction "
	DB	"Subtraction "
	DB	0

;  139  "Syntax error: Missing "
	DB	6
	DB	"Missing "
	DB	0

;  140  "LOAD "
	DB	"LOAD "
	DB	0

;  141  "file "
	DB	"file "
	DB	0

;  142  "DEFINE "
	DB	"DEFINE "
	DB	0

;  143  "arg is not natural variable name"
	DB	218
	DB	22
	DB	20
	DB	25
	DB	0

;  144  "argument not numeric"
	DB	2
	DB	20
	DB	8
	DB	0

;  145  "Syntax error: Invalid use of "
	DB	6
	DB	"Invalid use "
	DB	193
	DB	0

;  146  "size exceeds maximum permitted"
	DB	"size exceeds maximum permitted"
	DB	0

;  147  "dimension is zero,negative or out of range"
	DB	"dimension "
	DB	22
	DB	"zero,negative "
	DB	167
	DB	132
	DB	0

;  148  "argument is missing a left paren"
	DB	2
	DB	22
	DB	130
	DB	"a left paren"
	DB	0

;  149  "ENDFILE "
	DB	"ENDFILE "
	DB	0

;  150  "error"
	DB	"error"
	DB	0

;  151  "action caused irrecoverable error"
	DB	"action caused irrecoverable "
	DB	150
	DB	0

;  152  "file does not permit "
	DB	16
	DB	"permit "
	DB	0

;  153  "datatype name"
	DB	"datatype "
	DB	24
	DB	0

;  154  "BACKSPACE "
	DB	"BACKSPACE "
	DB	0

;  155  "program function name"
	DB	"program "
	DB	13
	DB	24
	DB	0

;  156  "External function "
	DB	"External "
	DB	13
	DB	0

;  157  "argument is not numeric"
	DB	1
	DB	8
	DB	0

;  158  "value "
	DB	"value "
	DB	0

;  159  "argument is null"
	DB	2
	DB	22
	DB	"null"
	DB	0

;  160  "ARRAY "
	DB	"ARRAY "
	DB	0

;  161  "Division "
	DB	"Division "
	DB	0

;  162  "caused non-recoverable output error"
	DB	12
	DB	"output "
	DB	150
	DB	0

;  163  "REWIND "
	DB	"REWIND "
	DB	0

;  164  "first arg is not correct operator name"
	DB	7
	DB	218
	DB	22
	DB	20
	DB	"correct operat"
	DB	167
	DB	24
	DB	0

;  165  "Function "
	DB	"Function "
	DB	0

;  166  "Addition "
	DB	"Addition "
	DB	0

;  167  "or "
	DB	"or "
	DB	0

;  168  "first argument is not a string"
	DB	7
	DB	4
	DB	0

;  169  "bound is not integer"
	DB	"bound "
	DB	22
	DB	20
	DB	10
	DB	0

;  170  "channel currently in use"
	DB	"channel currently "
	DB	201
	DB	"use"
	DB	0

;  171  "argument is negative or too large"
	DB	2
	DB	5
	DB	0

;  172  "Pattern "
	DB	"Pattern "
	DB	0

;  173  "Keyword "
	DB	"Keyword "
	DB	0

;  174  "file does not exist"
	DB	16
	DB	"exist"
	DB	0

;  175  "SORT/RSORT "
	DB	"SORT/RSORT "
	DB	0

;  176  "DATA "
	DB	"DATA "
	DB	0

;  177  "result in NRETURN is not name"
	DB	"result "
	DB	201
	DB	"NRETURN "
	DB	22
	DB	20
	DB	24
	DB	0

;  178  "OPSYN "
	DB	"OPSYN "
	DB	0

;  179  "string"
	DB	"string"
	DB	0

;  180  "second argument is not a string"
	DB	9
	DB	4
	DB	0

;  181  "Calling external function - "
	DB	"Calling external "
	DB	13
	DB	"- "
	DB	0

;  182  "third argument is not integer"
	DB	"third "
	DB	133
	DB	0

;  183  "Undefined "
	DB	"Undefined "
	DB	0

;  184  "does not evaluate to pattern"
	DB	"does "
	DB	20
	DB	"evaluate "
	DB	217
	DB	28
	DB	0

;  185  "right operand is not numeric"
	DB	21
	DB	3
	DB	8
	DB	0

;  186  "Inappropriate file specification for "
	DB	19
	DB	141
	DB	"specification f"
	DB	167
	DB	0

;  187  "MAXLNGTH "
	DB	"MAXLNGTH "
	DB	0

;  188  "replacement right operand is not a string"
	DB	"replacement "
	DB	21
	DB	3
	DB	"a "
	DB	179
	DB	0

;  189  "label"
	DB	"label"
	DB	0

;  190  "call to undefined entry label"
	DB	"call "
	DB	217
	DB	"undefined entry "
	DB	189
	DB	0

;  191  "function entry point is not defined label"
	DB	13
	DB	"entry point "
	DB	22
	DB	20
	DB	"defined "
	DB	189
	DB	0

;  192  "Undefined or erroneous entry label"
	DB	183
	DB	167
	DB	"erroneous entry "
	DB	189
	DB	0

;  193  "of "
	DB	"of "
	DB	0

;  194  "Value "
	DB	"Value "
	DB	0

;  195  "referenced with "
	DB	"referenced with "
	DB	0

;  196  "EJECT "
	DB	"EJECT "
	DB	0

;  197  "REMDR "
	DB	"REMDR "
	DB	0

;  198  "2nd arg out of range or non-integer"
	DB	"2nd "
	DB	218
	DB	132
	DB	" "
	DB	167
	DB	"non-"
	DB	10
	DB	0

;  199  "file cannot be "
	DB	141
	DB	"can"
	DB	20
	DB	"be "
	DB	0

;  200  "REPLACE "
	DB	"REPLACE "
	DB	0

;  201  "in "
	DB	"in "
	DB	0

;  202  "first argument is not suitable integer or string"
	DB	7
	DB	1
	DB	"suitable "
	DB	10
	DB	" "
	DB	167
	DB	179
	DB	0

;  203  "match left operand is not a string"
	DB	"match left "
	DB	3
	DB	"a "
	DB	179
	DB	0

;  204  "argument is out of range"
	DB	2
	DB	22
	DB	132
	DB	0

;  205  "second argument is not numeric"
	DB	9
	DB	157
	DB	0

;  206  "Concatenation "
	DB	"Concatenation "
	DB	0

;  207  "by name returned "
	DB	"by "
	DB	24
	DB	" returned "
	DB	0

;  208  "second argument is not integer"
	DB	9
	DB	133
	DB	0

;  209  "first argument is not numeric"
	DB	7
	DB	157
	DB	0

;  210  "subscript is not integer"
	DB	"subscript "
	DB	22
	DB	20
	DB	10
	DB	0

;  211  "RPOS "
	DB	"R"
	DB	245
	DB	0

;  212  "assigned to keyword ERRTEXT not a string"
	DB	29
	DB	"ERRTEXT "
	DB	20
	DB	"a "
	DB	179
	DB	0

;  213  "SET "
	DB	"SET "
	DB	0

;  214  "SUBSTR "
	DB	"SUBSTR "
	DB	0

;  215  "Syntax error: Unbalanced right "
	DB	6
	DB	"Unbalanced "
	DB	21
	DB	0

;  216  "bracket"
	DB	"bracket"
	DB	0

;  217  "to "
	DB	"to "
	DB	0

;  218  "arg "
	DB	"arg "
	DB	0

;  219  "keyword"
	DB	"keyword"
	DB	0

;  220  "count exceeds value of STLIMIT keyword"
	DB	"count exceeds "
	DB	158
	DB	193
	DB	"STLIMIT "
	DB	219
	DB	0

;  221  "length exceeds value of MAXLNGTH keyword"
	DB	"length exceeds "
	DB	158
	DB	193
	DB	187
	DB	219
	DB	0

;  222  "operand is not name of defined keyword"
	DB	3
	DB	24
	DB	" "
	DB	193
	DB	"defined "
	DB	219
	DB	0

;  223  "EXIT "
	DB	"EXIT "
	DB	0

;  224  "RTAB "
	DB	"R"
	DB	248
	DB	0

;  225  "during "
	DB	"during "
	DB	0

;  226  "value assigned is not integer"
	DB	158
	DB	"assigned "
	DB	22
	DB	20
	DB	10
	DB	0

;  227  "OUTPUT "
	DB	231
	DB	" "
	DB	0

;  228  "from "
	DB	"from "
	DB	0

;  229  "Alternation "
	DB	"Alternation "
	DB	0

;  230  "second argument is not trace type"
	DB	9
	DB	1
	DB	"trace type"
	DB	0

;  231  "OUTPUT"
	DB	"OUTPUT"
	DB	0

;  232  "value assigned is negative or too large"
	DB	158
	DB	"assigned "
	DB	5
	DB	0

;  233  "INPUT "
	DB	"INPUT "
	DB	0

;  234  "Expression "
	DB	"Expression "
	DB	0

;  235  "Goto SCONTINUE "
	DB	136
	DB	"SCONTINUE "
	DB	0

;  236  "undefined label"
	DB	"undefined "
	DB	189
	DB	0

;  237  "argument is missing a right paren"
	DB	2
	DB	22
	DB	130
	DB	"a "
	DB	21
	DB	"paren"
	DB	0

;  238  "Duplicate label"
	DB	"Duplicate "
	DB	189
	DB	0

;  239  "Unexpected "
	DB	"Unexpected "
	DB	0

;  240  "argument has null variable name"
	DB	137
	DB	"variable "
	DB	24
	DB	0

;  241  "TRACE "
	DB	"TRACE "
	DB	0

;  242  "FIELD "
	DB	"FIELD "
	DB	0

;  243  "Statement "
	DB	"Statement "
	DB	0

;  244  "evaluated argument is not a string"
	DB	11
	DB	4
	DB	0

;  245  "POS "
	DB	"POS "
	DB	0

;  246  "argument for OUTPUT"
	DB	2
	DB	"f"
	DB	167
	DB	231
	DB	0

;  247  "argument is not a string or expression"
	DB	4
	DB	17
	DB	0

;  248  "TAB "
	DB	"TAB "
	DB	0

;  249  "evaluated argument is not integer"
	DB	11
	DB	133
	DB	0

;  250  "evaluated argument is negative or too large"
	DB	11
	DB	171
	DB	0

;  251  "Goto CONTINUE "
	DB	136
	DB	"CONTINUE "
	DB	0

;  252  "Erroneous "
	DB	"Erroneous "
	DB	0

;  253  "argument is not integer or expression"
	DB	133
	DB	17
	DB	0

;  254  "LEN "
	DB	"LEN "
	DB	0

;  255  "first argument has null function name"
	DB	7
	DB	137
	DB	13
	DB	24
	DB	0

ERRORS	ends
	END
