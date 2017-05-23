program example1;
var a,i,b:integer;
function f(a,b:integer):integer;
var i,e:integer;
begin
e:=a;
i:=1;
repeat
e:=e*a;
i:=i+1;
writeln (i);
until i>=3;
f:=e;
end;

begin
read(a);
i:=0;
a:=f(a,b);
writeln (a);
end.
