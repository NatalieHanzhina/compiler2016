program example1;
var c,d,n:integer;

function nod(a, b: integer):integer; 
begin 
while a * b <> 0 do 
begin
if a > b 
then a:= a mod b 
else b:= b mod a; 
end;
nod:= a + b; 
end;

begin
  readln(c);
  readln(d);
  n:=nod(c,d);
  writeln(n);
end.
