/* ========== 排列表：定理内容解析 ========== */

// 硬编码的名称 → 定理内容字典
const THEOREM_CONTENT_DICT = {
    'mp': '$0>$1,$0⊢$1',
    'a1': '⊢$0>($1>$0)',
    'a2': '⊢($0>($1>$2))>(($0>$1)>($0>$2))',
    'a3': '⊢(~$0>~$1)>($1>$0)',
    'd<>1': '⊢($0<>$1)>~(($0>$1)>~($1>$0))',
    'd<>2': '⊢~(($0>$1)>~($1>$0))>($0<>$1)',
    'd&': '⊢($0&$1)<>~($0>~$1)',
    'd|': '⊢($0|$1)<>(~$0>$1)',
    '.i': '⊢$0>$0',
    '.t': '$0>$1,$1>$2⊢$0>$2',
    '.ne': '⊢~~$0>$0',
    '.ni': '⊢$0>~~$0',
    '.cs': '$0>($1>$2)⊢$1>($0>$2)',
    '.a30': '⊢($0>$1)>(~$1>~$0)',
    '.a31': '⊢(~$0>$1)>(~$1>$0)',
    '.a32': '⊢($0>~$1)>($1>~$0)',
    '.m': '$0,~$0⊢$1',
    '.mn': '⊢(~$0>$0)>$0',
    '.m1': '~$0>~$1,~$0>$1⊢$0',
    '.m2': '$0>$1,~$0>$1⊢$1',
    '.m&': '$0,$1⊢~($0>~$1)',
    '.m&1': '⊢~($0>$1)>$0',
    '.m&2': '⊢~($0>$1)>~$1',
    '.>TF': '$0,~$1⊢~($0>$1)',
    '.>FU': '~$0⊢$0>$1',
    '.pierce': '⊢(($0>$1)>$0)>$0',
    '.<>': '$0>$1,$1>$0⊢$0<>$1',
    '.<>1': '⊢($0<>$1)>($0>$1)',
    '.<>2': '⊢($0<>$1)>($1>$0)',
    '.<>i': '⊢$0<>$0',
    '.<>s': '$0<>$1⊢$1<>$0',
    '.<>t': '$0<>$1,$1<>$2⊢$0<>$2',
    '.<>&': '⊢($0<>$1)<>(($0>$1)&($1>$0))',
    '.<>TT': '$0,$1⊢$0<>$1',
    '.<>TF': '$0,~$1⊢~($0<>$1)',
    '.<>FT': '~$0,$1⊢~($0<>$1)',
    '.<>FF': '~$0,~$1⊢$0<>$1',
    '.&': '$0,$1⊢$0&$1',
    '.&1': '$0&$1⊢$0',
    '.&2': '$0&$1⊢$1',
    '.&n1': '~$0⊢~($0&$1)',
    '.&n2': '~$1⊢~($0&$1)',
    '.&s': '$0&$1⊢$1&$0',
    '.&a': '($0&$1)&$2⊢$0&($1&$2)',
    '.&m1': '($0&$1)>$2⊢$0>($1>$2)',
    '.&m2': '$0>($1>$2)⊢($0&$1)>$2',
    '.|i': '$0|$0⊢$0',
    '.|1': '$0⊢$0|$1',
    '.|2': '$1⊢$0|$1',
    '.|n': '~$0,~$1⊢~($0|$1)',
    '.|n1': '~($0|$1)⊢~$0',
    '.|n2': '~($0|$1)⊢~$1',
    '.|s': '$0|$1⊢$1|$0',
    '.|a': '($0|$1)|$2⊢$0|($1|$2)',
    '.|m': '$0>$2,$1>$2⊢($0|$1)>$2',
    '.|m1': '$2>($0|$1)⊢($2>$0)|($2>$1)',
    '.|m2': '($2>$0)|($2>$1)⊢$2>($0|$1)',
    '.|mp': '$a1>$a2,$b1>$b2,$a1|$b1⊢$a2|$b2',
    '.n|&': '~($0|$1)⊢~$0&~$1',
    '.nn|&': '~$0|~$1⊢~($0&$1)',
    '.n&|': '~($0&$1)⊢~$0|~$1',
    '.nn&|': '~$0&~$1⊢~($0|$1)',
    '.|nn&': '$0|$1⊢~(~$0&~$1)',
    '.&nn|': '$0&$1⊢~(~$0|~$1)',
    '.n': '⊢$0<>~~$0',
    '.>|': '⊢($0>$1)<>(~$0|$1)',
    '.a3<>': '⊢(~$0>~$1)<>($1>$0)',
    '.a31<>': '⊢(~$0>$1)<>(~$1>$0)',
    '.a32<>': '⊢($0>~$1)<>($1>~$0)',
    '.<>r>': '$a1<>$a2,$b1<>$b2⊢($a1>$b1)<>($a2>$b2)',
    '.<>rn': '$0<>$1⊢~$0<>~$1',
    '.<>r<>': '$a1<>$a2,$b1<>$b2⊢($a1<>$b1)<>($a2<>$b2)',
    '.<>r&': '$a1<>$a2,$b1<>$b2⊢($a1&$b1)<>($a2&$b2)',
    '.<>r|': '$a1<>$a2,$b1<>$b2⊢($a1|$b1)<>($a2|$b2)',
    '.<>rV': 'V$2:($0<>$1)⊢(V$2:$0)<>(V$2:$1)',
    '.<>rE': 'V$2:($0<>$1)⊢(E$2:$0)<>(E$2:$1)',
    '.<>rE!': 'V$x:($1<>$2)⊢(E!$x:$1)<>(E!$x:$2)',
    'a4': '⊢(V$0:$1)>#rp($1,$0,$2)',
    'a5': '⊢(V$0:($1>$2))>((V$0:$1)>(V$0:$2))',
    'a6': '⊢#nf($1,$0)>(V$0:#nf($1,$0))',
    'a7': '⊢$0=$0',
    'a8': '⊢($0=$1)>($2>#rp($2,$0,$1,$3))',
    'dE': '⊢(E$0:$1)<>~(V$0:~$1)',
    'dE!': '⊢(E!#nf($0,$2):#nf($1,$2))<>((E#nf($0,$2):#nf($1,$2))&(V#nf($0,$2):(V$2:((#nf($1,$2)&#rp(#nf($1,$2),#nf($0,$2),$2))>(#nf($0,$2)=$2)))))',
    '.nEVn': '⊢~(E$0:$1)<>(V$0:~$1)',
    '.nVEn': '⊢~(V$0:$1)<>(E$0:~$1)',
    '.nVVn': '⊢(E$2:(E$0:$1))<>~(V$2:(V$0:~$1))',
    '.nEn': '⊢(V$0:$1)<>~(E$0:~$1)',
    '.Ve': 'V$0:$1⊢$1',
    '.Vnf': '⊢(V$0:#nf($1,$0))<>#nf($1,$0)',
    '.Vs': 'V$0:(V$1:$2)⊢V$1:(V$0:$2)',
    '.Ee': 'E$0:#nf($1,$0)⊢#nf($1,$0)',
    '.Enf': '⊢(E$0:#nf($1,$0))<>#nf($1,$0)',
    '.Ei': '$1⊢E$0:$1',
    '.Es': 'E$0:(E$1:$2)⊢E$1:(E$0:$2)',
    '.EV': 'E$x:(V$y:$0)⊢V$y:(E$x:$0)',
    '.Eirp': '⊢#nf($1,$0)>(E$0:#rp(#nf($1,$0),$2,$0))',
    '.Erp': '⊢#rp($1,$0,$2)>(E$0:$1)',
    '.Emp': 'V$x:($1>$2),E$x:$1⊢E$x:$2',
    '.Vnf>': '⊢(V$0:(#nf($1,$0)>$2))<>(#nf($1,$0)>(V$0:$2))',
    '.V>nf': '⊢(V$0:($1>#nf($2,$0)))<>((E$0:$1)>#nf($2,$0))',
    '.V&1': 'V$0:($1&$2)⊢(V$0:$1)&(V$0:$2)',
    '.V&2': '(V$0:$1)&(V$0:$2)⊢V$0:($1&$2)',
    '.V&': '⊢(V$0:($1&$2))<>((V$0:$1)&(V$0:$2))',
    '.V|nf': '⊢(V$0:($1|#nf($2,$0)))<>((V$0:$1)|#nf($2,$0))',
    '.Vnf|': '⊢(V$0:(#nf($1,$0)|$2))<>(#nf($1,$0)|(V$0:$2))',
    '.V&nf': '⊢(V$0:($1&#nf($2,$0)))<>((V$0:$1)&#nf($2,$0))',
    '.Vnf&': '⊢(V$0:(#nf($1,$0)&$2))<>(#nf($1,$0)&(V$0:$2))',
    '.E>nf': '⊢(E$0:($1>#nf($2,$0)))<>((V$0:$1)>#nf($2,$0))',
    '.Enf>': '⊢(E$0:(#nf($1,$0)>$2))<>(#nf($1,$0)>(E$0:$2))',
    '.E|1': 'E$0:($1|$2)⊢(E$0:$1)|(E$0:$2)',
    '.E|2': '(E$0:$1)|(E$0:$2)⊢E$0:($1|$2)',
    '.E|': '⊢(E$0:($1|$2))<>((E$0:$1)|(E$0:$2))',
    '.E|nf': '⊢(E$0:($1|#nf($2,$0)))<>((E$0:$1)|#nf($2,$0))',
    '.Enf|': '⊢(E$0:(#nf($1,$0)|$2))<>(#nf($1,$0)|(E$0:$2))',
    '.E&nf': '⊢(E$0:($1&#nf($2,$0)))<>((E$0:$1)&#nf($2,$0))',
    '.Enf&': '⊢((E$0:(#nf($1,$0)&$2))<>(#nf($1,$0)&(E$0:$2))))',
    '.Vcn': 'V$x:#nf($1,$z)⊢V$z:#rp(#nf($1,$z),$x,$z)',
    '.Ecn': 'E$x:#nf($1,$z)⊢E$z:#rp(#nf($1,$z),$x,$z)',
    '.E!cn': 'E!$x:#nf($1,$z)⊢E!$z:#rp(#nf($1,$z),$x,$z)',
    '.Vcn<>': '⊢(V$x:#nf($1,$z))<>(V$z:#rp(#nf($1,$z),$x,$z))',
    '.Ecn<>': '⊢(E$x:#nf($1,$z))<>(E$z:#rp(#nf($1,$z),$x,$z))',
    '.E!cn<>': '⊢(E!$x:#nf($1,$z))<>(E!$z:#rp(#nf($1,$z),$x,$z))',
    '.{|@}cn': '⊢{#nf($0,$z)|#nf($x,$z)@#nf($y,$x,$z)}={#rp(#nf($0,$z),#nf($x,$z),#nf($z,$x))|#nf($z,$x)@#nf($y,$x,$z)}',
    '.{@|}cn': '⊢{#nf($x,$z)@#nf($y,$x,$z)|#nf($0,$z)}={#nf($z,$x)@#nf($y,$x,$z)|#rp(#nf($0,$z),#nf($x,$z),#nf($z,$x))}',
    '.=s': '$0=$1⊢$1=$0',
    '.=t': '$0=$1,$1=$2⊢$0=$2',
    '.=r+': '$a1=$a2,$b1=$b2⊢($a1+$b1)=($a2+$b2)',
    '.=r*': '$a1=$a2,$b1=$b2⊢($a1*$b1)=($a2*$b2)',
    '.=r=': '$a1=$a2,$b1=$b2⊢($a1=$b1)<>($a2=$b2)',
    '.=r@': '$a1=$a2,$b1=$b2⊢($a1@$b1)<>($a2@$b2)',
    '.=r<': '$a1=$a2,$b1=$b2⊢($a1<$b1)<>($a2<$b2)',
    '.=r()': '$a1=$a2,$b1=$b2⊢($a1,$b1)=($a2,$b2)',
    '.E!': '⊢(E!#nf($0,$2):#nf($1,$2))<>(E#nf($2,$0):(V#nf($0,$2):(#nf($1,$2)<>(#nf($2,$0)=#nf($0,$2)))))',
    'apn1': '⊢0@N',
    'apn2': '⊢Vx:((x@N)>(S(x)@N))',
    'apn3': '⊢Vx:~(S(x)=0)',
    'apn4': '⊢Vx:(Vy:((S(x)=S(y))>(x=y)))',
    'apn5': '⊢#rp(#nf($0,x),$1,0)>((Vx:((x@N)>(#rp(#nf($0,x),$1,x)>#rp(#nf($0,x),$1,S(x)))))>(Vx:((x@N)>#rp(#nf($0,x),$1,x))))',
    'd1': '⊢1=S(0)',
    'd2': '⊢2=S(1)',
    'd3': '⊢3=S(2)',
    'd4': '⊢4=S(3)',
    'd5': '⊢5=S(4)',
    'd6': '⊢6=S(5)',
    'd7': '⊢7=S(6)',
    'd8': '⊢8=S(7)',
    'd9': '⊢9=S(8)',
    'd10': '⊢10=S(9)',
    'd+1': '⊢($x@N)>(($x+0)=$x)',
    'd+2': '⊢($x@N)>(($y@N)>(($x+S($y))=S(($x+$y))))',
    'd*1': '⊢($x@N)>(($x*0)=0)',
    'd*2': '⊢($x@N)>(($y@N)>(($x*S($y))=(($x*$y)+$x)))',
    'd<=': '⊢Vx:(Vy:((x@N)>((y@N)>((x<=y)<>(Ez:((z@N)&((x+z)=y)))))))',
    'd>=': '⊢($x>=$y)<>($y<=$x)',
    'd/|': '⊢Vx:(Vy:((x@N)>((y@N)>((x/|y)<>(Ez:((z@N)&((x*z)=y)))))))',
    'dPrime': '⊢Vn:(Prime(n)<>((n@N)&~(Ex:((((x@N)&~(x=1))&~(x=n))&(Ey:((y@N)&((x*y)=n)))))))',
    '.=Se': '⊢(S($x)=S($y))>($x=$y)',
    '.=Si': '⊢($0=$1)>(S($0)=S($1))',
    '.=S': '⊢($0=$1)<>(S($0)=S($1))',
    '.pred': '⊢Vx:((x@N)>(~(x=0)>(Ez:((x=S(z))&(z@N)))))',
    '.0+': '⊢($0@N)>((0+$0)=$0)',
    '.S+': '⊢($0@N)>(($1@N)>((S($0)+$1)=S(($0+$1))))',
    '.+@N': '⊢($x@N)>(($y@N)>(($x+$y)@N))',
    '.+s': '⊢($x@N)>(($y@N)>(($x+$y)=($y+$x)))',
    '.+a': '⊢($x@N)>(($y@N)>(($z@N)>((($x+$y)+$z)=($x+($y+$z)))))',
    '.+e': '⊢($a@N)>(($b@N)>(($c@N)>((($a+$c)=($b+$c))>($a=$b))))',
    '.+=0': '⊢($x@N)>(($y@N)>((($x+$y)=0)>(($x=0)&($y=0))))',
    '.<=i': '⊢($x@N)>($x<=$x)',
    '.<=s': '⊢($x@N)>(($y@N)>(($x<=$y)>(($y<=$x)>($x=$y))))',
    '.<=t': '$x@N,$y@N,$z@N,$x<=$y,$y<=$z⊢$x<=$z',
    '.<=+': '⊢($x@N)>(($y@N)>($x<=($x+$y)))',
    '.+n<=': '⊢($x@N)>(($y@N)>~(($x+S($y))<=$x))',
    '.<=|>=': '⊢($x@N)>(($y@N)>(($x<=$y)|($y<=$x)))',
    '.0*': '⊢($x@N)>((0*$x)=0)',
    '.*@N': '⊢($x@N)>(($y@N)>(($x*$y)@N))',
    '.S*': '⊢($x@N)>(($y@N)>((S($x)*$y)=(($x*$y)+$y)))',
    '.*s': '⊢($x@N)>(($y@N)>(($x*$y)=($y*$x)))',
    '.*+d': '⊢($x@N)>(($y@N)>(($z@N)>(($x*($y+$z))=(($x*$y)+($x*$z)))))',
    '.+*d': '⊢($x@N)>(($y@N)>(($z@N)>((($x+$y)*$z)=(($x*$z)+($y*$z)))))',
    '.*a': '⊢($x@N)>(($y@N)>(($z@N)>((($x*$y)*$z)=($x*($y*$z)))))',
    '.*=0': '⊢($x@N)>(($y@N)>((($x*S($y))=0)>($x=0)))',
    '.*n=0': '⊢($x@N)>(($y@N)>(~($x=0)>(~($y=0)>~(($x*$y)=0))))',
    '.*e': '⊢($x@N)>(($y@N)>(($z@N)>((($x*S($z))=($y*S($z)))>($x=$y))))',
    'aExt': '⊢Vx:(Vy:((Vz:~(((z@x)>(z@y))>~((z@y)>(z@x))))>(x=y)))',
    'aSep': '⊢V#nf($x,$y,$z):~(V#nf($y,$x,$z):~(V#nf($z,$x,$y):~(((#nf($z,$x,$y)@#nf($y,$x,$z))>~((#nf($z,$x,$y)@#nf($x,$y,$z))>~#nf($0,$y)))>~(~((#nf($z,$x,$y)@#nf($x,$y,$z))>~#nf($0,$y))>(#nf($z,$x,$y)@#nf($y,$x,$z))))))',
    'aPair': '⊢Vx:(Vy:~(Vz:((x@z)>~(y@z))))',
    'aUnion': '⊢Vx:~(Va:~(Vy:(Vz:((z@y)>((y@x)>(z@a))))))',
    'aPow': '⊢Vx:~(Vy:~(Vz:((Va:((a@z)>(a@x)))>(z@y))))',
    'aRepl': '⊢(V#nf($x,$y,$z):~(V#nf($y,$x,$z):~(V#nf($z,$x,$y):(#nf($0,$y)>(#nf($z,$x,$y)=#nf($y,$x,$z))))))>(V#nf($y,$x,$z):~(V#nf($x,$y,$z):~(V#nf($z,$x,$y):~(((#nf($z,$x,$y)@#nf($x,$y,$z))>~(V#nf($x,$y,$z):((#nf($x,$y,$z)@#nf($y,$x,$z))>~#nf($0,$y))))>~(~(V#nf($x,$y,$z):((#nf($x,$y,$z)@#nf($y,$x,$z))>~#nf($0,$y)))>(#nf($z,$x,$y)@#nf($x,$y,$z)))))))',
    'aInf': '⊢~(Vx:((Vy:((Vz:~(z@y))>(y@x)))>~(Vy:((y@x)>(Vz:((Va:~(((a@z)>(~(a@y)>(a=y)))>~((~(a@y)>(a=y))>(a@z))))>(z@x)))))))',
    'aReg': '⊢Vx:(~(Va:~(a@x))>~(Vy:((y@x)>~(Vz:((z@y)>~(z@x))))))',
    'aChoice': '⊢Vx:~(Vy:~(Vz:(Vw:(~((z@w)>~(w@x))>~(Vv:~(Vu:~((~(Vt:(~((u@w)>~(w@t))>((u@t)>~(t@y))))>(u=v))>~((u=v)>~(Vt:(~((u@w)>~(w@t))>((u@t)>~(t@y))))))))))))',
    'd{}': '⊢Vx:~(x@{})',
    'd{.}': '⊢($1@{$2})<>($1=$2)',
    'd{..}': '⊢($1@{$2,$3})<>(($1=$2)|($1=$3))',
    'd()': '⊢($1,$2)={{$1},{$1,$2}}',
    'd()pr': '⊢(E$x:(E$y:($0=($x,$y))))>($0=(Pr1($0),Pr2($0)))',
    'd<': '⊢(#nf($a,$0)<#nf($b,$0))<>(V$0:(($0@#nf($a,$0))>($0@#nf($b,$0))))',
    'dUnion': '⊢(#nf($z,$0)@Union(#nf($x,$0)))<>(E$0:(($0@#nf($x,$0))&(#nf($z,$0)@$0)))',
    'dU': '⊢($z@($xU$y))<>(($z@$x)|($z@$y))',
    'dI': '⊢($z@($xI$y))<>(($z@$x)&($z@$y))',
    'dPow': '⊢($z@Pow($x))<>($z<$x)',
    'd{@|}': '⊢($x@{$x@#nf($y,$x)|$z})<>(($x@#nf($y,$x))&$z)',
    'd{|@}': '⊢(#nf($w,$x)@{$z|$x@#nf($y,$x)})<>(E$x:(($x@#nf($y,$x))&($z=#nf($w,$x))))',
    'd\\': '⊢(#nf($a,$c)\\#nf($b,$c))={$c@#nf($a,$c)|~($c@#nf($b,$c))}',
    'dX': '⊢(#nf($a,$m,$n)X#nf($b,$m,$n))=Union({{(#nf($m,$n),#nf($n,$m))|#nf($n,$m)@#nf($b,$m,$n)}|#nf($m,$n)@#nf($a,$m,$n)})',
    'domega': '⊢Vn:((n@omega)<>(Vx:((({}@x)&(Vy:((y@x)>((yU{y})@x))))>(n@x))))',
    'dRel': '⊢Va:(Vb:(Vr:(Rel(a,r,b)<>(Ef:((f@r)&(f=(a,b)))))))',
    'dEquiv': '⊢Va:(Vb:(Equiv(a,b)<>(Er:((Vx:((x@a)>(E!y:((y@b)&Rel(x,r,y)))))&(Vy:((y@b)>(E!x:((x@a)&Rel(x,r,y)))))))))',
    'dOrder': '⊢Vx:(Vr:(Order(x,r)<>((Va:(Vb:(((a@x)&(b@x))>((Rel(a,r,b)|Rel(b,r,a))&((Rel(a,r,b)&Rel(b,r,a))<>(a=b))))))&(Va:(Vb:(Vc:((((a@x)&(b@x))&(c@x))>((Rel(a,r,b)&Rel(b,r,c))>Rel(a,r,c)))))))))',
    'dWOrder': '⊢Vx:(Vr:(WellOrder(x,r)<>(Order(x,r)&(Vs:(((s<x)&~(s={}))>(Em:((m@s)&(Vn:((n@s)>Rel(m,r,n))))))))))',
    '.aext': '⊢Vx:(Vy:((Vz:((z@x)<>(z@y)))>(x=y)))',
    '.asep': '⊢V#nf($x,$y,$z):(E#nf($y,$x,$z):(V#nf($z,$x,$y):((#nf($z,$x,$y)@#nf($y,$x,$z))<>((#nf($z,$x,$y)@#nf($x,$y,$z))&#nf($0,$y)))))',
    '.apair': '⊢Vx:(Vy:(Ez:((x@z)&(y@z))))',
    '.aunion': '⊢Vx:(Ea:(Vy:(Vz:((z@y)>((y@x)>(z@a))))))',
    '.apow': '⊢Vx:(Ey:(Vz:((z<x)>(z@y))))',
    '.arepl': '⊢(V#nf($x,$y,$z):(E#nf($y,$x,$z):(V#nf($z,$x,$y):(#nf($0,$y)>(#nf($z,$x,$y)=#nf($y,$x,$z))))))>(V#nf($y,$x,$z):(E#nf($x,$y,$z):(V#nf($z,$x,$y):((#nf($z,$x,$y)@#nf($x,$y,$z))<>(E#nf($x,$y,$z):((#nf($x,$y,$z)@#nf($y,$x,$z))&#nf($0,$y)))))))',
    '.ainf': '⊢Ex:(({}@x)&(Vy:((y@x)>((yU{y})@x))))',
    '.areg': '⊢Vx:(~(x={})>(Ey:((y@x)&((yIx)={}))))',
    '.achoice': '⊢Vx:(Ey:(Vz:(Vw:(((z@w)&(w@x))>(Ev:(Vu:((Et:(((u@w)&(w@t))&((u@t)&(t@y))))<>(u=v))))))))',
    '.<i': '⊢$0<$0',
    '.<t': '$0<$1,$1<$2⊢$0<$2',
    '.ext': '⊢(V$z:(($z@#nf($x,$z))<>($z@#nf($y,$z))))>(#nf($x,$z)=#nf($y,$z))',
    '.=<': '⊢(($x<$y)&($y<$x))>($x=$y)',
    '.={}': '⊢Vx:((Vy:~(y@x))>(x={}))',
    '.=n{}': '$a@$b⊢~($b={})',
    '.filter': 'E#nf($x,$z):(V#nf($z,$x):(#nf($0,$x)>(#nf($z,$x)@#nf($x,$z))))⊢E!#nf($x,$z):(V#nf($z,$x):((#nf($z,$x)@#nf($x,$z))<>#nf($0,$x)))',
    '.{.}=e': '⊢({$1}={$2})<>($1=$2)',
    '.{..}s': '⊢{$0,$1}={$1,$0}',
    '.{..}1': '⊢$0@{$0,$1}',
    '.{..}2': '⊢$1@{$0,$1}',
    '.{.=..}': '{$a}={$b,$c}⊢($a=$b)&($a=$c)',
    '.{..}i': '⊢{$0}={$0,$0}',
    '.{..}t': '{$a,$b}={$b,$c}⊢$a=$c',
    '.()1': '($a1,$b1)=($a2,$b2)⊢$a1=$a2',
    '.()2': '($a1,$b1)=($a2,$b2)⊢$b1=$b2',
    '.()=': '⊢(($a1,$b1)=($a2,$b2))<>(($a1=$a2)&($b1=$b2))',
    '.Xe': '($x,$y)@($aX$b)⊢($x@$a)&($y@$b)',
    '.Xi': '$x@$a,$y@$b⊢($x,$y)@($aX$b)',
    '.Pr1': '⊢$x=Pr1(($x,$y))',
    '.Pr2': '⊢$y=Pr2(($x,$y))',
    '.X': '⊢($x@($aX$b))<>(($x=(Pr1($x),Pr2($x)))&((Pr1($x)@$a)&(Pr2($x)@$b)))',
    '.pn1': '⊢{}@omega',
    '.pn2': '⊢Vn:((n@omega)>((nU{n})@omega))',
    '.pn3': '⊢Vx:~((xU{x})={})',
    '.pn4': '⊢Vx:(Vy:(((xU{x})=(yU{y}))>(x=y)))',
    '.pn5': '⊢#rp(#nf($0,x),$1,{})>((Vx:((x@omega)>(#rp(#nf($0,x),$1,x)>#rp(#nf($0,x),$1,(xU{x})))))>(Vx:((x@omega)>#rp(#nf($0,x),$1,x))))',
    'd0': '⊢0={}',
    'dS': '⊢S($x)=($xU{$x})',
    'dN': '⊢N=omega',
    '.N@+': '⊢($x@N)>(($y@N)>($x@($x+S($y))))',
    '.N@t': '⊢($x@N)>(($y@$x)>($y@N))',
    '.N@<=': '⊢($x@N)>(($y@$x)>($y<=$x))',
    '.<=Rel': '⊢((($x@N)&($y@N))&($x<=$y))<>Rel($x,{x@(NXN)|(Pr1(x)<=Pr2(x))},$y)',
    '.<=Order': '⊢Order(N,{x@(NXN)|(Pr1(x)<=Pr2(x))})',
    'dfZ': '⊢Vx:((x@(NXN))>(Z(x)={y@(NXN)|((Pr1(x)+Pr2(y))=(Pr2(x)+Pr1(y)))}))',
    'dZ': '⊢Z={Z(x)|x@(NXN)}',
    'dZ+': '⊢($x@(NXN))>(($y@(NXN))>((Z($x)+Z($y))=Z(((Pr1($x)+Pr1($y)),(Pr2($x)+Pr2($y))))))',
    'dZ-': '⊢($x@(NXN))>(($y@(NXN))>((Z($x)-Z($y))=Z(((Pr1($x)+Pr2($y)),(Pr2($x)+Pr1($y))))))',
    'dZ*': '⊢($x@(NXN))>(($y@(NXN))>((Z($x)*Z($y))=Z((((Pr1($x)*Pr1($y))+(Pr2($x)*Pr2($y))),((Pr1($x)*Pr2($y))+(Pr2($x)*Pr1($y)))))))',
    'dZ<=': '⊢($x@(NXN))>(($y@(NXN))>((Z($x)<=Z($y))<>((Pr1($x)+Pr2($y))<=(Pr2($x)+Pr1($y)))))',
    'dZ-2': '⊢-2=Z((0,2))',
    'dZ-1': '⊢-1=Z((0,1))',
    'dZ0': '⊢+0=Z((0,0))',
    'dZ1': '⊢+1=Z((1,0))',
    'dZ2': '⊢+2=Z((2,0))',
    '.@Z': '$x@N,$y@N⊢Z(($x,$y))@Z',
    '.Zr': '$x@N,$y@N,$z@N⊢Z(($x,$y))=Z((($x+$z),($y+$z)))',
    '.Z+s': '⊢($x@Z)>(($y@Z)>(($x+$y)=($y+$x)))',
    'dQ-2': '⊢-2q=Q((-2,+1))',
    'dQ-1': '⊢-1q=Q((-1,+1))',
    'dQ0': '⊢0q=Q((+0,+1))',
    'dQ1': '⊢1q=Q((+1,+1))',
    'dQ2': '⊢2q=Q((+2,+1))',
    'd0.5': '⊢0.5=Q((+5,+10))',
    'd-3.14': '⊢-3.14=Q((-314,+100))',
    'd5.': '⊢5.=Q((+5,+1))',
    'dfQ': '⊢Vx:((x@(ZX(Z\\{+0})))>(Q(x)={y@(ZX(Z\\{+0}))|((Pr1(x)*Pr2(y))=(Pr2(x)*Pr1(y)))}))',
    'dQ': '⊢Q={Q(x)|x@(ZX(Z\\{+0}))}',
    'dQ+': '⊢($x@(ZX(Z\\{+0})))>(($y@(ZX(Z\\{+0})))>((Q($x)+Q($y))=Q((((Pr1($x)*Pr2($y))+(Pr1($y)*Pr2($x))),(Pr2($x)*Pr2($y))))))',
    'dQ-': '⊢($x@(ZX(Z\\{+0})))>(($y@(ZX(Z\\{+0})))>((Q($x)-Q($y))=Q((((Pr1($x)*Pr2($y))-(Pr1($y)*Pr2($x))),(Pr2($x)*Pr2($y))))))',
    'dQ*': '⊢($x@(ZX(Z\\{+0})))>(($y@(ZX(Z\\{+0})))>((Q($x)*Q($y))=Q(((Pr1($x)*Pr1($y)),(Pr2($x)*Pr2($y))))))',
    'dQ/': '⊢($x@(ZX(Z\\{+0})))>(($y@((Z\\{+0})X(Z\\{+0})))>((Q($x)/Q($y))=Q(((Pr1($x)*Pr2($y)),(Pr2($x)*Pr1($y))))))',
    'dQ<=': '⊢($x@(ZX(Z\\{+0})))>(($y@(ZX(Z\\{+0})))>(((Pr2($x)*Pr2($y))>=0)>((Q($x)<=Q($y))<>((Pr1($x)*Pr2($y))<=(Pr2($x)*Pr1($y))))))',
    'dR-2': '⊢-2r={x@Q|((x<=-2)&~(x=-2))}',
    'dR-1': '⊢-1r={x@Q|((x<=-1)&~(x=-1))}',
    'dR0': '⊢0r={x@Q|((x<=0)&~(x=0))}',
    'dR1': '⊢1r={x@Q|((x<=1)&~(x=1))}',
    'dR2': '⊢2r={x@Q|((x<=2)&~(x=2))}',
    'dR0.5': '⊢0.5r={x@Q|((x<=0.5)&~(x=0.5))}',
    'd-3.14r': '⊢-3.14r={x@Q|((x<=-3.14)&~(x=-3.14))}',
    'ddkCut': '⊢Va:(DdkCut(a)<>((((~(a={})&~(a=Q))&(((Q\\a)Ua)=Q))&(Vx:(Vy:((x@a)>((y@(Q\\a))>((x<=y)&~(x=y)))))))&~(Ex:((x@a)&(Vy:((y@a)>(x>=y)))))))',
    'dR': '⊢R={a@Pow(Q)|DdkCut(a)}',
    'dR+': '⊢Vx:(Vy:((x@R)>((y@R)>((x+y)={z@Q|(Em:(En:(((m@x)&(n@y))&(z=(m+n)))))}))))',
    'dR-': '⊢Vx:(Vy:((x@R)>((y@R)>((x-y)={z@Q|(Em:(En:(((m@x)&(n@(Q\\y)))&(z=(m-n)))))}))))',
    'dR*1': '⊢Vx:(Vy:((x@R)>((y@R)>((Vm:((m@(Q\\x))>(m>=0q)))>((Vm:((m@(Q\\y))>(m>=0q)))>((x*y)={z@Q|~(Em:(En:(((m@(Q\\x))&(n@(Q\\y)))&(z=(m*n)))))}))))))',
    'dR*2': '⊢Vx:(Vy:((x@R)>((y@R)>(((x*y)=(0r-((0r-x)*y)))&((x*y)=(0r-(x*(0r-y))))))))',
    'dR/1': '⊢Vx:(Vy:((x@R)>((y@R)>((0q@x)>((y/x)=(y*{z@Q|((z>=0)&(Em:((m@x)&(z=(1q/m)))))}))))))',
    'dR/2': '⊢Vx:(Vy:((x@R)>((y@R)>((x/(0r-y))=(0r-(x/y))))))',
    'dR<=': '⊢($x@R)>(($y@R)>(($x<=$y)<>($x<$y)))'
};

