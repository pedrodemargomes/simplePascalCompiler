program HelloWorld;
var 
	arg1 : integer;
	arg2 : integer;
	c : integer;
	fa: integer;

function f(arg1: integer) : integer;
var sum : integer;
begin
	sum := arg1;
	arg1 := 67;
	f:= sum;
end;


begin
	arg1 := 12;
	arg2 := 13;
	fa := f(arg1);
	writeln(arg1);
	writeln(arg2);
	writeln(fa);
end.
