editor: start.o main.o trans.o score.o
	ln score start main trans -lc -O editor

start.o: externals start.c
	cc +Iexternals start -e200

main.o: externals main.c
	cc +Iexternals main

score.o: externals score.c
	cc +Iexternals score

trans.o: trans.asm
	as trans -n -d

externals: externals.c
	cc -A +Hexternals externals.c -O RAM:asm
	Delete RAM:asm
