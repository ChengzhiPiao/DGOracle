#include <iostream>
using namespace std;
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iomanip>

#define INPUT "original_graph/"
#define RESULT "mis/"
#define INST "instruction/"
#define INSTNUM 1000
#define SWAPUV {int tmp=u; u=v; v=tmp;}

struct edge_type{
    int st,ed,next;
};

struct node_type{
    int id,head,degree;
    int match;
    bool mis,del,sup;
};

struct bdtwo_info{
    int gap;
    int recycle;
    int misnum;
    double t;
};

struct inst_type{
    int type,u,v;
    inst_type(int t=-1, int st=0, int ed=0):type(t),u(st),v(ed){}
};

vector<edge_type> edge;
vector<node_type> g;
int misnum = 0;
vector<int> rubbish;
vector<bool> used;
vector<int> match_node;
int n,m;
int max_sup_misnum,sup_misnum,init_gap;
double sup_percent;

char FILEID[10] = "";
double percent = 1.0;

int visnum = 0;
double avgvisnum = 0.0;
int test_misnum;
bdtwo_info bd;
double init_time, test_time, del_time;

bool exist_edge(int u, int v)
{
    if(g[u].degree>g[v].degree) SWAPUV
    int j = g[u].head;
    while(j>=0)
    {
        if(edge[j].ed==v) return true;
        j = edge[j].next;
    }
    return false;
}

void add_edge(int u, int v)
{
    edge_type e;
    e.st = u;
    e.ed = v;
    e.next = g[u].head;
    g[u].head = edge.size();
    edge.push_back(e);
    e.st = v;
    e.ed = u;
    e.next = g[v].head;
    g[v].head = edge.size();
    edge.push_back(e);
    g[u].degree++;
    g[v].degree++;
}

void del_edge(int u, int v ,bool stop = false)
{
    if(g[u].match==v) g[u].match = -1;
    int j=g[u].head;
    if(j==-1)
    {
        cout << "DEL_NOT_EXIST" << endl;
        return;
    }
    if(edge[j].ed==v)
    {
        g[u].head = edge[j].next;
        g[u].degree--;
        edge[j].next = -1;
        if(!stop) del_edge(v,u,true);
        return;
    }
    int pre;
    while(edge[j].ed!=v && j>=0)
    {
        pre = j;
        j = edge[j].next;
    }
    if(j==-1)
    {
        cout << "DEL_NOT_EXIST" << endl;
        return;
    }
    edge[pre].next = edge[j].next;
    g[u].degree--;
    edge[j].next = -1;
    if(!stop) del_edge(v,u,true);
}

void del_v(int u)
{
    int j = g[u].head;
    while(j!=-1)
    {
        del_edge(u,edge[j].ed);
        j = g[u].head;
    }
    g[u].del = true;
    if(g[u].mis)
    {
        misnum--;
        g[u].mis = false;
    }
    if(g[u].sup)
    {
        sup_misnum--;
        g[u].sup = false;
    }
}

void init()
{
    clock_t startTime,endTime;
    startTime = clock();
    ifstream fin;
    char s[40] = INPUT;
    strcat(s,FILEID);
    fin.open(strcat(s,".in"));
    fin >> n >> m;
    g.resize(n);
    edge.reserve(m*2+INSTNUM*2);
    for(int i=0; i<n; i++)
    {
        g[i].id = i;
        g[i].head = g[i].match = -1;
        g[i].degree = 0;
        g[i].mis = g[i].del = false;
        g[i].sup = false;
    }
    for(int i=m; i>0; i--)
    {
        int u,v;
        fin >> u >> v;
        add_edge(u,v);
    }
    fin.close();
    used.resize(n);
    for(int i=0; i<n; i++)
        used[i] = false;
    char s2[40] = RESULT;
    strcat(s2,FILEID);
    fin.open(strcat(s2,".txt"));
    fin >> max_sup_misnum;
    sup_misnum = int(max_sup_misnum * percent);
    for(int i=0,u; i<sup_misnum; i++)
    {
        fin >> u;
        g[u].sup = true;
    }
    fin.close();
    endTime = clock();
    init_time = (double)(endTime - startTime) / CLOCKS_PER_SEC;
}