/* ========== 推理规则分类（用于搜索结果的辅助简化显示） ========== */

// 每个分类：[分类名, 若干片段]；片段为 [单个键] 或 [起始键, 结束键]（均含端点）
const CATEGORY_DEFS = [
    ['命题逻辑公理与定义', [['mp', 'd|']]],
    ['命题逻辑定理',       [['.i', '.<>r|']]],
    ['一阶逻辑公理与定义', [['a4', 'dE!']]],
    ['一阶逻辑定理',       [['.nEVn', '.E!cn<>'], ['.<>rV'], ['.<>rE'], ['.<>rE!'], ['.=s', '.E!']]],
    ['PA 公理与定义',      [['apn1', 'dPrime']]],
    ['PA 定理',            [['.=Se', '.*e']]],
    ['ZFC 公理与定义',     [['aExt', '.achoice']]],
    ['ZFC 定理',           [['.<i', '.X'], ['.{|@}cn'], ['.{@|}cn']]],
    ['数系公理与定义',     [['d0'], ['dS'], ['dN'], ['dfZ', 'dZ2'], ['dQ-2', 'dR<=']]],
    ['数系定理',           [['.pn1', '.pn5'], ['.N@+', '.<=Order'], ['.@Z'], ['.Zr'], ['.Z+s']]],
];

