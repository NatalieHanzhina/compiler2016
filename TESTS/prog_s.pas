program example1;
var a,i,b:integer;
function f(a,b:integer):integer;
var i,e:integer;
begin
e:=3;
i:=0;
repeat
a:=a*3;
i:=i+1;
writeln (i);
until i>=3;
f:=a;
f:=4;
end;
begin
read(a);
i:=0;
a:=f(a,b);
writeln (a);
end.