void extend_sup()
{
    for(int u=0; u<n; u++)
        if(!g[u].sup)
        {
            bool ok = true;
            int j = g[u].head;
            while(j>=0)
            {
                int v = edge[j].ed;
                if(g[v].sup)
                {
                    ok = false;
                    break;
                }
                j = edge[j].next;
            }
            if(ok)
            {
                g[u].sup = true;
                sup_misnum++;
            }
        }
    sup_percent = sup_misnum * 1.0 / max_sup_misnum;
    init_gap = max_sup_misnum - sup_misnum;
}

void bdone_del(int u, vector<int> &d, vector<bool> &del, vector<queue<int>> &que)
{
    del[u] = true;
    int j = g[u].head;
    while(j>=0)
    {
        int v = edge[j].ed;
        if(!del[v])
        {
            d[v]--;
            que[d[v]].push(v);
        }
        j = edge[j].next;
    }
}

int recycle_rubbish()
{
    int num = 0;
    for(int i=0; i<rubbish.size(); i++)
    {
        int u = rubbish[i];
        if(g[u].mis)
        {
            g[u].mis = false;
            misnum--;
        }
    }
    for(int i=0; i<rubbish.size(); i++)
    {
        bool flag = false;
        int u = rubbish[i];
        for(int j=g[u].head; j>=0; j=edge[j].next)
        {
            int v = edge[j].ed;
            if(g[v].mis)
            {
                flag = true;
                break;
            }
        }
        if(flag) continue;
        g[u].mis = true;
        misnum++;
        num++;
    }
    return num;
}


bdtwo_info sup_bdone_init()
{
    clock_t startTime,endTime;
    startTime = clock();
    vector<int> d;
    vector<bool> del;
    del.resize(n);
    for(int i=0; i<n; i++)
        del[i] = g[i].del;  //g[i].del==true means that node i has been deleted
    d.resize(n);
    int maxd = 0;
    for(int i=0; i<n; i++)
    {
        d[i] = g[i].degree;
        if(!del[i] && d[i]>maxd) maxd = d[i];
    }
    rubbish.clear();
    for(int i=0; i<n; i++)
        if(!del[i]) g[i].mis = false;
    misnum = 0;
    vector<queue<int>> que;
    que.resize(maxd+2);
    for(int i=0; i<n; i++)
        if(!del[i]) que[d[i]].push(i);
    while(true)
    {
        if(que[1].empty())
        {
            if(maxd<=1) break;
            if(que[maxd].empty()) maxd--;
            else{
                int u = que[maxd].front();
                que[maxd].pop();
                if(del[u]==false && d[u]==maxd && g[u].sup==false)
                {
                    rubbish.push_back(u);
                    bdone_del(u,d,del,que);
                }
            }
        }
        else{
            int u = que[1].front();
            que[1].pop();
            if(del[u] || d[u]==0) continue;
            g[u].mis = true;
            misnum++;
            int j = g[u].head;
            int v;
            while(j>=0)
            {
                v = edge[j].ed;
                if(!del[v]) break;
                j = edge[j].next;
            }
            g[u].match = v;
            g[v].match = u;
            bdone_del(u,d,del,que);
            bdone_del(v,d,del,que);
        }
    }
    while(!que[0].empty())
    {
        int u = que[0].front();
        que[0].pop();
        if(!del[u])
        {
            g[u].mis = true;
            misnum++;
            del[u] = true;
        }
    }
    bdtwo_info tmp;
    tmp.gap = rubbish.size();
    tmp.recycle = recycle_rubbish();
    tmp.misnum = misnum;
    //cout << "|SUP|\t= " << sup_misnum << endl;
    //cout << "|MIS|\t= " << tmp.misnum << endl;
    //cout << "gap\t= " << tmp.gap << endl;
    //cout << "recycle\t= " << tmp.recycle << endl;
    endTime = clock();
    tmp.t = (double)(endTime - startTime) / CLOCKS_PER_SEC;
    return tmp;
}