// 规则名 → 分类名
const RULE_CATEGORY_MAP = new Map();
// 分类名的输出顺序（按 CATEGORY_DEFS 的定义顺序）
const CATEGORY_ORDER = [];

(function buildCategoryMap() {
    const keys = Object.keys(THEOREM_CONTENT_DICT);

    for (const [catName, segments] of CATEGORY_DEFS) {
        CATEGORY_ORDER.push(catName);
        for (const seg of segments) {
            let names;
            if (seg.length === 1) {
                names = keys.includes(seg[0]) ? [seg[0]] : [];
            } else {
                const si = keys.indexOf(seg[0]);
                const ei = keys.indexOf(seg[1]);
                if (si !== -1 && ei !== -1 && si <= ei) {
                    names = keys.slice(si, ei + 1);
                } else {
                    console.warn(`分类范围无效：${seg[0]} ~ ${seg[1]}`);
                    names = [];
                }
            }
            for (const n of names) {
                if (!RULE_CATEGORY_MAP.has(n)) {
                    RULE_CATEGORY_MAP.set(n, catName);
                }
            }
        }
    }
})();

// ==================== Deductrium 2.0 支持 ====================

/* ============================================================
 * LZString（内联精简版，仅 compressToUTF16 / decompressFromUTF16）
 * 与游戏引用的 lz-string 库行为一致
 * ============================================================ */
