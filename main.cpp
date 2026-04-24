#include <bits/stdc++.h>
using namespace std;

/* See README for problem interpretation.
   We compute biconnected components (BCCs) in the given undirected graph G of m edges.
   A vertex can be invited iff it lies in some odd cycle, i.e., in a non-bipartite BCC.
   We'll implement an iterative Tarjan-style DFS to avoid recursion stack overflow.
*/

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n,m; if(!(cin>>n>>m)) return 0;
    vector<vector<int>> adj(n+1);
    adj.shrink_to_fit();
    for(int i=0;i<m;i++){
        int x,y; cin>>x>>y;
        if(x==y) continue;
        adj[x].push_back(y);
        adj[y].push_back(x);
    }

    vector<int> tin(n+1, -1), low(n+1, -1), parent(n+1, -1), itIdx(n+1, 0);
    vector<pair<int,int>> edgeStack; edgeStack.reserve(m);
    vector<char> inOdd(n+1, 0);
    int timer = 0;

    struct Frame{int u;};

    for(int s=1; s<=n; ++s){
        if(tin[s]!=-1) continue;
        // start new DFS at s
        vector<int> stackNodes; stackNodes.reserve(1024);
        parent[s] = -1; itIdx[s] = 0; stackNodes.push_back(s);
        while(!stackNodes.empty()){
            int u = stackNodes.back();
            if(tin[u]==-1){
                tin[u]=low[u]=++timer;
            }
            if(itIdx[u] < (int)adj[u].size()){
                int v = adj[u][ itIdx[u]++ ];
                if(tin[v]==-1){
                    edgeStack.emplace_back(u,v);
                    parent[v]=u; itIdx[v]=0; stackNodes.push_back(v);
                    continue;
                } else if(v!=parent[u] && tin[v] < tin[u]){
                    edgeStack.emplace_back(u,v);
                    low[u] = min(low[u], tin[v]);
                }
                // loop continues on same u to process next neighbor
            } else {
                // finishing u
                stackNodes.pop_back();
                int p = parent[u];
                if(p != -1){
                    // form BCC if needed
                    if(low[u] >= tin[p]){
                        // Extract component edges until (p,u) and check bipartiteness via DSU with parity
                        struct DSU {
                            unordered_map<int,int> par, rnk, pr; // pr: parity from node to parent
                            pair<int,int> findp(int x){
                                auto it = par.find(x);
                                if(it==par.end()){
                                    par[x]=x; rnk[x]=0; pr[x]=0; return {x,0};
                                }
                                if(it->second==x) return {x, pr[x]};
                                auto res = findp(it->second);
                                par[x]=res.first; pr[x]^=res.second; return {par[x], pr[x]};
                            }
                            bool unite_diff(int a, int b){
                                auto pa = findp(a); auto pb = findp(b);
                                int ra=pa.first, rb=pb.first, xa=pa.second, xb=pb.second;
                                if(ra==rb){
                                    return (xa ^ xb) == 1; // must be different
                                }
                                // attach smaller rank under larger
                                int ra_r = rnk[ra]; int rb_r = rnk[rb];
                                if(ra_r < rb_r){
                                    par[ra]=rb; pr[ra]= xa ^ xb ^ 1; // parity from ra to rb
                                } else if(ra_r > rb_r){
                                    par[rb]=ra; pr[rb]= xa ^ xb ^ 1; // need to compute from rb to ra => same expression
                                } else {
                                    par[rb]=ra; pr[rb]= xa ^ xb ^ 1; rnk[ra]=ra_r+1;
                                }
                                return true;
                            }
                        } dsu;
                        bool bip = true;
                        unordered_set<int> compVerts; compVerts.reserve(16);
                        while(!edgeStack.empty()){
                            auto e = edgeStack.back(); edgeStack.pop_back();
                            compVerts.insert(e.first); compVerts.insert(e.second);
                            if(bip){ if(!dsu.unite_diff(e.first, e.second)) bip=false; }
                            if(e.first==p && e.second==u) break;
                        }
                        if(!bip){
                            for(int vtx: compVerts) inOdd[vtx]=1;
                        }
                    }
                    low[p] = min(low[p], low[u]);
                }
            }
        }
    }

    int cannot=0; for(int i=1;i<=n;i++) if(!inOdd[i]) cannot++;
    cout<<cannot<<"\n";
    return 0;
}