bool extend_match(int i)
{
    if(i==-1 || g[i].del || g[i].match>=0) return false;
    int mp = -1;
    int dmp = n;
    for(int j=g[i].head; j>=0; j=edge[j].next)
    {
        int v = edge[j].ed;
        if((g[i].mis!=g[v].mis) && (g[v].match==-1) && (g[v].degree<dmp))
        {
            mp = v;
            dmp = g[v].degree;
        }
    }
    if(mp>=0)
    {
        g[i].match = mp;
        g[mp].match = i;
        return true;
    }
    else return false;
}

void extend_match()
{
    for(int i=0; i<n; i++)
        if(g[i].mis && (g[i].match==-1)) extend_match(i);
}

void check()
{
    int num = 0;
    for(int i=0; i<n; i++)
    {
        if(g[i].mis)
        {
            num++;
            for(int j=g[i].head; j>=0; j=edge[j].next)
            {
                int v = edge[j].ed;
                if(g[v].mis)
                {
                    cerr << i << "\t" << v <<"\tMisCheck: WRONG" << endl;
                    //return;
                }
            }
        }
    }
    cout << "MisCheck: yes\t" << num << endl;
}

void check_sup()
{
    int num = 0;
    for(int i=0; i<n; i++)
    {
        if(g[i].sup)
        {
            num++;
            for(int j=g[i].head; j>=0; j=edge[j].next)
            {
                int v = edge[j].ed;
                if(g[v].sup)
                {
                    cout << i << "\t" << v <<"\tSupCheck: WRONG" << endl;
                    //return;
                }
            }
        }
    }
    cout << "SupCheck: yes\t" << num << endl;
}


bool replacing(int i)
{
    avgvisnum += 1;
    if(g[i].match==-1) return false;
    avgvisnum += 1;
    int u = g[i].match;
    for(int j=g[u].head; j>=0; j=edge[j].next)
    {
        int v = edge[j].ed;
        if((g[v].mis==false) && used[v]) return false;
    }
    match_node.push_back(u);
    used[u] = true;
    used[i] = true;
    for(int j=g[u].head; j>=0; j=edge[j].next)
    {
        int v = edge[j].ed;
        if(g[v].mis && (used[v]==false) && (replacing(v)==false)) return false;
    }
    return true;
}

bool do_replace(int i)
{
    visnum++;
    if(!replacing(i))
    {
        for(int j=0; j<match_node.size(); j++)
            used[match_node[j]] = used[g[match_node[j]].match] = false;
        match_node.clear();
        return false;
    }
    for(int j=0; j<match_node.size(); j++)
    {
        int v = match_node[j];
        int u = g[v].match;
        g[u].mis = false;
        g[v].mis = true;
        used[u] = used[v] = false;
    }
    match_node.clear();
    return true;
}

void clear_match(int u)
{
    int w = g[u].match;
    if(w>=0) g[w].match = g[u].match = -1;
}

void test_add(int u, int v)
{
    clock_t startTime,endTime;
    startTime = clock();
    add_edge(u,v);
    if(g[u].mis&&g[v].mis)
    {
        if((do_replace(u)==false) && (do_replace(v)==false))
        {
            clear_match(u);
            clear_match(v);
            if(g[u].degree>=g[v].degree) g[u].mis = false;
            else g[v].mis = false;
            misnum--;
        }
    }
    if(g[u].mis!=g[v].mis && g[u].match==-1 && g[v].match==-1)
    {
        g[u].match = v;
        g[v].match = u;
    }
    endTime = clock();
    test_time += (double)(endTime - startTime) / CLOCKS_PER_SEC;
}