var LZString = (function () {
    var f = String.fromCharCode;
    var LZString = {
        compressToUTF16: function (input) {
            if (input == null) return "";
            return LZString._compress(input, 15, function (a) { return f(a + 32); }) + " ";
        },
        decompressFromUTF16: function (compressed) {
            if (compressed == null) return "";
            if (compressed == "") return null;
            return LZString._decompress(compressed.length, 16384, function (index) {
                return compressed.charCodeAt(index) - 32;
            });
        },
        _compress: function (uncompressed, bitsPerChar, getCharFromInt) {
            if (uncompressed == null) return "";
            var i, value,
                context_dictionary = {}, context_dictionaryToCreate = {},
                context_c = "", context_wc = "", context_w = "",
                context_enlargeIn = 2, context_dictSize = 3, context_numBits = 2,
                context_data = [], context_data_val = 0, context_data_position = 0, ii;

            for (ii = 0; ii < uncompressed.length; ii += 1) {
                context_c = uncompressed.charAt(ii);
                if (!Object.prototype.hasOwnProperty.call(context_dictionary, context_c)) {
                    context_dictionary[context_c] = context_dictSize++;
                    context_dictionaryToCreate[context_c] = true;
                }
                context_wc = context_w + context_c;
                if (Object.prototype.hasOwnProperty.call(context_dictionary, context_wc)) {
                    context_w = context_wc;
                } else {
                    if (Object.prototype.hasOwnProperty.call(context_dictionaryToCreate, context_w)) {
                        if (context_w.charCodeAt(0) < 256) {
                            for (i = 0; i < context_numBits; i++) {
                                context_data_val = (context_data_val << 1);
                                if (context_data_position == bitsPerChar - 1) {
                                    context_data_position = 0;
                                    context_data.push(getCharFromInt(context_data_val));
                                    context_data_val = 0;
                                } else context_data_position++;
                            }
                            value = context_w.charCodeAt(0);
                            for (i = 0; i < 8; i++) {
                                context_data_val = (context_data_val << 1) | (value & 1);
                                if (context_data_position == bitsPerChar - 1) {
                                    context_data_position = 0;
                                    context_data.push(getCharFromInt(context_data_val));
                                    context_data_val = 0;
                                } else context_data_position++;
                                value = value >> 1;
                            }
                        } else {
                            value = 1;
                            for (i = 0; i < context_numBits; i++) {
                                context_data_val = (context_data_val << 1) | value;
                                if (context_data_position == bitsPerChar - 1) {
                                    context_data_position = 0;
                                    context_data.push(getCharFromInt(context_data_val));
                                    context_data_val = 0;
                                } else context_data_position++;
                                value = 0;
                            }
                            value = context_w.charCodeAt(0);
                            for (i = 0; i < 16; i++) {
                                context_data_val = (context_data_val << 1) | (value & 1);
                                if (context_data_position == bitsPerChar - 1) {
                                    context_data_position = 0;
                                    context_data.push(getCharFromInt(context_data_val));
                                    context_data_val = 0;
                                } else context_data_position++;
                                value = value >> 1;
                            }
                        }
                        context_enlargeIn--;
                        if (context_enlargeIn == 0) { context_enlargeIn = Math.pow(2, context_numBits); context_numBits++; }
                        delete context_dictionaryToCreate[context_w];
                    } else {
                        value = context_dictionary[context_w];
                        for (i = 0; i < context_numBits; i++) {
                            context_data_val = (context_data_val << 1) | (value & 1);
                            if (context_data_position == bitsPerChar - 1) {
                                context_data_position = 0;
                                context_data.push(getCharFromInt(context_data_val));
                                context_data_val = 0;
                            } else context_data_position++;
                            value = value >> 1;
                        }
                    }
                    context_enlargeIn--;
                    if (context_enlargeIn == 0) { context_enlargeIn = Math.pow(2, context_numBits); context_numBits++; }
                    context_dictionary[context_wc] = context_dictSize++;
                    context_w = String(context_c);
                }
            }

            if (context_w !== "") {
                if (Object.prototype.hasOwnProperty.call(context_dictionaryToCreate, context_w)) {
                    if (context_w.charCodeAt(0) < 256) {
                        for (i = 0; i < context_numBits; i++) {
                            context_data_val = (context_data_val << 1);
                            if (context_data_position == bitsPerChar - 1) {
                                context_data_position = 0;
                                context_data.push(getCharFromInt(context_data_val));
                                context_data_val = 0;
                            } else context_data_position++;
                        }
                        value = context_w.charCodeAt(0);
                        for (i = 0; i < 8; i++) {
                            context_data_val = (context_data_val << 1) | (value & 1);
                            if (context_data_position == bitsPerChar - 1) {
                                context_data_position = 0;
                                context_data.push(getCharFromInt(context_data_val));
                                context_data_val = 0;
                            } else context_data_position++;
                            value = value >> 1;
                        }
                    } else {
                        value = 1;
                        for (i = 0; i < context_numBits; i++) {
                            context_data_val = (context_data_val << 1) | value;
                            if (context_data_position == bitsPerChar - 1) {
                                context_data_position = 0;
                                context_data.push(getCharFromInt(context_data_val));
                                context_data_val = 0;
                            } else context_data_position++;
                            value = 0;
                        }
                        value = context_w.charCodeAt(0);
                        for (i = 0; i < 16; i++) {
                            context_data_val = (context_data_val << 1) | (value & 1);
                            if (context_data_position == bitsPerChar - 1) {
                                context_data_position = 0;
                                context_data.push(getCharFromInt(context_data_val));
                                context_data_val = 0;
                            } else context_data_position++;
                            value = value >> 1;
                        }
                    }
                    context_enlargeIn--;
                    if (context_enlargeIn == 0) { context_enlargeIn = Math.pow(2, context_numBits); context_numBits++; }
                    delete context_dictionaryToCreate[context_w];
                } else {
                    value = context_dictionary[context_w];
                    for (i = 0; i < context_numBits; i++) {
                        context_data_val = (context_data_val << 1) | (value & 1);
                        if (context_data_position == bitsPerChar - 1) {
                            context_data_position = 0;
                            context_data.push(getCharFromInt(context_data_val));
                            context_data_val = 0;
                        } else context_data_position++;
                        value = value >> 1;
                    }
                }
                context_enlargeIn--;
                if (context_enlargeIn == 0) { context_enlargeIn = Math.pow(2, context_numBits); context_numBits++; }
            }

            value = 2;
            for (i = 0; i < context_numBits; i++) {
                context_data_val = (context_data_val << 1) | (value & 1);
                if (context_data_position == bitsPerChar - 1) {
                    context_data_position = 0;
                    context_data.push(getCharFromInt(context_data_val));
                    context_data_val = 0;
                } else context_data_position++;
                value = value >> 1;
            }

            while (true) {
                context_data_val = (context_data_val << 1);
                if (context_data_position == bitsPerChar - 1) {
                    context_data.push(getCharFromInt(context_data_val));
                    break;
                } else context_data_position++;
            }
            return context_data.join('');
        },
        _decompress: function (length, resetValue, getNextValue) {
            var dictionary = [], next, enlargeIn = 4, dictSize = 4, numBits = 3,
                entry = "", result = [], i, w, bits, resb, maxpower, power, c,
                data = { val: getNextValue(0), position: resetValue, index: 1 };

            for (i = 0; i < 3; i += 1) dictionary[i] = i;

            bits = 0; maxpower = Math.pow(2, 2); power = 1;
            while (power != maxpower) {
                resb = data.val & data.position;
                data.position >>= 1;
                if (data.position == 0) { data.position = resetValue; data.val = getNextValue(data.index++); }
                bits |= (resb > 0 ? 1 : 0) * power;
                power <<= 1;
            }

            switch (next = bits) {
                case 0:
                    bits = 0; maxpower = Math.pow(2, 8); power = 1;
                    while (power != maxpower) {
                        resb = data.val & data.position;
                        data.position >>= 1;
                        if (data.position == 0) { data.position = resetValue; data.val = getNextValue(data.index++); }
                        bits |= (resb > 0 ? 1 : 0) * power;
                        power <<= 1;
                    }
                    c = f(bits); break;
                case 1:
                    bits = 0; maxpower = Math.pow(2, 16); power = 1;
                    while (power != maxpower) {
                        resb = data.val & data.position;
                        data.position >>= 1;
                        if (data.position == 0) { data.position = resetValue; data.val = getNextValue(data.index++); }
                        bits |= (resb > 0 ? 1 : 0) * power;
                        power <<= 1;
                    }
                    c = f(bits); break;
                case 2:
                    return "";
            }
            dictionary[3] = c;
            w = c;
            result.push(c);

            while (true) {
                if (data.index > length) return "";

                bits = 0; maxpower = Math.pow(2, numBits); power = 1;
                while (power != maxpower) {
                    resb = data.val & data.position;
                    data.position >>= 1;
                    if (data.position == 0) { data.position = resetValue; data.val = getNextValue(data.index++); }
                    bits |= (resb > 0 ? 1 : 0) * power;
                    power <<= 1;
                }

                switch (c = bits) {
                    case 0:
                        bits = 0; maxpower = Math.pow(2, 8); power = 1;
                        while (power != maxpower) {
                            resb = data.val & data.position;
                            data.position >>= 1;
                            if (data.position == 0) { data.position = resetValue; data.val = getNextValue(data.index++); }
                            bits |= (resb > 0 ? 1 : 0) * power;
                            power <<= 1;
                        }
                        dictionary[dictSize++] = f(bits);
                        c = dictSize - 1;
                        enlargeIn--;
                        break;
                    case 1:
                        bits = 0; maxpower = Math.pow(2, 16); power = 1;
                        while (power != maxpower) {
                            resb = data.val & data.position;
                            data.position >>= 1;
                            if (data.position == 0) { data.position = resetValue; data.val = getNextValue(data.index++); }
                            bits |= (resb > 0 ? 1 : 0) * power;
                            power <<= 1;
                        }
                        dictionary[dictSize++] = f(bits);
                        c = dictSize - 1;
                        enlargeIn--;
                        break;
                    case 2:
                        return result.join('');
                }

                if (enlargeIn == 0) { enlargeIn = Math.pow(2, numBits); numBits++; }

                if (dictionary[c]) {
                    entry = dictionary[c];
                } else {
                    if (c === dictSize) entry = w + w.charAt(0);
                    else return null;
                }
                result.push(entry);

                dictionary[dictSize++] = w + entry.charAt(0);
                enlargeIn--;
                w = entry;

                if (enlargeIn == 0) { enlargeIn = Math.pow(2, numBits); numBits++; }
            }
        }
    };
    return LZString;
})();

