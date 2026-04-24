#include <bits/stdc++.h>
using namespace std;

/*
We have n ministers labeled 1..n. Among all pairs, almost all have mutual sightings (edges in the complement graph). Input gives m pairs (x,y) that have NOT seen each other. So we are given graph G on n nodes with edges = m pairs; these edges represent allowed adjacency and also the "friendship" pairs that could form groups.

Constraints: At a banquet, selected ministers sit in a circle with adjacency rules:
- Two ministers who HAVE seen each other (i.e., not in input pair) cannot be seated adjacent. Equivalently, adjacent pairs in the circle must be from the given m pairs (non-sighting pairs). So the circle must be a cycle in G.
- To prevent pairwise "mutual help groups", king invites an odd number of ministers (not 1). So the cycle length must be an odd number >=3.

Question: Count ministers who can NEVER be invited in any valid banquet (i.e., those not lying in any odd cycle of G).

Thus the answer is n minus the number of vertices that belong to at least one odd cycle of the given graph G.

Observation: A vertex belongs to some odd cycle iff it lies in a non-bipartite biconnected component (block) of G, or more generally, vertices that are in the odd-cycle vertex set. In undirected graphs, a vertex is in some odd cycle iff it belongs to a block that is non-bipartite (contains an odd cycle). Articulation vertices that connect components can belong to odd cycle if that block is non-bipartite.

Approach:
- Build G with m edges undirected.
- Compute biconnected components using Tarjan (stack of edges).
- For each BCC, test if it has an odd cycle: equivalently, check if the induced subgraph is bipartite. Since BCC can be processed by painting within edges in the component. If any edge connects same color -> not bipartite -> BCC has odd cycle.
- If a BCC is non-bipartite, then all vertices in this BCC are in some odd cycle (within that BCC). Mark them.
- Finally count unmarked vertices -> cannot be invited.

Complexities O(n+m).
*/

struct Edge {int u,v;};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n,m; if(!(cin>>n>>m)) return 0;
    vector<vector<int>> adj(n+1);
    vector<pair<int,int>> edges; edges.reserve(m);
    for(int i=0;i<m;i++){
        int x,y; cin>>x>>y;
        if(x==y) continue; // ignore self-loops; though not expected
        adj[x].push_back(y);
        adj[y].push_back(x);
        edges.emplace_back(min(x,y), max(x,y));
    }

    // Tarjan for BCCs
    int timer=0; 
    vector<int> tin(n+1, -1), low(n+1, -1);
    vector<pair<int,int>> st; st.reserve(m);
    vector<char> inOdd(n+1, 0);

    function<void(int,int)> dfs = [&](int u, int p){
        tin[u]=low[u]=++timer;
        int child=0;
        for(int v: adj[u]){
            if(v==p) continue; // but multiple edges? none specified; assume simple
            if(tin[v]==-1){
                st.emplace_back(u,v);
                dfs(v,u);
                low[u]=min(low[u], low[v]);
                if(low[v] >= tin[u]){
                    // pop edges until (u,v) to form a BCC
                    vector<pair<int,int>> compEdges;
                    vector<int> compVerts;
                    unordered_set<int> seen;
                    while(!st.empty()){
                        auto e = st.back(); st.pop_back();
                        compEdges.push_back(e);
                        if(!seen.count(e.first)){ seen.insert(e.first); compVerts.push_back(e.first);} 
                        if(!seen.count(e.second)){ seen.insert(e.second); compVerts.push_back(e.second);} 
                        if(e.first==u && e.second==v){
                            break;
                        }
                    }
                    // Check bipartiteness within this component using BFS over comp subgraph
                    // Build adjacency list for comp
                    unordered_map<int, vector<int>> compAdj;
                    compAdj.reserve(compVerts.size()*2+1);
                    for(auto &e: compEdges){
                        compAdj[e.first].push_back(e.second);
                        compAdj[e.second].push_back(e.first);
                    }
                    unordered_map<int,int> color;
                    bool bip = true;
                    for(int s: compVerts){
                        if(color.find(s)!=color.end()) continue;
                        queue<int> q; q.push(s); color[s]=0;
                        while(!q.empty() && bip){
                            int x=q.front(); q.pop();
                            auto it = compAdj.find(x);
                            if(it==compAdj.end()) continue;
                            for(int y: it->second){
                                auto itc = color.find(y);
                                if(itc==color.end()){
                                    color[y]=color[x]^1;
                                    q.push(y);
                                } else if(itc->second==color[x]){
                                    bip=false; break;
                                }
                            }
                        }
                        if(!bip) break;
                    }
                    if(!bip){
                        for(int a: compVerts) inOdd[a]=1;
                    }
                }
            } else if(tin[v] < tin[u]){
                // back edge
                st.emplace_back(u,v);
                low[u]=min(low[u], tin[v]);
            }
        }
    };

    for(int i=1;i<=n;i++) if(tin[i]==-1) dfs(i,-1);

    int cannot=0; for(int i=1;i<=n;i++) if(!inOdd[i]) cannot++;
    cout<<cannot<<"\n";
    return 0;
}
