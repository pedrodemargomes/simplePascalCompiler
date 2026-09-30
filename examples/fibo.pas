program fibo;
var
	f1 : integer;
	f2 : integer;
	s : integer;
	n : integer;
begin
	f1 := 1;
	f2 := 1;
	n := 20;
	while n > 0 do
	begin
		writeln(f1);
		s := f1 + f2;
		f1 := f2;
		f2 := s;
		n := n - 1;
	end;


end.
