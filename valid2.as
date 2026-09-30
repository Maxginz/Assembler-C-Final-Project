.extern W
.entry START
mcro my_macro
	move $1, $2
	add $3, $4, $5
mcroend
	my_macro
START: la W
	hlt