// D2 解压：优先原样，失败时依次尝试补回空格、忽略尾部换行
function tryDecompressD2(raw) {
    let out = LZString.decompressFromUTF16(raw);
    if (out !== null && out !== '') return { text: out, note: '' };

    if (!raw.endsWith(' ')) {
        out = LZString.decompressFromUTF16(raw + ' ');
        if (out !== null && out !== '') return { text: out, note: '（已自动补回末尾空格）' };
    }
    const noTail = raw.replace(/[\r\n\t]+$/, '');
    if (noTail !== raw) {
        out = LZString.decompressFromUTF16(noTail);
        if (out !== null && out !== '') return { text: out, note: '（已忽略尾部换行）' };
        if (!noTail.endsWith(' ')) {
            out = LZString.decompressFromUTF16(noTail + ' ');
            if (out !== null && out !== '') return { text: out, note: '（已忽略尾部换行并补回空格）' };
        }
    }
    throw new Error('解压失败：不是有效的 LZString UTF-16 数据');
}

// D1 专属功能的守卫
function requireDdt1(featureName) {
    if (versionSelect.value !== 'ddt1') {
        showError(`“${featureName}”仅适用于 Deductrium 1，当前版本为 Deductrium 2.0`);
        return false;
    }
    return true;
}

// ==================== 基础映射表（与游戏一致） ====================

const splittor = "-(=)-";
const dict = {
    ',"aE0","aPair","aPow","aUnion","areg","arepl","asep","ainf",': "a#`",
    '"0","1","2","3","4","5","6","7","8","9","10"': "b#`",
    '"Union","Pow","S"': "c#`",
    ',"apn1","apn2","apn3","d1","d2","d3","d4","d5","d6","d7","d8","d9",': "d#`",
    '","2,2,4,2,': '@`',
    '3,3,3,3,': '3`',
    '2,2,2,2,2,2,': '6`',
    '1,1,1,1,1,1,': '1`',
    '2,2,2,1,2,': '5`',
    '素食主义者（累计获40µg推理素）': '4#`',
    '你推出你，他推出他（⊢$0>$0）': '2#`',
    '我推出你，你推出我（<>）': '=#`',
    '我给且只给所有不自己理发的人理发': 'Q#`',
    '会跑的“⊢”（演绎元定理）': '3#`',
    '第一次消费': '1#`',
    '[["progL","[ach]解锁了成就",': 'h`',
    '","[ach]': '2`',
    '","录制*",[["': '*`',
    ',["mp",[': 'm`',
    '],[]]]],"': '[`',
    '",[],["': '{`',
    '","a': 'a`',
    '","d': 'd`',
    '],[]],["': ']`',
    '",[-1],[': '-`',
    '","元规则生成*",[["': 'M`',
    '5`3,1,2,3,2,': ',`',
    '"]],["': '.`',
    ')<>(': '=`',
    '.<>': 'q`',
    '",[-1]`': 'z`',
};

const replaceArr1 = Object.entries(dict);           // 编码正向
const replaceArr2 = replaceArr1.slice(0).reverse(); // 解码反向

// ==================== 随机数生成器 ====================

class Rnd {
    constructor(x) { this.x = x >>> 0; }
    next() {
        this.x = ((this.x + 0x19da44d9) + (this.x << 8)) >>> 0;
        this.x = (this.x ^ (this.x >>> 4)) >>> 0;
        this.x = ((this.x * 0xb5502e5) + (this.x >>> 0)) >>> 0;
        return (this.x >>> 0) / 0x100000000;
    }
}

// ==================== 洗牌与中文处理 ====================

class Shuffle {
    static shuffleArray(array, randomFunction) {
        for (let i = array.length - 1; i > 0; i--) {
            const j = Math.floor(randomFunction.next() * (i + 1));
            [array[i], array[j]] = [array[j], array[i]];
        }
        return array;
    }
    static shuffleArrayReverse(array, randomFunction) {
        const nums = new Array(array.length).fill(0).map((_, i) => i);
        Shuffle.shuffleArray(nums, randomFunction);
        const res = new Array(array.length);
        for (const [i, j] of nums.entries()) {
            res[j] = array[i];
        }
        return res.join("");
    }
    static replaceZh(a) {
        let r = "";
        for (let i = 0; i < a.length; i++) {
            if (a.charCodeAt(i) > 0xFF) {
                r += "U`" + a.charCodeAt(i).toString(36) + ".";
            } else {
                r += a[i];
            }
        }
        return r;
    }
    static recoverZh(a) {
        let r = a[0];
        let code = "?";
        for (let i = 1; i < a.length; i++) {
            if (code !== "?") {
                if (a[i] !== ".") {
                    code += a[i];
                } else {
                    r = r.slice(0, -1);
                    r += String.fromCharCode(Number.parseInt(code, 36));
                    code = "?";
                }
            } else if (a[i] === "`" && a[i - 1] === "U") {
                code = "";
            } else {
                r += a[i];
            }
        }
        return r;
    }
}

// ==================== 核心编码/解码函数 ====================

function encodeProgressDoc(jsonStr, mode = 'survival') {
    let processed = jsonStr;
    for (const [a, b] of replaceArr1) {
        processed = processed.replaceAll(a, b);
    }
    processed = Shuffle.replaceZh(processed);
    const seed = (mode === 'survival') ? processed.length : (processed.length - 1);
    const rnd = new Rnd(seed);
    const shuffled = Shuffle.shuffleArray(Array.from(processed), rnd);
    return shuffled.join("");
}

function decodeProgressDoc(encodedStr, mode = 'survival') {
    const seed = (mode === 'survival') ? encodedStr.length : (encodedStr.length - 1);
    const rnd = new Rnd(seed);
    const unshuffled = Shuffle.shuffleArrayReverse(Array.from(encodedStr), rnd);
    const recoveredZh = Shuffle.recoverZh(unshuffled);
    let result = recoveredZh;
    for (const [a, b] of replaceArr2) {
        result = result.replaceAll(b, a);
    }
    return result;
}

// ==================== DOM 交互逻辑 ====================

const encodedTextarea = document.getElementById('encodedText');
const jsonTextarea = document.getElementById('jsonText');
const proofTextarea = document.getElementById('proofText');
const versionSelect = document.getElementById('versionSelect');
const modeSelect = document.getElementById('modeSelect');
const fileInput = document.getElementById('fileInput');
const btnDecode = document.getElementById('btnDecode');
const btnEncode = document.getElementById('btnEncode');
const btnExtractProofs = document.getElementById('btnExtractProofs');
const btnExtractTypeProofs = document.getElementById('btnExtractTypeProofs');
const btnShowArrangement = document.getElementById('btnShowArrangement');
const btnClear = document.getElementById('btnClear');
const btnDownload = document.getElementById('btnDownload');
const errorMsg = document.getElementById('errorMsg');
const infoMsg = document.getElementById('infoMsg');
const ruleSearchInput = document.getElementById('ruleSearchInput');
const ruleSearchBox   = document.getElementById('ruleSearchBox');
const btnSearchRule   = document.getElementById('btnSearchRule');

// 显示错误
function showError(message) {
    errorMsg.textContent = message;
    errorMsg.style.display = 'block';
    infoMsg.style.display = 'none';
}

// 显示信息
function showInfo(message, warn) {
    infoMsg.textContent = message;
    infoMsg.style.display = 'block';
    errorMsg.style.display = 'none';
    infoMsg.classList.toggle('warn', !!warn);
}

// 清除消息
function clearMessages() {
    errorMsg.style.display = 'none';
    infoMsg.style.display = 'none';
}

/* ========== 证明 / 排列表 选项 ========== */

const DEFAULT_CUSTOM_RULES    = true;   // 在证明项中加入自定义推理规则
const DEFAULT_CONTENT_DISPLAY = true;   // 在排列表中加入定理内容显示
const DEFAULT_DEPENDENT_RULES = false;   // 在证明项中加入依赖推理规则

const ProofOptionIds = {
    customRules:    'chkCustomRules',
    contentDisplay: 'chkContentDisplay',
    dependentRules: 'chkDependentRules'
};

const proofOptions = {
    customRules:    DEFAULT_CUSTOM_RULES,
    contentDisplay: DEFAULT_CONTENT_DISPLAY,
    dependentRules: DEFAULT_DEPENDENT_RULES
};

