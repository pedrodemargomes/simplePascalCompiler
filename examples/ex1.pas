program HelloWorld;
var 
	a : integer;
	b : integer;
	c : integer;
	d : integer;
begin
	a := 2;
	b := 3;
	c := 4;
	d := 5;
	a := (a*(1+b) - (1+c)*123 + d);
	writeln(a);
	a := (a+b)+c+a*b+c;
	writeln(a);
end.
