program HelloWorld;
var 
	a : integer;
	b : integer;
	c : integer;

function f() : integer;
var fa : integer;
begin
	a := 1;
	fa := 123;
	writeln(fa);
	f := 1;
end;

begin
	a := 42;
	a := f();
	writeln(a);
end.