function initProofOptions() {
    for (const [key, id] of Object.entries(ProofOptionIds)) {
        const el = document.getElementById(id);
        if (!el) continue; // 复选框缺失 → 保留常量默认值
        el.checked = proofOptions[key]; 
        proofOptions[key] = !!el.checked;
        el.addEventListener('change', () => {
            proofOptions[key] = !!el.checked;
            onProofOptionChange(key, !!el.checked);
        });
    }
}

function getProofOptions() {
    return { ...proofOptions };
}

function isProofOptionEnabled(key) {
    return !!proofOptions[key];
}

function onProofOptionChange(key, checked) {
    // 清空上一次的证明项结果，避免残留与当前选项不一致的内容
    const proofText = document.getElementById('proofText');
    if (proofText) proofText.value = '';
    if (typeof clearMessages === 'function') clearMessages();
}

// ==================== 解码结果分段 ====================

/*
    解码后的整体结构：
    [第一部分]-(=)-[第二部分]-(=)-[第三部分]-(=)-[第四部分]

    第一部分：[[成就与门],推理素数量,已使用推理素数量,拆门数,步数,[最大已达到序数的PrSS表示],序数进位基底,(未知，可选项)]
    第二部分：2,2,2,1,...:0.5898,...:1
    第三部分：[[常量定义表],[函数定义表],[谓词定义表],[[],元规则表],{推理规则表},[排列表],[]]
    第四部分：[类型层证明项表]
*/

function splitDecodedParts(decoded) {
    const parts = decoded.split(splittor);
    if (parts.length < 4) {
        throw new Error(
            `解码结果格式错误：期望以 "${splittor}" 分隔成 4 个部分，实际得到 ${parts.length} 个`
        );
    }
    return parts;
}

// 解析第三部分（修正：7 个元素，索引左移）
function parseSection3(str) {
    let obj;
    try {
        obj = JSON.parse(str);
    } catch (e) {
        throw new Error('第三部分不是合法 JSON：' + e.message);
    }
    if (!Array.isArray(obj)) throw new Error('第三部分不是数组');

    // 结构：[[常量],[函数],[谓词],[[],元规则],{推理规则},[排列表],[]]
    //  索引：   0     1     2        3          4          5      6
    return {
        constTable:  obj[0] || [],   // 常量定义表
        funcTable:   obj[1] || [],   // 函数定义表
        predTable:   obj[2] || [],   // 谓词定义表
        metaRules:   obj[3] || [],   // [[], 元规则表]
        rules:       obj[4] || {},   // 推理规则表   ← 原为 obj[5]
        arrangement: obj[5] || [],   // 排列表       ← 原为 obj[6]
        extra:       obj[6] || []    // 末尾空表
    };
}

// 读取解码文本并取出第三部分
function getSection3() {
    const decoded = jsonTextarea.value.trim();
    if (!decoded) throw new Error('请先解码或粘贴解码后的文本到文本框');
    const parts = splitDecodedParts(decoded);
    return parseSection3(parts[2].trim());
}

// 读取解码文本并取出第四部分原始字符串
function getSection4Raw() {
    const decoded = jsonTextarea.value.trim();
    if (!decoded) throw new Error('请先解码或粘贴解码后的文本到文本框');
    const parts = splitDecodedParts(decoded);
    return parts[3].trim();
}

// ==================== 证明项提取功能 ====================

// 判断一个值是否是单个定理定义：[sequent, tag, steps, ...]
// 只要求前 3 项有效，后面的项忽略
function isTheoremDef(v) {
    return Array.isArray(v) && v.length >= 3
        && typeof v[0] === 'string'      // sequent
        && typeof v[1] === 'string'      // tag
        && Array.isArray(v[2]);          // steps
}

// 只按顶层逗号切分：圆括号 () 和花括号 {} 内的逗号不分割
function splitTopLevelCommas(s) {
    const parts = [];
    let depth = 0;
    let start = 0;
    for (let i = 0; i < s.length; i++) {
        const c = s[i];
        if (c === '(' || c === '{') depth++;
        else if (c === ')' || c === '}') depth--;
        else if (c === ',' && depth === 0) {
            parts.push(s.slice(start, i));
            start = i + 1;
        }
    }
    parts.push(s.slice(start));
    return parts;
}

/* ============================================================
   ★ 新增：规则顺序表缓存
   ------------------------------------------------------------
   数据来源：第三部分的排列表（sec3.arrangement）
   规则：剔除所有以 "< f >" 开头的文件夹项，剩下的按原顺序组成数组
        （注意 "< f >" 含空格，与 parseFolderItem / buildArrangementTree
          中的判定保持一致，共 5 个字符）
   构建时机：第一次被 getOrder() 调用时构建，之后直接复用
   失效时机：见下方 decodeAction / btnClear / jsonTextarea input
   ============================================================ */
const RuleOrderStore = (() => {
    let order = null;   // string[] | null   规则名顺序表

    function build() {
        // 1. 取出排列表（getSection3 会解析解码文本的第三部分）
        const sec3  = getSection3();
        const items = Array.isArray(sec3.arrangement) ? sec3.arrangement : [];

        // 2. 过滤掉所有文件夹项，保留规则名，维持原顺序
        const list = [];
        for (const item of items) {
            if (typeof item !== 'string') continue;    // 非字符串项直接跳过
            if (item.startsWith('< f >')) continue;    // "< f >" 开头 = 文件夹，跳过
            list.push(item);
        }

        order = list;
    }

    return {
        // 取规则顺序表（首次调用时构建）
        getOrder() {
            if (order === null) build();
            return order;
        },
        // 令缓存失效（解码文本变化后调用）
        invalidate() {
            order = null;
        }
    };
})();

/* ============================================================
   ★ 改写：通过 steps 分析该定理依赖的推理规则
   规则顺序表由 RuleOrderStore 统一提供，不在本函数内重复解析
   ============================================================ */
const extractAtoms = (() => {
    const UNARY_OPS = new Set(['<', '>', 'c', 'v', 'u', 'e']);
    const DELIMITERS = new Set([':', ',']);

    function parse(expr) {
        const n = expr.length;

        function _parse(i) {
            if (i >= n) throw new Error('表达式意外结束');
            const ch = expr[i];

            if (UNARY_OPS.has(ch)) {
                const [child, end] = _parse(i + 1);
                return [{ op: ch, children: [child], start: i, end }, end];
            }

            if (ch === ':') {
                const start = i;
                i += 1;
                const children = [];

                while (true) {
                    if (i >= n) throw new Error('冒号后缺少操作数');

                    const [child, next] = _parse(i);
                    children.push(child);
                    i = next;

                    if (i >= n) throw new Error('冒号表达式缺少逗号');

                    if (expr[i] === ':') {
                        i += 1;
                        continue;
                    }

                    if (expr[i] === ',') {
                        i += 1;
                        if (i >= n) throw new Error('逗号后缺少操作数');

                        const [last, end] = _parse(i);
                        children.push(last);
                        return [{ op: ':', children, start, end }, end];
                    }

                    throw new Error("冒号后需要 ':' 或 ','");
                }
            }

            if (ch === ',') throw new Error('意外的逗号');

            let j = i;
            while (j < n && !DELIMITERS.has(expr[j])) j += 1;
            if (j === i) throw new Error(`无法识别的字符: ${ch}`);

            return [{ op: expr.slice(i, j), children: [], start: i, end: j }, j];
        }

        const [root, end] = _parse(0);
        if (end !== n) {
            throw new Error(`表达式解析到位置 ${end} 后仍有剩余字符`);
        }
        return root;
    }

    function collectAtoms(node) {
        if (node.children.length === 0) return [node.op];

        const result = [];
        for (const child of node.children) {
            result.push(...collectAtoms(child));
        }
        return result;
    }

    return function extractAtoms(expr) {
        return collectAtoms(parse(expr));
    };
})();