bool attempt_true_node(int u)
{
    if(u==-1) return false;
    if(g[u].mis==true) return false;
    if(g[u].match>=0) return false;
    visnum++;
    used[u] = true;
    for(int j=g[u].head; j>=0; j=edge[j].next)
    {
        int v = edge[j].ed;
        if(g[v].mis && !replacing(v))
        {
            used[u] = false;
            for(int j=0; j<match_node.size(); j++)
                used[match_node[j]] = used[g[match_node[j]].match] = false;
            match_node.clear();
            return false;
        }
    }
    used[u] = false;
    g[u].mis = true;
    for(int j=0; j<match_node.size(); j++)
    {
        int v = match_node[j];
        int u = g[v].match;
        g[u].mis = false;
        g[v].mis = true;
        used[u] = used[v] = false;
    }
    match_node.clear();
    return true;
}

void test_del(int u, int v)
{
    clock_t startTime, endTime, del_startTime, del_endTime;
    startTime = clock();
    del_startTime = clock();
    del_edge(u,v);
    del_endTime = clock();
    del_time += ((double)(del_endTime - del_startTime) / CLOCKS_PER_SEC);
    if(attempt_true_node(u)) misnum++;
    if(attempt_true_node(v)) misnum++;
    extend_match(u);
    extend_match(v);
    endTime = clock();
    test_time += (double)(endTime - startTime) / CLOCKS_PER_SEC;
}

void test_delv(int u)
{
    clock_t startTime, endTime, del_startTime, del_endTime;
    startTime = clock();
    int v;
    if(g[u].del) return;
    v = g[u].match;
    del_startTime = clock();
    del_v(u);
    del_endTime = clock();
    del_time += ((double)(del_endTime - del_startTime) / CLOCKS_PER_SEC);
    if(attempt_true_node(v)) misnum++;
    extend_match(v);
    endTime = clock();
    test_time += (double)(endTime - startTime) / CLOCKS_PER_SEC;
}

void test_inst()
{
    vector<inst_type> inst;
    inst.resize(INSTNUM);
    char s[40] = INST;
    strcat(s,FILEID);
    ifstream fin;
    fin.open(strcat(s,".inst"));
    for(int i=0; i<INSTNUM; i++)
    {
        fin >> inst[i].type;
        if(inst[i].type==2) fin >> inst[i].u;
        else fin >> inst[i].u >> inst[i].v;
    }
    fin.close();
    test_time = del_time = 0.0;
    for(int i=0; i<INSTNUM; i++)
    {
        if(inst[i].type==0)
        {
            test_add(inst[i].u, inst[i].v);
        }
        else if(inst[i].type==1)
        {
            test_del(inst[i].u, inst[i].v);
        }
        else{
            test_delv(inst[i].u);
        }
    }
    test_misnum = misnum;
}

void output_base_ana()
{
    ofstream fout;
    fout.open("record.txt",ios::app);
    fout << FILEID << '\t';
    fout << bd.gap << '\t' << bd.recycle << '\t' << bd.misnum << '\t' << bd.t << '\t' << sup_misnum << endl;
    fout.close();
}

void output_update_ana()
{
    ofstream fout;
    fout.open("record.txt",ios::app);

    fout << FILEID << '\t';
    fout << fixed << setprecision(3) << sup_percent << '\t';
    fout << init_gap << '\t' << test_misnum << '\t';
    fout << (test_time-del_time)*(1000000/INSTNUM) << endl;

    //fout << FILEID << '\t' << init_gap << endl;
    //fout << FILEID << '\t' << avgvisnum/visnum << endl;
    fout.close();
}

void get_percent(char* str)
{
    if(strcmp(str,"99")==0) percent = 0.99;
    else if(strcmp(str,"90")==0) percent = 0.9;
    else if(strcmp(str,"75")==0) percent = 0.75;
    else if(strcmp(str,"50")==0) percent = 0.5;
    else percent = 1.0;
}

int main(int argc, char* argv[])
{
    if(argc>=2) strcpy(FILEID,argv[1]);
    else strcpy(FILEID,"1");
    if(argc>=3) get_percent(argv[2]);
    init();
    extend_sup();
    check_sup();
    bd = sup_bdone_init();
    extend_match();
    test_inst();
    check();
    output_update_ana();
    return 0;
}
