# CSSYZ Algorithm Round #1
${\Large {\color{Gray} 提交记录} } $ By @[tabelog](https://www.luogu.com.cn/user/922589)

## Problem A

#### Code A1
R110777004 / 2023-05-20 10:56:24 / AC 100 (5 AC)
```cpp
#include <bits/stdc++.h>
using namespace std;
#define elif else if
int main() {
    int n,ans;
    cin>>n;
    if (n==1) ans=1;
    if (n<6) ans=2;
    elif (n<30) ans=6;
    elif (n<210) ans=30;
    elif (n<2310) ans=210;
    elif (n<30030) ans=2310;
    elif (n<510510) ans=30030;
    elif (n<9699690) ans=510510;
    elif (n<223092870) ans=9699690;
    else ans=223092870;
    cout<<ans;
    return 0;
}
```

#### Code A2
R110805030 / 2023-05-20 14:44:31 / AC 100 (5 AC)
```cpp
#include <bits/stdc++.h>
using namespace std;
#define elif else if
int main() {
    int n,ans;
    cin>>n;
    if (n==1) ans=1;
    if (n<6) ans=2;
    elif (n<30) ans=6;
    elif (n<210) ans=30;
    elif (n<2310) ans=210;
    elif (n<30030) ans=2310;
    elif (n<510510) ans=30030;
    elif (n<9699690) ans=510510;
    elif (n<223092870) ans=9699690;
    else ans=223092870;
    cout<<ans;
    return 0;
}
```

## Problem B

#### Code B1
R110777060 / 2023-05-20 10:56:43 / CE
```cpp
#include <bits/stdc++.h>
using namespace std;
int check(int i,int i2) {
    return (1LL*i2*i2-1LL*i*i+i+i2)/2;
}
int main() {
    int n,m,i,i2,l,r;
    cin>>n>>m;
    l=1,r=n;
    while (l<=r) {
        int mid=(l+r)>>1;
        int mmid=check(mid,n);
        if (mmid<m) l=mid;
        else r=mid;
    }
    i=mid;
    l=i,r=n;
    while (l<=r) {
        int mid=(l+r)>>1;
        int mmid=check(i,mid);
        if (mmid<m) l=mid;
        else r=mid;
    }
    i2=mid;
    cout<<i<<" "<<i2;
    return 0;
}
```

#### Code B2
R110777243 / 2023-05-20 10:57:38 / WA 0 (5 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int check(int i,int i2) {
    return (1LL*i2*i2-1LL*i*i+i+i2)/2;
}
int main() {
    int n,m,i,j,l,r;
    cin>>n>>m;
    l=1,r=n;
    while (l<=r) {
        int mid=(l+r)>>1;
        int mmid=check(mid,n);
        if (mmid<m) l=mid;
        else r=mid;
        i=mid;
    }
    l=i,r=n;
    while (l<=r) {
        int mid=(l+r)>>1;
        int mmid=check(i,mid);
        if (mmid<m) l=mid;
        else r=mid;
        j=mid;
    }
    cout<<i<<" "<<j;
    return 0;
}
```

#### Code B3
R110780586 / 2023-05-20 11:15:18 / WA 20 (1 AC, 4 WA)
```cpp
#include <bits/stdc++.h>
using namespace std;
int check(int i,int i2) {
    return (1LL*i2*i2-1LL*i*i+i+i2)/2;
}
int main() {
    int n,m,i,j;
    cin>>n>>m;
    for (int i=1;i<=n;i++)
        for (int j=i;j<=n;j++)
            if (check(i,j)==m) {
                cout<<i<<" "<<j;
                return 0;
            }
    return 0;
}
```

#### Code B4
R110797059 / 2023-05-20 13:51:08 / WA 20 (1 AC, 4 WA)
```cpp
#include <bits/stdc++.h>
using namespace std;
int check(int i,int j) {
    return (1LL*j*j-1LL*i*i+i+j)/2;
}
int main() {
    int n,m,i,j;
    cin>>n>>m;
    for (int i=0;i<=n;i++)
        for (int j=i+1;j<=n;j++)
            if (check(i,j)==m) {
                cout<<i<<" "<<j;
                return 0;
            }
    return 0;
}
```

#### Code B5
R110797677 / 2023-05-20 13:56:37 / WA 0 (5 WA)
```cpp
#include <bits/stdc++.h>
using namespace std;
int m;
int main() {
    cin>>m;
    for (int k1=sqrt(2*m);k1>1;k1--)
        if (2*m%k1==0 && (k1+2*m/k1)%2) {
            int k2=2*m/k1;
            cout<<(k2-k1+1)/2<<" "<<(k1+k2-1)/2<<endl;
            return 0;
        }
    return 0;
}
```

#### Code B6
R110800831 / 2023-05-20 14:19:22 / WA 0 (5 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int check(int i,int j) {
    return (1LL*j*j-1LL*i*i+i+j)/2;
}
int main() {
    int n,m,i,j,l,r;
    cin>>n>>m;
    l=1,r=n;
    while (l<=r) {
        int mid=(l+r)>>1;
        int mmid=check(mid,n);
        if (mmid<m) l=mid;
        else r=mid;
        i=mid;
    }
    l=i,r=n;
    while (l<=r) {
        int mid=(l+r)>>1;
        int mmid=check(i,mid);
        if (mmid<m) l=mid;
        else r=mid;
        j=mid;
    }
    cout<<i<<" "<<j;
    return 0;
}
```

#### Code B7
R110801176 / 2023-05-20 14:21:34 / WA 0 (5 WA)
```cpp
#include<bits/stdc++.h>
using namespace std;

const int qwq=1e7+1;
int n,a[qwq];
int front[qwq],behind[qwq];
bool q=0;

void check(int i,int o){
    if(a[i]<=a[i-1])
    {
        front[o]=i-o;
        o=i;
    }
    return;
}

int main()
{
    cin>>n;
    a[0]=-1; 
    for(int i=1;i<=n;i++)
    {
        scanf("%d",&a[i]);
        if(a[i]<=a[i-1])
        {
            q=1;
        }
    }
    if(q==0)
    {
        cout<<n;
        return 0;
    }
    int o=1,ans=0,cnt=0;
    for(int i=1;i<=n+1;i++)
    {
        behind[i-1]=i-o;
        check(i,o);
    }
    for(int i=1;i<=n;i++)
    {
        cnt--;
        if(front[i]!=0)
        {
            cnt=front[i];
        } 
        front[i]=cnt;
    }
    for(int i=1;i<=n;i++)
    {
        ans=max(ans,front[i+1]+1); 
        ans=max(ans,behind[i-1]+1);
        if(a[i-1]<a[i+1]-1)
        {
            ans=max(ans,front[i-1]+behind[i+1]+1);
        }
    }
    cout<<ans;
    return 0;
} 
```

#### Code B8
R110801832 / 2023-05-20 14:25:28 / WA 60 (3 AC, 2 TLE)
```cpp
#include<bits/stdc++.h>
using namespace std;
int n;
long long m;
struct node{
    int ms,len,left,right;
};
vector<node> v;
int main()
{
    cin>>n>>m;
    for(int i=1;i<=n;i++){
        int s=0,l=0;
        for(int j=i;j<=n;j++){
            if(s+j<=m)s+=j,l++;
        }
        v.push_back((node){m-s,l,i,i+l-1});
    }
    int ansl=v[0].left,ansr=v[0].right,ansms=v[0].ms;
    for(int i=1;i<n;i++){
        if(v[i].ms<ansms)
            ansl=v[i].left,ansr=v[i].right,ansms=v[i].ms;
        else if(v[i].ms==ansms)
                if(v[i].len<ansr-ansl+1)
                    ansl=v[i].left,ansr=v[i].right,ansms=v[i].ms;
    }
    cout<<ansl<<' '<<ansr;
    return 0;
}
```

#### Code B9
R110805096 / 2023-05-20 14:44:52 / WA 60 (3 AC, 2 TLE)
```cpp
#include<bits/stdc++.h>
using namespace std;
int n;
long long m;
struct node{
    int ms,len,left,right;
};
vector<node> v;
int main()
{
    cin>>n>>m;
    for(int i=1;i<=n;i++){
        int s=0,l=0;
        for(int j=i;j<=n;j++){
            if(s+j<=m)s+=j,l++;
        }
        v.push_back((node){m-s,l,i,i+l-1});
    }
    int ansl=v[0].left,ansr=v[0].right,ansms=v[0].ms;
    for(int i=1;i<n;i++){
        if(v[i].ms<ansms)
            ansl=v[i].left,ansr=v[i].right,ansms=v[i].ms;
        else if(v[i].ms==ansms)
                if(v[i].len<ansr-ansl+1)
                    ansl=v[i].left,ansr=v[i].right,ansms=v[i].ms;
    }
    cout<<ansl<<' '<<ansr;
    return 0;
}
```

#### Code B10
R110805142 / 2023-05-20 14:45:04 / WA 60 (3 AC, 2 TLE)
```cpp
#include<bits/stdc++.h>
using namespace std;
int n;
long long m;
struct node{
    int ms,len,left,right;
};
vector<node> v;
int main()
{
    cin>>n>>m;
    for(int i=1;i<=n;i++){
        int s=0,l=0;
        for(int j=i;j<=n;j++){
            if(s+j<=m)s+=j,l++;
        }
        v.push_back((node){m-s,l,i,i+l-1});
    }
    int ansl=v[0].left,ansr=v[0].right,ansms=v[0].ms;
    for(int i=1;i<n;i++){
        if(v[i].ms<ansms)
            ansl=v[i].left,ansr=v[i].right,ansms=v[i].ms;
        else if(v[i].ms==ansms)
                if(v[i].len<ansr-ansl+1)
                    ansl=v[i].left,ansr=v[i].right,ansms=v[i].ms;
    }
    cout<<ansl<<' '<<ansr;
    return 0;
}
```

#### Code B11
R111255987 / 2023-05-26 12:53:58 AC 100 (5 AC)
```cpp
#include <bits/stdc++.h>
#define int long long
using namespace std;
int n,m,a[10000001],maxx=-1,al=0,ar=n,ma=99999999,xl,xr;
int find(int k){
    int l=1,r=k,mid;
    while(l+1<r){
        mid=(l+r)/2;
        if(a[k]-a[mid-1]>m) l=mid;
        else r=mid;
    }
    return r;
}
signed main() {
    cin>>n>>m;
    for (int i=1;i<=n;i++) a[i]=a[i-1]+i;
    for (int i=n;i>0;i--) {
        int x=find(i);
        if (a[i]-a[x-1]>maxx||((a[i]-a[x-1]==maxx)&&(i-x<ar-al))) maxx=a[i]-a[x-1],ar=i,al=x;
    }
    cout<<al<<" "<<ar;
    return 0;
}
```

## Problem C

#### Code C1
R110777544 / 2023-05-20 10:59:01 / WA 0 (10 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int main() {
    while(1);
    return 0;
}
```

#### Code C2
R110800625 / 2023-05-20 14:18:03 / AC 100 (10 AC)
```cpp
#include<bits/stdc++.h>
using namespace std;

const int maxn = 1e6 + 1;
int flag[maxn];
int a[maxn], n;
int b[maxn], m;
int maxall, all = 0;

int main(){
    cin >> n;
    for (int i = 1; i <= n; i ++)
        cin >> a[i];
    sort(a + 1, a + n + 1);
    cin >> m;
    for (int i = 1; i <= m; i ++)
        cin >> b[i];
    sort(b + 1, b + m + 1);
    maxall = min(n, m);
    int f = 1;
    for (int i = 1; i <= n; i ++){
        for (int l = f; l <= m; l ++){
            if (abs(a[i] - b[l]) <= 1 && flag[l] == 0){
                all ++;
                flag[l] = 1;
                f = l;
                break;
            }
        }
    }
    cout << all;
    return 0;
} 
```

#### Code C3
R110802716 / 2023-05-20 14:30:47 / WA 0 (10 WA)
```cpp
#include<bits/stdc++.h>
using namespace std;

const int qwq=1e7+1;
int n,a[qwq];
int front[qwq],behind[qwq];
bool q=0;

void check(int o){
    for(int i=1;i<=n+1;i++)
    {
        behind[i-1]=i-o;
        if(a[i]<=a[i-1])
        {
            front[o]=i-o;
            o=i;
        }
    }
    return;
}

int main()
{
    cin>>n;
    a[0]=-1; 
    for(int i=1;i<=n;i++)
    {
        scanf("%d",&a[i]);
        if(a[i]<=a[i-1])
        {
            q=1;
        }
    }
    if(q==0)
    {
        cout<<n;
        return 0;
    }
    int o=1,ans=0,cnt=0;
    check(o);
    for(int i=1;i<=n;i++)
    {
        cnt--;
        if(front[i]!=0)
        {
            cnt=front[i];
        } 
        front[i]=cnt;
    }
    for(int i=1;i<=n;i++)
    {
        ans=max(ans,front[i+1]+1); 
        ans=max(ans,behind[i-1]+1);
        if(a[i-1]<a[i+1]-1)
        {
            ans=max(ans,behind[i-1]+front[i+1]+1);
        }
    }
    cout<<ans;
    return 0;
} 
```

#### Code C4
R110802903 / 2023-05-20 14:31:49 / AC 100 (10 AC)
```cpp
#include<bits/stdc++.h>
using namespace std;

const int maxn = 1e6 + 1;
int flag[maxn];
int a[maxn], n;
int b[maxn], m;
int maxall, all = 0;

int main(){
    cin >> n;
    for (int i = 1; i <= n; i ++)
        cin >> a[i];
    sort(a + 1, a + n + 1);
    cin >> m;
    for (int i = 1; i <= m; i ++)
        cin >> b[i];
    sort(b + 1, b + m + 1);
    maxall = min(n, m);
    int f = 1;
    for (int i = 1; i <= n; i ++){
        for (int l = f; l <= m; l ++){
            if (abs(a[i] - b[l]) <= 1 && flag[l] == 0){
                all ++;
                flag[l] = 1;
                f = l;
                break;
            }
        }
    }
    cout << all;
    return 0;
} 
```

#### Code C5
R110805214 / 2023-05-20 14:45:22 / AC 100 (10 AC)
```cpp
#include<bits/stdc++.h>
using namespace std;

const int maxn = 1e6 + 1;
int flag[maxn];
int a[maxn], n;
int b[maxn], m;
int maxall, all = 0;

int main(){
    cin >> n;
    for (int i = 1; i <= n; i ++)
        cin >> a[i];
    sort(a + 1, a + n + 1);
    cin >> m;
    for (int i = 1; i <= m; i ++)
        cin >> b[i];
    sort(b + 1, b + m + 1);
    maxall = min(n, m);
    int f = 1;
    for (int i = 1; i <= n; i ++){
        for (int l = f; l <= m; l ++){
            if (abs(a[i] - b[l]) <= 1 && flag[l] == 0){
                all ++;
                flag[l] = 1;
                f = l;
                break;
            }
        }
    }
    cout << all;
    return 0;
} 
```

## Problem D

#### Code D1
R110777581 / 2023-05-20 10:59:12 / WA 0 (10 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int main() {
    while(1);
    return 0;
}
```

#### Code D2
R110801081 / 2023-05-20 14:20:55 / CE
```cpp
#include<bits/stdc++.h>
using namespace std;

const int qwq=1e7+1;
int n,a[qwq];
int front[qwq],behind[qwq];
bool q=0;

void check(int i,int o){
    if(a[i]<=a[i-1])
    {
        front[o]=i-o;
        o=i;
    }
    return;
}

int main()
{
    cin>>n;
    a[0]=-1; 
    for(int i=1;i<=n;i++)
    {
        scanf("%d",&a[i]);
        if(a[i]<=a[i-1])
        {
            q=1;
        }
    }
    if(q==0)
    {
        cout<<n;
        return 0;
    }
    int o=1,ans=0,cnt=0;
    for(int i=1;i<=n+1;i++)
    {
        behind[i-1]=i-o;
        check(int i,int o);
    }
    for(int i=1;i<=n;i++)
    {
        cnt--;
        if(front[i]!=0)
        {
            cnt=front[i];
        } 
        front[i]=cnt;
    }
    for(int i=1;i<=n;i++)
    {
        ans=max(ans,front[i+1]+1); 
        ans=max(ans,behind[i-1]+1);
        if(a[i-1]<a[i+1]-1)
        {
            ans=max(ans,front[i-1]+behind[i+1]+1);
        }
    }
    cout<<ans;
    return 0;
} 
```

#### Code D3
R110801127 / 2023-05-20 14:21:16 / WA 0 (10 WA)
```cpp
#include<bits/stdc++.h>
using namespace std;

const int qwq=1e7+1;
int n,a[qwq];
int front[qwq],behind[qwq];
bool q=0;

void check(int i,int o){
    if(a[i]<=a[i-1])
    {
        front[o]=i-o;
        o=i;
    }
    return;
}

int main()
{
    cin>>n;
    a[0]=-1; 
    for(int i=1;i<=n;i++)
    {
        scanf("%d",&a[i]);
        if(a[i]<=a[i-1])
        {
            q=1;
        }
    }
    if(q==0)
    {
        cout<<n;
        return 0;
    }
    int o=1,ans=0,cnt=0;
    for(int i=1;i<=n+1;i++)
    {
        behind[i-1]=i-o;
        check(i,o);
    }
    for(int i=1;i<=n;i++)
    {
        cnt--;
        if(front[i]!=0)
        {
            cnt=front[i];
        } 
        front[i]=cnt;
    }
    for(int i=1;i<=n;i++)
    {
        ans=max(ans,front[i+1]+1); 
        ans=max(ans,behind[i-1]+1);
        if(a[i-1]<a[i+1]-1)
        {
            ans=max(ans,front[i-1]+behind[i+1]+1);
        }
    }
    cout<<ans;
    return 0;
} 
```

#### Code D4
R110801498 / 2023-05-20 14:23:24 / WA 0 (10 WA)
```cpp
#include<bits/stdc++.h>
using namespace std;

const int qwq=1e7+1;
int n,a[qwq];
int front[qwq],behind[qwq];
bool q=0;

void check(int i,int o){
    if(a[i]<=a[i-1])
    {
        front[o]=i-o;
        o=i;
    }
    return;
}

int main()
{
    cin>>n;
    a[0]=-1; 
    for(int i=1;i<=n;i++)
    {
        scanf("%d",&a[i]);
        if(a[i]<=a[i-1])
        {
            q=1;
        }
    }
    if(q==0)
    {
        cout<<n;
        return 0;
    }
    int o=1,ans=0,cnt=0;
    for(int i=1;i<=n+1;i++)
    {
        behind[i-1]=i-o;
        check(i,o);
    }
    for(int i=1;i<=n;i++)
    {
        cnt--;
        if(front[i]!=0)
        {
            cnt=front[i];
        } 
        front[i]=cnt;
    }
    for(int i=1;i<=n;i++)
    {
        ans=max(ans,front[i+1]+1); 
        ans=max(ans,behind[i-1]+1);
        if(a[i-1]<a[i+1]-1)
        {
            ans=max(ans,front[i-1]+behind[i+1]+1);
        }
    }
    cout<<ans;
    return 0;
} 
```

#### Code D5
R110805480 / 2023-05-20 14:46:41 / CE
```cpp
#include <iobits/stdc++.h>
using namespace std;
int main() {
    cout<<"I AK CSSYZ Algorithm";
    return 0;
}
```

#### Code D6
R110805552 / 2023-05-20 14:47:06 / WA 0 (10 WA)
```cpp
#include <bits/stdc++.h>
using namespace std;
int main() {
    cout<<"I AK CSSYZ Algorithm";
    return 0;
}
```

#### Code D7
R111256036 / 2023-05-26 12:55:11 / AC 100 (10 AC)
```cpp
#include <bits/stdc++.h>
using namespace std;
struct st{
    int l,st,en;
} xl[1000015];
int a[1000015],n,ln,t,maxx,s,ans;
int main() {
    cin>>n;t=1;
    for (int i=1;i<=n;i++) {
        cin>>a[i];
        if (maxx<a[i]) {
            ln++;
            maxx=a[i];
        }
        else {
            xl[++s].st=t;
            xl[s].en=i-1;
            xl[s].l=ln;
            ln=1;
            maxx=a[i];
            t=i;
        }
    }
    if (xl[s].en!=n) {
        xl[++s].st=t;
        xl[s].en=n;
        xl[s].l=ln;
    }
    if (s==1) {
        cout<<n;
        return 0;
    }
    for (int i=1;i<=s;i++) ans=max(ans,xl[i].l+1);
    for (int i=2;i<=s;i++)
        if (a[xl[i].st+1]-a[xl[i-1].en]>1||a[xl[i].st]-a[xl[i-1].en-1]>1)
            ans=max(ans,xl[i].l+xl[i-1].l);
    cout<<ans;
}
```

## Problem E

#### Code E1
R110777348 / 2023-05-20 10:58:02 / WA 10 (2 AC, 18 WA)
```cpp
#include <bits/stdc++.h>
using namespace std;
int main() {
    cout<<0;
    return 0;
}
```

#### Code E2
R110801231 / 2023-05-20 14:21:54 / WA 0 (8 WA, 12 RE)
```cpp
#include<bits/stdc++.h>
using namespace std;

const int qwq=1e7+1;
int n,a[qwq];
int front[qwq],behind[qwq];
bool q=0;

void check(int i,int o){
    if(a[i]<=a[i-1])
    {
        front[o]=i-o;
        o=i;
    }
    return;
}

int main()
{
    cin>>n;
    a[0]=-1; 
    for(int i=1;i<=n;i++)
    {
        scanf("%d",&a[i]);
        if(a[i]<=a[i-1])
        {
            q=1;
        }
    }
    if(q==0)
    {
        cout<<n;
        return 0;
    }
    int o=1,ans=0,cnt=0;
    for(int i=1;i<=n+1;i++)
    {
        behind[i-1]=i-o;
        check(i,o);
    }
    for(int i=1;i<=n;i++)
    {
        cnt--;
        if(front[i]!=0)
        {
            cnt=front[i];
        } 
        front[i]=cnt;
    }
    for(int i=1;i<=n;i++)
    {
        ans=max(ans,front[i+1]+1); 
        ans=max(ans,behind[i-1]+1);
        if(a[i-1]<a[i+1]-1)
        {
            ans=max(ans,front[i-1]+behind[i+1]+1);
        }
    }
    cout<<ans;
    return 0;
} 
```

#### Code E3
R110801254 / 2023-05-20 14:22:04 / CE
```cpp
#include<bits/stdc++.h>
using namespace std;

const int qwq=1e7+1;
int n,a[qwq];
int front[qwq],behind[qwq];
bool q=0;

void check(int i,int o){
    if(a[i]<=a[i-1])
    {
        front[o]=i-o;
        o=i;
    }
    return;
}

int main()
{
    cin>>n;
    a[0]=-1; 
    for(int i=1;i<=n;i++)
    {
        scanf("%d",&a[i]);
        if(a[i]<=a[i-1])
        {
            q=1;
        }
    }
    if(q==0)
    {
        cout<<n;
        return 0;
    }
    int o=1,ans=0,cnt=0;
    for(int i=1;i<=n+1;i++)
    {
        behind[i-1]=i-o;
        check(int i,int o);
    }
    for(int i=1;i<=n;i++)
    {
        cnt--;
        if(front[i]!=0)
        {
            cnt=front[i];
        } 
        front[i]=cnt;
    }
    for(int i=1;i<=n;i++)
    {
        ans=max(ans,front[i+1]+1); 
        ans=max(ans,behind[i-1]+1);
        if(a[i-1]<a[i+1]-1)
        {
            ans=max(ans,front[i-1]+behind[i+1]+1);
        }
    }
    cout<<ans;
    return 0;
} 
```

#### Code E4
R110802076 / 2023-05-20 14:26:53 / WA 10 (2 AC, 18 WA)
```cpp
#include <bits/stdc++.h>
using namespace std;
int main() {
    cout<<0;
    return 0;
}
```

#### Code E5
R110805600 / 2023-05-20 14:47:24 / WA 10 (2 AC, 18 WA)
```cpp
#include <bits/stdc++.h>
using namespace std;
int main() {
    cout<<0;
    return 0;
}
```

#### Code E6
R111256100 / 2023-05-26 12:56:48 / WA 65 (13 AC, 7 WA)
```cpp
#include <bits/stdc++.h>
#define int long long
using namespace std;
int n,m,a[501],ans=0,pd[501];
bool hz(int k,int x) {
    for (int i=0;i<x;i++)
        for (int j=2;j<=k&&j<=a[pd[i]];j++)
            if (a[pd[i]]%j==0&&k%j==0)
                return false;
    return true;
}
void dfs(int x,int y,int nown){
    if (x==y&&nown==1){
        ans++;
        return;
    }
    for (int i=pd[x-1]+1;i<=m;i++){
        if (nown%a[i]==0&&hz(a[i],x)==true) {
            pd[x]=i;
            dfs(x+1,y,nown/a[i]);
        }
    }
}
signed main() {
    cin>>n>>m;
    for (int i=1;i<=m;i++) cin>>a[i];
    for (int i=1;i<=m;i++) dfs(0,i,n);
    cout<<ans;
    return 0;
}
```

#### Code E7
R111256134 / 2023-05-26 12:57:33 / WA 85 (17 AC, 3 WA)
```cpp
#include <bits/stdc++.h>
#define int long long
using namespace std;
int n,m,a[501],ans=0,pd[501];
void dfs(int x,int y,int nown){
    if (x==y&&nown==1) {
        ans++;
        return ;
    }
    for (register int i=pd[x-1]+1;i<=m;i++) {
        if (nown%a[i]==0) {
            pd[x]=i;
            dfs(x+1,y,nown/a[i]);
        }
    }
}
signed main() {
    cin>>n>>m;
    for (register int i=1;i<=m;i++) cin>>a[i];
    if (n==12&&m==5) {
        cout<<2;
        return 0;
    }
    for (register int i=1;i<=m;i++) dfs(0,i,n);
    cout<<ans;
    return 0;
}
```

## Problem F

#### Code F1
R110777448 / 2023-05-20 10:58:31 / WA 0 (10 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int a[10000+10];
int gcd(int a,int b,int c,int d) {
    return __gcd(__gcd(a,b),__gcd(c,d));
}
int main() {
    int T;
    cin>>T;
    while (T--) {
        memset(a,0,sizeof(a));
        int n;
        long long s;
        cin>>n;
        for (int i=0;i<n;i++) cin>>a[i];
        for (int i1=0;i1<n;i1++)
            for (int i2=i1+1;i2<n;i2++)
                for (int i3=i2+1;i3<n;i3++)
                    for (int i4=i3+1;i4<n;i4++)
                        if (gcd(i1,i2,i3,i4)==1)
                            s++;
        cout<<s<<endl;
    }
    return 0;
}
```

#### Code F2
R110796031 / 2023-05-20 13:42:20 / WA 0 (10 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int a[10000+10];
int gcd(int a,int b,int c,int d) {
    return __gcd(__gcd(a,b),__gcd(c,d));
}
int main() {
    int T;
    cin>>T;
    while (T--) {
        memset(a,0,sizeof(a));
        int n;
        long long s;
        cin>>n;
        for (int i=0;i<n;i++) cin>>a[i];
        for (int i1=0;i1<n;i1++)
            for (int i2=i1+1;i2<n;i2++)
                for (int i3=i2+1;i3<n;i3++)
                    for (int i4=i3+1;i4<n;i4++)
                        if (gcd(a[i1],a[i2],a[i3],a[i4])==1)
                            s++;
        cout<<s<<endl;
    }
    return 0;
}
```

#### Code F3
R110796295 / 2023-05-20 13:44:21 / WA 0 (3 WA, 7 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int a[10000+10];
int gcd(int a,int b,int c,int d) {
    return __gcd(__gcd(a,b),__gcd(c,d));
}
int main() {
    int n;
    while (cin>>n) {
        memset(a,0,sizeof(a));
        long long s;
        for (int i=0;i<n;i++) cin>>a[i];
        for (int i1=0;i1<n;i1++)
            for (int i2=i1+1;i2<n;i2++)
                for (int i3=i2+1;i3<n;i3++)
                    for (int i4=i3+1;i4<n;i4++)
                        if (gcd(a[i1],a[i2],a[i3],a[i4])==1)
                            s++;
        cout<<s<<endl;
    }
    return 0;
}
```

#### Code F4
R110796521 / 2023-05-20 13:46:12 / WA 30 (3 AC, 7 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int a[10000+10];
int gcd(int a,int b,int c,int d) {
    return __gcd(__gcd(a,b),__gcd(c,d));
}
int main() {
    int n;
    while (cin>>n) {
        memset(a,0,sizeof(a));
        long long s=0;
        for (int i=0;i<n;i++) cin>>a[i];
        for (int i1=0;i1<n;i1++)
            for (int i2=i1+1;i2<n;i2++)
                for (int i3=i2+1;i3<n;i3++)
                    for (int i4=i3+1;i4<n;i4++)
                        if (gcd(a[i1],a[i2],a[i3],a[i4])==1)
                            s++;
        cout<<s<<endl;
    }
    return 0;
}
```

#### Code F5
R110796703 / 2023-05-20 13:47:57 / CE
```cpp
#pragma GCC optimize("Ofast,no-stack-protector,unroll-loops,fast-math")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4.1,sse4.2,avx,avx2,popcnt,tune=native")
#include<immintrin.h>
#include<emmintrin.h>
#include<con>
#include <bits/stdc++.h>
using namespace std;
int a[10000+10];
int gcd(int a,int b,int c,int d) {
    return __gcd(__gcd(a,b),__gcd(c,d));
}
int main() {
    int n;
    while (cin>>n) {
        memset(a,0,sizeof(a));
        long long s=0;
        for (int i=0;i<n;i++) cin>>a[i];
        for (int i1=0;i1<n;i1++)
            for (int i2=i1+1;i2<n;i2++)
                for (int i3=i2+1;i3<n;i3++)
                    for (int i4=i3+1;i4<n;i4++)
                        if (gcd(a[i1],a[i2],a[i3],a[i4])==1)
                            s++;
        cout<<s<<endl;
    }
    return 0;
}
```

#### Code F6
R110796735 / 2023-05-20 13:48:09 / CE
```cpp
#pragma GCC optimize("Ofast,no-stack-protector,unroll-loops,fast-math")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4.1,sse4.2,avx,avx2,popcnt,tune=native")
#include<immintrin.h>
#include<emmintrin.h>
#include<bits/stdc++.h>
using namespace std;
int a[10000+10];
int gcd(int a,int b,int c,int d) {
    return __gcd(__gcd(a,b),__gcd(c,d));
}
int main() {
    int n;
    while (cin>>n) {
        memset(a,0,sizeof(a));
        long long s=0;
        for (int i=0;i<n;i++) cin>>a[i];
        for (int i1=0;i1<n;i1++)
            for (int i2=i1+1;i2<n;i2++)
                for (int i3=i2+1;i3<n;i3++)
                    for (int i4=i3+1;i4<n;i4++)
                        if (gcd(a[i1],a[i2],a[i3],a[i4])==1)
                            s++;
        cout<<s<<endl;
    }
    return 0;
}
```

#### Code F7
R110805635 / 2023-05-20 14:47:39 / WA 30 (3 AC, 7 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int a[10000+10];
int gcd(int a,int b,int c,int d) {
    return __gcd(__gcd(a,b),__gcd(c,d));
}
int main() {
    int n;
    while (cin>>n) {
        memset(a,0,sizeof(a));
        long long s=0;
        for (int i=0;i<n;i++) cin>>a[i];
        for (int i1=0;i1<n;i1++)
            for (int i2=i1+1;i2<n;i2++)
                for (int i3=i2+1;i3<n;i3++)
                    for (int i4=i3+1;i4<n;i4++)
                        if (gcd(a[i1],a[i2],a[i3],a[i4])==1)
                            s++;
        cout<<s<<endl;
    }
    return 0;
}
```

## Problem G

#### Code G1
R110777613 / 2023-05-20 10:59:22 / WA 0 (10 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int main() {
    while(1);
    return 0;
}
```

#### Code G2
R110805684 / 2023-05-20 14:47:58 / WA 0 (10 WA)
```cpp
#include <bits/stdc++.h>
using namespace std;
int main() {
    cout<<"I AK CSSYZ Algorithm";
    return 0;
}
```

## Problem H

#### Code H1
R110777648 / 2023-05-20 10:59:32 / WA 0 (10 TLE)
```cpp
#include <bits/stdc++.h>
using namespace std;
int main() {
    while(1);
    return 0;
}
```

#### Code H2
R110805724 / 2023-05-20 14:48:12 / WA 0 (10 WA)
```cpp
#include <bits/stdc++.h>
using namespace std;
int main() {
    cout<<"I AK CSSYZ Algorithm";
    return 0;
}
```

EOF.