// ===== 辅助方法：处理 unknown 规则 =====
function processUnknownRules(unknown) {
    // 数字出现负数，例如：.1*-2、.-2--3
    const hasNegativeNumber = (s) => /(^|[.@+*\/-])-\d/.test(s);

    // natop 类：d11（数字 > 10）、.1@N、.1+2、.3*4 等
    const isNatop = (s) => {
        const dMatch = s.match(/d(\d+)/);
        if (dMatch && parseInt(dMatch[1], 10) > 10) return true;

        if (/\.\d+@N/.test(s)) return true;
        if (/\.\d+\+\d+/.test(s)) return true;
        if (/\.\d+\*\d+/.test(s)) return true;

        return false;
    };

    // dZn 类：dZ0、dZ1、dZ-1
    const isDZn = (s) => /dZ-?\d+/.test(s);

    let hasNatop = false;
    let hasDZn = false;
    let hasDZop = false;
    const unknownRest = new Set();

    for (const rawName of unknown) {
        // 如果 unknown 中包含 '#'，先去掉
        let name = rawName.includes('#') ? rawName.replace(/#/g, '') : rawName;

        if (!name) continue;

        // natop 类，换成 natop
        if (isNatop(name)) {
            hasNatop = true;
            continue;
        }

        // dZn 类，换成 dZn（需优先于负数判断，避免 dZ-1 被误判）
        if (isDZn(name)) {
            hasDZn = true;
            continue;
        }

        // 负数优先，换成 dZop
        if (hasNegativeNumber(name)) {
            hasDZop = true;
            continue;
        }

        // 排除之后的 unknown
        unknownRest.add(name);
    }

    const result = [];

    // 总顺序：natop, dZn, dZop
    if (hasNatop) result.push('natop');
    if (hasDZn) result.push('dZn');
    if (hasDZop) result.push('dZop');

    // 然后是排除了之后的 unknown，继续源代码逻辑：加 #
    for (const name of unknownRest) {
        result.push(`#${name}`);
    }

    return result;
}

// ===== 目标方法 =====
function getDependentRules(steps) {
    // 首次调用时构建，之后 O(1) 取用；调用方无需关心缓存细节
    const ruleOrder = RuleOrderStore.getOrder();

    // 记录规则在 ruleOrder 中的位置
    const orderIndex = new Map(
        Array.from(ruleOrder, (name, i) => [name, i])
    );

    // 解析每个 step 的 [0]，取并集，保留首次出现顺序
    const seen = new Set();

    for (const step of steps) {
        const expr = step[0];
        if (typeof expr !== 'string') continue;

        const atoms = extractAtoms(expr);
        for (const atom of atoms) {
            seen.add(atom);
        }
    }

    const unknown = [];
    const known = [];

    for (const name of seen) {
        if (orderIndex.has(name)) {
            known.push(name);
        } else {
            unknown.push(name);
        }
    }

    // ruleOrder 中越靠后的，最终越靠前
    known.sort((a, b) => orderIndex.get(b) - orderIndex.get(a));

    // 处理 unknown：natop/dZop 在最前，然后是排除了之后的 unknown（加 #）
    const processedUnknown = processUnknownRules(unknown);

    // 最终结果：处理后的 unknown + 其他原子公式
    return [
        ...processedUnknown,
        ...known,
    ];
}

// 把单个定理转成证明项文本（只使用前 3 项，输出中不含 tag）
function convertTheorem(name, sequent, tag, steps) {
    // 输出首行：名字 + 内容（去掉原来的 tag 部分）
    let output = `${name} ${sequent}\n`;

    // 可选：依赖推理规则
    if (isProofOptionEnabled('dependentRules')) {
        const deps = getDependentRules(steps);
        output += `[依赖：${deps.join(', ')}]\n`;
    }

    let left = '';
    if (sequent.includes('⊢')) {
        left = sequent.split('⊢')[0];
    } else {
        left = sequent;
    }
    // 只按顶层逗号切分前提，避免把有序对 "($a1,$b1)" 或 "{$0,$1}" 拆开
    const hyps = left.trim()
        ? splitTopLevelCommas(left).map(s => s.trim()).filter(s => s.length > 0)
        : [];

    const lines = [];
    for (const h of hyps) {
        lines.push(`hyp ${h}`);
    }

    const base = hyps.length;
    for (let i = 0; i < steps.length; i++) {
        const [rule, refs, vars] = steps[i];
        const currentLine = base + i;
        const args = [];
        for (const r of refs) {
            if (r < 0) {
                args.push(String(currentLine + r));
            } else {
                args.push(String(r));
            }
        }
        if (Array.isArray(vars)) args.push(...vars);
        if (args.length > 0) {
            lines.push(`d ${rule} ${args.join(' ')}`);
        } else {
            lines.push(`d ${rule}`);
        }
    }
    return output + lines.join('\n');
}

// 遍历推理规则表

function extractProofsFromRules(rules) {
    if (!rules || typeof rules !== 'object' || Array.isArray(rules)) {
        throw new Error('推理规则表不是对象');
    }

    const opts = getProofOptions();
    const specials = [];  // 特殊条目：name以dv/df/dc开头，tag为'元规则生成*'，steps为null
    const normals = [];   // 普通定理条目

    for (const [name, value] of Object.entries(rules)) {
        // 优先处理特殊条目（此时 steps 为 null，不满足 isTheoremDef 的数组要求）
        if (
            Array.isArray(value) && value.length >= 3 &&
            typeof value[0] === 'string' &&          // sequent
            typeof value[1] === 'string' &&          // tag
            value[1] === '元规则生成*' &&
            value[2] === null &&
            (name.startsWith('dv') || name.startsWith('df') || name.startsWith('dc'))
        ) {
            // 仅在开启“自定义推理规则”时显示
            if (opts.customRules) {
                specials.push(`${name} ${value[0]}`);
            }
            continue;
        }

        // 普通定理：必须满足 isTheoremDef，且 tag 不是 '元规则生成*'
        if (!isTheoremDef(value)) continue;
        const [sequent, tag, steps] = value;
        if (tag === '元规则生成*') continue;
        normals.push(convertTheorem(name, sequent, tag, steps));
    }

    // 特殊条目排在最前面，之间不空行；之后空一行再接原来的输出内容
    let output = specials.join('\n');
    if (normals.length > 0) {
        if (specials.length > 0) {
            output += '\n\n';
        }
        output += normals.join('\n\n');
    }
    return output;
}

// ==================== 类型层证明项提取 ====================
function extractTypeLevelProofs(raw) {
    let arr;
    try {
        arr = JSON.parse(raw);
    } catch (e) {
        throw new Error('第四部分不是合法 JSON：' + e.message);
    }
    if (!Array.isArray(arr)) throw new Error('第四部分不是数组');
    return arr.map(s => String(s)).join('\n');
}

function resolveRuleContent(name, rules) {
    // 1. 优先查硬编码字典
    if (Object.prototype.hasOwnProperty.call(THEOREM_CONTENT_DICT, name)) {
        return String(THEOREM_CONTENT_DICT[name]);
    }
    // 2. 再查推理规则表：value = [sequent, tag, steps, ...]
    if (rules && typeof rules === 'object' && !Array.isArray(rules)
        && Object.prototype.hasOwnProperty.call(rules, name)) {
        const v = rules[name];
        if (Array.isArray(v) && typeof v[0] === 'string') {
            return v[0];
        }
    }
    // 3. 都没有
    return '表达式因未知错误而丢失';
}

// ==================== 排列表树状展示 ====================
/*
    排列表形如：
      ["<f>命题逻辑::80::1kbb30::+", "<f>公理::4::ohryb4::+", "mp", ...]
    - "<f>" 前缀表示文件夹
    - 参数以 "::" 分隔（注意：无空格），第一项是名字，第二项是其下（不含自己）项的总数
    - 其余项为推理规则名称
*/
function parseFolderItem(item) {
    let body;
    if (item.startsWith('< f >#')) {
        body = item.slice(6);
    } else if (item.startsWith('< f >')) {
        body = item.slice(5);
    } else {
        body = item;
    }

    const segs = body.split('::');
    const name = (segs[0] || '').trim();
    const count = Number.parseInt(segs[1], 10);
    return { name, count: Number.isFinite(count) ? count : 0 };
}

function buildArrangementTree(items, rules) {
    const showContent = isProofOptionEnabled('contentDisplay');

    function parse(pos, end) {
        const nodes = [];
        while (pos < end) {
            const item = items[pos];
            if (typeof item === 'string' && item.startsWith('< f >')) {
                const { name, count } = parseFolderItem(item);
                const childEnd = Math.min(pos + 1 + count, end);
                const children = parse(pos + 1, childEnd);
                nodes.push({ type: 'folder', name, count, children });
                pos = childEnd;
            } else {
                const name = String(item);
                const content = showContent ? resolveRuleContent(name, rules) : '';
                nodes.push({ type: 'rule', name, content });
                pos++;
            }
        }
        return nodes;
    }
    return parse(0, items.length);
}

function renderTree(nodes, prefix = '') {
    let out = '';
    nodes.forEach((node, idx) => {
        const isLast = idx === nodes.length - 1;
        const branch = isLast ? '└─ ' : '├─ ';
        const childPrefix = prefix + (isLast ? '   ' : '│  ');
        if (node.type === 'folder') {
            out += prefix + branch + '📁 ' + node.name + '\n';
            out += renderTree(node.children, childPrefix);
        } else {
            const suffix = node.content ? ' ' + node.content : '';
            out += prefix + branch + node.name + suffix + '\n';
        }
    });
    return out;
}

// ==================== 文件上传 ====================
fileInput.addEventListener('change', function (e) {
    const file = e.target.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = function (ev) {
        encodedTextarea.value = ev.target.result;
        clearMessages();
        showInfo(`已加载文件：${file.name}`);
        decodeAction();
    };
    reader.onerror = function () { showError('文件读取失败'); };
    reader.readAsText(file);
    fileInput.value = '';
});

// ==================== 解码 ====================
btnDecode.addEventListener('click', decodeAction);

function decodeAction() {
    clearMessages();
    // 注意：D2 末尾空格属于数据，不能用 trim；D1 沿用 trim
    const rawEncoded = encodedTextarea.value;
    if (!rawEncoded.trim()) {
        showError('请先输入或上传编码文档');
        return;
    }
    const version = versionSelect.value;

    // ---------- Deductrium 2.0 ----------
    if (version === 'ddt2') {
        try {
            const { text, note } = tryDecompressD2(rawEncoded);
            let pkg;
            try { pkg = JSON.parse(text); }
            catch (e) { throw new Error('解压成功，但 JSON 解析失败：' + e.message); }
            if (!pkg || typeof pkg !== 'object' || Array.isArray(pkg)) {
                throw new Error('解析结果不是对象，期望 {version, mode, timestamp, data}');
            }
            jsonTextarea.value = JSON.stringify(pkg, null, 2);
            btnDownload.disabled = false;

            const modeMismatch = pkg.mode !== undefined && pkg.mode !== modeSelect.value;
            showInfo('解码成功' + note + '。可编辑 JSON 后重新编码。' +
                (modeMismatch ? `（注意：JSON 内 mode="${pkg.mode}"，与当前模式 "${modeSelect.value}" 不一致）` : ''),
                modeMismatch);
        } catch (e) {
            showError('解码失败：' + e.message);
            console.error(e);
        }
        return;
    }

    // ---------- Deductrium 1（原逻辑，仅把 trim 移到此处） ----------
    const encoded = rawEncoded.trim();
    const mode = modeSelect.value;
    try {
        const decoded = decodeProgressDoc(encoded, mode);
        jsonTextarea.value = decoded;
        RuleOrderStore.invalidate();
        showInfo('解码成功！现在可以查看/编辑解码文本。');
        btnDownload.disabled = true;
    } catch (e) {
        showError('解码失败：' + e.message);
        console.error(e);
    }
}

// 用户手动编辑解码文本时也让缓存失效（程序化赋值不触发 input，所以 decodeAction 里必须单独再调一次）
jsonTextarea.addEventListener('input', () => {
    RuleOrderStore.invalidate();
});

// ==================== 编码 ====================
btnEncode.addEventListener('click', function () {
    clearMessages();
    const json = jsonTextarea.value;
    if (!json) { showError('解码文本框为空，请先解码或粘贴内容'); return; }
    const version = versionSelect.value;

    // ---------- Deductrium 2.0 ----------
    if (version === 'ddt2') {
        let pkg;
        try { pkg = JSON.parse(json); }
        catch (e) { showError('JSON 解析失败：' + e.message); return; }
        if (!pkg || typeof pkg !== 'object' || Array.isArray(pkg)) {
            showError('JSON 顶层必须是对象 {version, mode, timestamp, data}');
            return;
        }

        const mode = modeSelect.value;
        const filled = [];
        if (pkg.version === undefined)   { pkg.version = 0;            filled.push('version=0'); }
        if (pkg.mode === undefined)      { pkg.mode = mode;            filled.push('mode=' + mode); }
        if (pkg.timestamp === undefined) { pkg.timestamp = Date.now(); filled.push('timestamp=now'); }
        if (pkg.data === undefined)      { pkg.data = {};              filled.push('data={}'); }
        const modeMismatch = pkg.mode !== mode;

        try {
            const compact = JSON.stringify(pkg);
            const compressed = LZString.compressToUTF16(compact);
            encodedTextarea.value = compressed;
            jsonTextarea.value = JSON.stringify(pkg, null, 2);   // 回写规范化 JSON
            btnDownload.disabled = false;

            if (modeMismatch) {
                showInfo(
                    `⚠ JSON 内 mode="${pkg.mode}" 与目标模式 "${mode}" 不一致。\n` +
                    `游戏在该模式下导入会跳转/失败。\n` +
                    (filled.length ? `已自动补齐：${filled.join(', ')}` : ''),
                    true
                );
            } else {
                let msg = '编码成功！可以下载进度文档。';
                if (filled.length) msg += `（自动补齐：${filled.join(', ')}）`;
                showInfo(msg);
            }
        } catch (e) {
            showError('编码失败：' + e.message);
            console.error(e);
        }
        return;
    }

    // ---------- Deductrium 1（原逻辑） ----------
    const mode = modeSelect.value;
    try {
        const encoded = encodeProgressDoc(json, mode);
        encodedTextarea.value = encoded;
        showInfo('编码成功！可以下载编码结果。');
        btnDownload.disabled = false;
    } catch (e) {
        showError('编码失败：' + e.message);
        console.error(e);
    }
});

// ==================== 清空 ====================
btnClear.addEventListener('click', function () {
    encodedTextarea.value = '';
    jsonTextarea.value = '';
    proofTextarea.value = '';
    ruleSearchInput.value = '';               // ← 新增
    ruleSearchBox.style.display = 'none';     // ← 新增
    btnDownload.disabled = true;
    RuleOrderStore.invalidate();
    clearMessages();
});

// ==================== 下载 ====================
btnDownload.addEventListener('click', function () {
    const content = encodedTextarea.value;
    if (!content) { showError('没有可下载的内容'); return; }
    const version = versionSelect.value;
    const ext = version === 'ddt2' ? 'dt' : 'txt';
    const blob = new Blob([content], { type: 'text/plain;charset=utf-8' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'progress_document.' + ext;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
    showInfo('下载已开始');
});

// ==================== 提取证明项（推理规则表） ====================
btnExtractProofs.addEventListener('click', function () {
    if (!requireDdt1('提取证明项')) return;
    clearMessages();
    try {
        const sec3 = getSection3();
        const text = extractProofsFromRules(sec3.rules);
        proofTextarea.value = text || '（推理规则表中未找到可转换的定理定义）';
        showInfo('证明项提取成功！');
    } catch (e) {
        proofTextarea.value = '';
        showError('提取证明项失败：' + e.message);
        console.error(e);
    }
});

// ==================== 提取类型层证明项 ====================
btnExtractTypeProofs.addEventListener('click', function () {
    if (!requireDdt1('提取类型层证明项')) return;
    clearMessages();
    try {
        const raw = getSection4Raw();
        const text = extractTypeLevelProofs(raw);
        proofTextarea.value = text || '（类型层证明项为空）';
        showInfo('类型层证明项提取成功！');
    } catch (e) {
        proofTextarea.value = '';
        showError('提取类型层证明项失败：' + e.message);
        console.error(e);
    }
});

// ==================== 推理规则搜索 ====================

// 取可搜索的规则名全集：排列表中显示的所有推理规则
function getSearchCandidates() {
    return Array.from(RuleOrderStore.getOrder());
}

// 原子推理规则：THEOREM_CONTENT_DICT 中的规则，以及 processUnknownRules 聚合出的类别名
const ATOMIC_EXTRA_RULES = new Set(['natop', 'dZn', 'dZop']);

function isAtomicRule(name) {
    return Object.prototype.hasOwnProperty.call(THEOREM_CONTENT_DICT, name)
        || ATOMIC_EXTRA_RULES.has(name);
}

// 递归收集依赖闭包（迭代栈实现，避免深递归）
// 包含根节点；闭包中每个节点要么原子、要么无 steps（视作叶子）
function computeRuleDependencyClosure(root, rules) {
    const closure = new Set();
    const visited = new Set();
    const stack = [root];

    // getDependentRules 会把"不在排列表中的规则名"加上 "#" 前缀，
    // 但对依赖闭包而言，只要 "#xxx" 中的 xxx 是已知定理（THEOREM_CONTENT_DICT 中），
    // 就应该还原为 xxx，否则会因查不到分类而落到"剩余未分类"里原样输出。
    const normalize = (n) => {
        if (typeof n === 'string' && n.startsWith('#')) {
            const bare = n.slice(1);
            if (Object.prototype.hasOwnProperty.call(THEOREM_CONTENT_DICT, bare)) {
                return bare;
            }
        }
        return n;
    };

    while (stack.length) {
        const n = normalize(stack.pop());
        if (visited.has(n)) continue;
        visited.add(n);

        closure.add(n);

        if (isAtomicRule(n)) continue;

        const v = rules[n];
        if (!isTheoremDef(v)) continue;
        const steps = v[2];
        if (!Array.isArray(steps)) continue;

        const deps = getDependentRules(steps);
        for (const d of deps) {
            const nd = normalize(d);
            if (!visited.has(nd)) stack.push(nd);
        }
    }
    return closure;
}

// 按「getDependentRules 的排序」排序一个规则集合
// 规则名：known 按 ruleOrder 位置降序 → unknown（processUnknownRules）→ 聚合名 natop/dZn/dZop
function sortRulesByDependentOrder(nameSet, orderIndex) {
    const known   = [];
    const unknown = [];
    const aggs    = new Set();

    for (const n of nameSet) {
        if (ATOMIC_EXTRA_RULES.has(n)) {
            aggs.add(n);                // 聚合名原样保留，不再过 processUnknownRules
        } else if (orderIndex.has(n)) {
            known.push(n);
        } else {
            unknown.push(n);
        }
    }

    // known：orderIndex 大的在前（与 getDependentRules 一致）
    known.sort((a, b) => orderIndex.get(b) - orderIndex.get(a));

    // 聚合名固定顺序 natop, dZn, dZop
    const aggHead = ['natop', 'dZn', 'dZop'].filter(x => aggs.has(x));

    // unknown 过一遍 processUnknownRules（自动挑出 natop/dZn/dZop 类型并给其余加 #）
    const processedUnknown = processUnknownRules(unknown);

    return [...aggHead, ...processedUnknown, ...known];
}

let searchDismissed = false;   // 点击外部后关闭联想，直到下一次输入才复活

const escapeHtml = s => s.replace(/[&<>]/g, c =>
    ({ '&': '&amp;', '<': '&lt;', '>': '&gt;' }[c]));

function updateRuleSuggestions() {
    const q = ruleSearchInput.value;
    if (!q || searchDismissed) {
        ruleSearchBox.style.display = 'none';
        return;
    }

    let candidates = [];
    try {
        candidates = getSearchCandidates();
    } catch (e) {
        ruleSearchBox.innerHTML =
            '<div class="rule-search-empty">请先解码后再搜索</div>';
        ruleSearchBox.style.display = 'block';
        return;
    }

    const lq   = q.toLowerCase();
    const hits = candidates
        .filter(s => s.toLowerCase().startsWith(lq))
        .slice(0, 100);

    if (!hits.length) {
        ruleSearchBox.innerHTML =
            '<div class="rule-search-empty">无匹配规则</div>';
        ruleSearchBox.style.display = 'block';
        return;
    }
    ruleSearchBox.innerHTML = hits.map(s =>
        `<div data-name="${escapeHtml(s)}">` +
            `<b>${escapeHtml(s.slice(0, q.length))}</b>` +
            `${escapeHtml(s.slice(q.length))}` +
        `</div>`
    ).join('');
    ruleSearchBox.style.display = 'block';
}

function selectRuleSuggestion(name) {
    ruleSearchInput.value = name;
    searchDismissed = true;
    ruleSearchBox.style.display = 'none';
    ruleSearchInput.focus();
}

function searchRuleAction() {
    if (!requireDdt1('推理规则搜索')) return;
    clearMessages();
    const name = ruleSearchInput.value.trim();
    if (!name) {
        showError('请先输入推理规则名称');
        return;
    }
    searchDismissed = true;
    ruleSearchBox.style.display = 'none';

    try {
        const sec3  = getSection3();
        const rules = sec3.rules || {};
        const order = RuleOrderStore.getOrder();
        const orderIndex = new Map(Array.from(order, (n, i) => [n, i]));

        const inOrder = orderIndex.has(name);
        const inDict  = Object.prototype.hasOwnProperty.call(THEOREM_CONTENT_DICT, name);
        const inRules = Object.prototype.hasOwnProperty.call(rules, name);
        if (!inOrder && !inDict && !inRules) {
            proofTextarea.value = '';
            showError(`未找到规则：${name}`);
            return;
        }

        // 递归收集依赖闭包（包含 name 自身）
        const closure = computeRuleDependencyClosure(name, rules);

        // 分类替换：只要闭包中出现某分类的任意规则，就合并为该分类名
        const triggeredCategories = new Set();
        const remainingRules = new Set();

        for (const r of closure) {
            const cat = RULE_CATEGORY_MAP.get(r);
            if (cat) {
                triggeredCategories.add(cat);
            } else {
                remainingRules.add(r);
            }
        }

        // 分类名按 CATEGORY_ORDER 排在最前
        const sortedCategories = CATEGORY_ORDER.filter(c => triggeredCategories.has(c));

        // 剩余规则：属于排列表的按排列顺序（orderIndex 升序）；
        // 不属于排列表的（如 natop、dZn、dZop、#xxx）按名称排序放在最后
        const remainingInOrder = [];
        const remainingNotInOrder = [];
        for (const r of remainingRules) {
            if (orderIndex.has(r)) {
                remainingInOrder.push(r);
            } else {
                remainingNotInOrder.push(r);
            }
        }
        remainingInOrder.sort((a, b) => orderIndex.get(a) - orderIndex.get(b));
        remainingNotInOrder.sort((a, b) => a.localeCompare(b));

        // 聚合名改为中文显示
        const displayName = (r) => {
            if (r === 'natop') return '自然数自动计算';
            if (r === 'dZn')   return '整数定义';
            if (r === 'dZop')  return '整数自动计算';
            return r;
        };

        const outputLines = [
            ...sortedCategories,
            ...remainingInOrder.map(displayName),
            ...remainingNotInOrder.map(displayName),
        ];

        proofTextarea.value = outputLines.join('\n');
        showInfo(`已搜索规则 ${name}，简化后共 ${outputLines.length} 项`);
    } catch (e) {
        proofTextarea.value = '';
        showError('搜索失败：' + e.message);
    }
}

// ==================== 展示排列表 ====================
btnShowArrangement.addEventListener('click', function () {
    if (!requireDdt1('展示排列表')) return;
    clearMessages();
    try {
        const sec3 = getSection3();
        const items = sec3.arrangement;
        if (!Array.isArray(items)) throw new Error('排列表不是数组');
        const tree = buildArrangementTree(items, sec3.rules);
        const header = `排列表（共 ${items.length} 项）\n\n`;
        proofTextarea.value = header + renderTree(tree);
        showInfo('排列表展示成功！');
    } catch (e) {
        proofTextarea.value = '';
        showError('展示排列表失败：' + e.message);
        console.error(e);
    }
});

// ==================== 推理规则搜索事件 ====================
btnSearchRule.addEventListener('click', searchRuleAction);

ruleSearchInput.addEventListener('input', () => {
    searchDismissed = false;
    updateRuleSuggestions();
});

ruleSearchInput.addEventListener('keydown', e => {
    if (e.key === 'Enter') {
        e.preventDefault();
        searchRuleAction();
    } else if (e.key === 'Escape') {
        searchDismissed = true;
        ruleSearchBox.style.display = 'none';
    }
});

// 点击联想行：填充输入框（不自动搜索，与示例一致）
ruleSearchBox.addEventListener('mousedown', e => {
    const row = e.target.closest('div[data-name]');
    if (!row) return;
    e.preventDefault();
    selectRuleSuggestion(row.dataset.name);
});

// 点击其他任何地方：关闭联想
document.addEventListener('mousedown', e => {
    if (!ruleSearchInput.contains(e.target) &&
        !ruleSearchBox.contains(e.target)) {
        searchDismissed = true;
        ruleSearchBox.style.display = 'none';
    }
});

versionSelect.addEventListener('change', function () {
    clearMessages();
});

// ==================== 初始化 ====================
initProofOptions();
btnDownload.disabled = true;
clearMessages();