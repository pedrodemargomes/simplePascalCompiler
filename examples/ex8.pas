program HelloWorld;
var 
	arg1 : integer;
	arg2 : integer;
	c : integer;
	fa: integer;

function f(arg1: integer; arg2 : integer) : integer;
var sum : integer;
begin
	sum := arg1 + arg2;
	arg1 := 67;
	f:= sum + 1 + arg1;
	writeln(f);
end;


begin
	arg1 := 12;
	arg2 := 13;
	f(arg1, arg1+arg2+10);
	writeln(arg1);
	writeln(arg2);
end.
