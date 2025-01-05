单源最短路伪代码

$
\text{for each vertex }v\text{ :} \\ 
\quad \operatorname{dist}_v = \infty \\ 
\quad \operatorname{prev}_v = \text{none} \\ 
\operatorname{dist}_{\operatorname{source}} = 0 \\
\text{set all vertices to unexplored} \\
\text{while destination not explored :} \\
\quad v = \text{least-valued unexplored vertex} \\
\quad \text{set }v\text{ to explored} \\
\quad \text{for each edge }(v, w)\text{ :} \\
\quad \quad \text{if }\operatorname{dist}_v + \operatorname{len}(v,w) < \operatorname{dist}_w\text{ :} \\
\quad \quad \quad \operatorname{dist}_w = \operatorname{dist}_v + \operatorname{len}(v,w) \\
\quad \quad \quad \operatorname{prev}_w = v
$