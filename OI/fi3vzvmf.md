newcpp
```
IMPORT stdobject, stdclass
IMPORT stdio, stdcalc, stdvar, stdtto
IMPORT stdstring

FUNC main (IN IS (stdvar.none); OUT IS (stdobject.int)) {
	stdio.iostream.istream cin;
	stdio.iostream.ostream cout;
	stdobject.int n;
	stdclass.array<stdobject.int,100> a;
	
	(cin n stdcalc.rtl);
	
	stdobject.int i;
	i IS 0;
	:FLAG p;
	(cin a->nums->i stdcalc.rtl);
	i IS (i 1 stdcalc.add);
	IF ((i n stdcalc.lq) (stdvar.true) stdcalc.eq) JMP p1;
	:UNFLAG p;
	DELETE i;
	
	stdobject.int i;
	i IS 0;
	:FLAG p;
	(cout (stdstring.tto.tostring(a->nums->i) stdstring.var.space stdstring.calc.add) stdcalc.ltl);
	i IS (i 1 stdcalc.add);
	IF ((i n stdcalc.lq) (stdvar.true) stdcalc.eq) JMP p1;
	:UNFLAG p;
	DELETE i;
	
	return 0;
}
```

cpp
```cpp
#include <bits/stdc++.h>

int main() {
	int n;
	int a[100];
	
	std::cin>>n;
	
	for (int i=0;i<n;i++) {
		std::cin>>a[i];
	}
	
	for (int i=0;i<n;i++) {
		std::cout<<(to_string(a[i]) + ' ');
	}
	
	return 0;
}
```