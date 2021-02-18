#include<bits/stdc++.h>
using namespace std;

struct node
{
	int f;
	unordered_map <string,node*> child;
	string label;
	string name;
};

int same_class(vector <vector<string>> &db, int &n, int &m)
{
	string dv=db[n-1][0];
	for(int i=1;i<m;i++)
	{
		if(dv!=db[n-1][i])
			return 0;
	}
	return 1;
}

vector <vector<string>> transpose(vector <vector<string>> db)
{
	int n=db.size(),m=db[0].size();
	vector <vector<string>> v(m,vector<string>(n,""));
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
			v[j][i]=db[i][j];
	}
	return v;
}

string majority_class(vector <vector<string>> &db, int &n, int &m)
{
	int cnt=0;
	string dv,t;
	unordered_map <string,int> clas;
	for(int i=0;i<m;i++)
	{
		t=db[0][i];
		if(++clas[t]>cnt)
			cnt=clas[t],dv=t;
	}
	return dv;
}

int ig_parent(vector <vector<string>> &db, int &n, int &m)
{
	double s=0;
	unordered_map <string,int> cnt;
	for(int i=0;i<m;i++)
		cnt[db[n-1][i]]++;
	for(auto &i:cnt)
	{
		double f=(1.0*i.second)/m;
		s-=f*log2(f);
	}
	return s;
}

int ig_child(vector <vector<string>> &db, int n, int m, int &i, string s)
{
	double s1=0,m1=0;
	unordered_map <string,int> cnt;
	for(int k=0;k<m;k++)
	{
		if(db[i][k]==s)
			cnt[db[n-1][k]]++,m1++;
	}
	for(auto &j:cnt)
	{
		double f=(1.0*j.second)/m1;
		s1-=f*log2(f);
	}
	return s1;
}

void attribute_selection_method(vector <vector<string>> &db, int &n, int &m, unordered_map <string,vector<vector<string>>> &count, int &f)
{
	double p,diff=INT_MIN;
	p=ig_parent(db,n,m);
	for(int i=0;i<n-1;i++)
	{
		double s=0,split_inf=0,val;
		unordered_map <string,int> cnt;
		for(int j=0;j<m;j++)
			cnt[db[i][j]]++;
		for(auto &j:cnt)
		{
			double fr=(1.0*j.second)/m;
			s+=(fr)*ig_child(db,n,m,i,j.first);
			split_inf-=fr*log2(fr);
		}
		val=(p-s)/split_inf;
		if(val>diff)
			diff=val,f=i;
	}
	for(int j=0;j<m;j++)
	{
		vector <string> eg;
		for(int k=0;k<n;k++)
		{
			if(k!=f)
				eg.push_back(db[k][j]);
		}
		count[db[f][j]].push_back(eg);
	}
}

node* generate_decision_tree(vector <vector<string>> &db, int n, int m, int cntr)
{
	node *t=new node;
	if(same_class(db,n,m))
	{
		t->f=-1;
		t->label=db[n-1][0];
	}
	else if(n==1)
	{
		t->f=-1;
		t->label=majority_class(db,n,m);
	}
	else
	{
		unordered_map <string,vector<vector<string>>> cnt;
		int attr;
		attribute_selection_method(db,n,m,cnt,attr);
		t->f=attr+cntr;
		for(auto &i:cnt)
		{
			vector <vector<string>> db1=transpose(i.second);
			t->child[i.first]=generate_decision_tree(db1,db1.size(),db1[0].size(),cntr+1);
			t->child[i.first]->name=i.first;
		}
	}
	return t;
}

void dfs(node *t, int x)
{
	for(int i=0;i<x;i++)
		cout<<"\t";
	cout<<"-"<<t->name<<"\n";
	for(auto &i:t->child)
		dfs(i.second,x+1);
}

vector <vector<string>> read_txt(ifstream &fin, int &cnt)
{
	string s;
	vector <vector<string>> db;
	while(getline(fin,s))
	{
		int i=0,n=s.length();
		cnt=1;
		while(i++<n)
		{
			if(s[i-1]==' ')
				break;
		}
		vector <string> attr;
		string t;
		while(i<n)
		{
			if(i+1==n)
				attr.push_back(t+s[i++]);
			else if(s[i]==' ')
			{
				attr.push_back(t);
				t.clear();
				i++,cnt++;
			}
			else
				t+=s[i++];
		}
		db.push_back(attr);
	}
	return db;
}

void predict_decision_tree(vector <vector<string>> db, int n, int m, node *t, vector <vector<string>> &pred)
{
	if(!t)
		return;
	else if(t->f==-1)
	{
		db.push_back(vector <string> (m,t->label));
		db=transpose(db);
		for(auto &i:db)
			pred.push_back(i);
		return;
	}
	int j=t->f;
	map <string,vector<vector<string>>> cnt;
	vector <vector<string>> temp=transpose(db);
	for(int i=0;i<temp.size();i++)
		cnt[temp[i][j]].push_back(temp[i]);
	for(auto &k:cnt)
	{
		string s=k.first;
		temp=transpose(k.second);
		int n1=temp.size(),m1=temp[0].size();
		predict_decision_tree(temp,n1,m1,t->child[s],pred);
	}
}

void accuracy_dt(vector <vector<string>> db, int n, int m)
{
	double correct=0,cnt=0;
	for(int i=0;i<n;i++)
	{
		if(db[i][m-1]==db[i][m-2])
			correct++;
		cnt++;
	}
	cout<<"***************************************************\n";
	cout<<"\tTesting Samples\n\n";
	cout<<"Age | Income | Student | Credit_Rating | Class(buys_computer) : Prediction\n\n";
	for(auto &i:db)
	{
		for(int j=0;j<m-1;j++)
			cout<<i[j]<<" | ";
		cout<<" : " <<i[m-1]<<"\n";
	}
	cout<<"\nAccuracy of predictions: "<<correct/cnt;
}

int main()
{
	ifstream train("train.txt",ios::in),test("test.txt",ios::in),predict("predict.txt",ios::in);
	string s;
	int cnt_tr,cnt_te,cnt_pre,m;
	vector <vector<string>> db_tr=read_txt(train,cnt_tr),db_te=read_txt(test,cnt_te),db_pre=read_txt(predict,cnt_pre),a1,a2;
	db_tr=transpose(db_tr);
	m=db_tr[0].size();
	node *t=generate_decision_tree(db_tr,cnt_tr,m,0);
	t->name="database";
	cout<<"DFS of the Decision Tree Produced\n\n";
	dfs(t,0);
	cout<<"\n";
	predict_decision_tree(transpose(db_te),cnt_te,db_te.size(),t,a1);
	accuracy_dt(a1,a1.size(),a1[0].size());
	cout<<"\n\n***************************************************\n";
	predict_decision_tree(transpose(db_pre),cnt_pre,db_pre.size(),t,a2);
	cout<<"\tPredicting Samples\n\n";
	cout<<"Age | Income | Student | Credit_Rating | Class(buys_computer) : Prediction\n\n";
	for(auto &i:a2)
	{
		for(int j=0;j<i.size()-1;j++)
			cout<<i[j]<<" | ";
		cout<<" : "<<i[i.size()-1]<<"\n";
	}
	return 0;
}
