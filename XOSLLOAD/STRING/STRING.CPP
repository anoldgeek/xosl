
//extern "C" char GetDigit(int Digit);
char GetDigit(int Digit);

char *ltoa(long Value, char *Str, int Base)
{
	char InvString[32];
	char *pStr;
	char *pOrig = Str;

	if (Base == 10 && Value < 0) {
		*Str++ = '-';
		Value = -Value;
	}

	pStr = InvString;
	do {
		*pStr++ = GetDigit(Value % Base);
		Value /= Base;
	} while (Value);

	while (pStr != InvString) {
		*Str++ = *--pStr;
	}
	*Str = '\0';
	return pOrig;
}

char *ultoa(unsigned long Value, char *Str, int Base)
{
       char InvString[32];
       char *pStr;
       char *pOrig = Str;

       pStr = InvString;
       do {
               *pStr++ = GetDigit(Value % Base);
               Value /= Base;
       } while (Value);

       while (pStr != InvString) {
               *Str++ = *--pStr;
       }
       *Str = '\0';
       return pOrig;
}

char *itoa(int Value, char *Str, int Base)
{
       return ltoa((long) Value, Str, Base);
}

/*
char *strcat( char *to, const char *from )
{
	char *temp ;

	temp = to ;
	while( *to++ ) ;
	to-- ;
	while( *to++ = *from++ ) ;
	return temp ;
}


char *strcpy( char *to, const char *from )
{
	char *temp ;

	temp = to ;
	while( *to++ = *from++ ) ;
	return temp ;
}
*/

/*
char *strcpy( char *to,  const char *from )
{
asm{
    mov si,[bp+4]
    mov di,[bp+6]
	mov dx,di	; save 'to'
	cld
L1:
	public strcpy1
strcpy1:
	mov al,[si]
	movsb
	or al,al
	jnz L1
	mov ax,dx	; return 'to'
	}
}

char *strcat(char *to, const char *from )
{
asm{
    mov di,[bp+4]
    mov si,[bp+6]
	mov dx,si	; save 'to'
	cld
L1:	lodsb
	or al,al
	jnz L1
	dec si
	xchg di,si
	jmp strcpy1
	}
}
*/

