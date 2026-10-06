program f;
var
	a : integer;

function fibo(n: integer): integer;
begin
	if n > 1 then
	begin
		fibo := fibo(n - 2) + fibo(n - 1);
	end
	else	
	begin
		fibo := n;
	end;
end;

begin
	a := fibo(10);
	writeln(a);
end.
