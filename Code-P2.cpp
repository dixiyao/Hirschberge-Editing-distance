//C++11
//Jimmyyao 
//utf-8
#include<iostream>
#include<algorithm>
#include<map>
#include<fstream>
#include<map>
#include<vector>
using namespace std;

class Mismatch{
private:
	map<string,int> maps;
public:
	Mismatch(){
		maps["--"]=0;maps["A-"]=1;maps["T-"]=2;maps["G-"]=1;maps["C-"]=3;
		maps["-A"]=1;maps["AA"]=0;maps["TA"]=1;maps["GA"]=5;maps["CA"]=1;
		maps["-T"]=2;maps["AT"]=1;maps["TT"]=0;maps["GT"]=9;maps["CT"]=1;
		maps["-G"]=1;maps["AG"]=5;maps["TG"]=9;maps["GG"]=0;maps["CG"]=1;
		maps["-C"]=3;maps["AC"]=1;maps["TC"]=1;maps["GC"]=1;maps["CC"]=0;
	}
	int get(char a, char b){
		string tmp;
		tmp+=a;
		tmp+=b;
		return maps[tmp];
	}
}; 

vector<int> f(string s1,string s2,Mismatch mismatch){
	int m=s1.length(),n=s2.length();
	vector<int> dp(m+1,INT_MAX);s
	dp[0]=0;
	for(int i=1;i<m+1;i++)
		dp[i]=mismatch.get(s1[i-1],'-')+dp[i-1];
	for(int j=1;j<n+1;j++){
		int nextuse=dp[0];
		dp[0]=dp[0]+mismatch.get('-',s2[j-1]);
		for(int i=1;i<m+1;i++){
			int tmp=dp[i];
			dp[i]=min(mismatch.get(s1[i-1],s2[j-1])+nextuse,mismatch.get('-',s2[j-1])+dp[i]);
			dp[i]=min(dp[i],mismatch.get(s1[i-1],'-')+dp[i-1]);
			nextuse=tmp;
		}
	}	
	return dp;
}

int get_q(vector<int> left,vector<int> right){
	int q=-1;int cost=INT_MAX;
	int len=left.size();
	for(int i=0;i<left.size();i++){
		if(left[i]+right[len-i-1]<cost){
			q=i;
			cost=left[i]+right[len-i-1];
		}
	}
	return q;
}

pair<string,string>dynamicProgramming(string s1,string s2,Mismatch mismatch){
	vector<vector<int>> dp(s1.length()+1,vector<int>(s2.length()+1,INT_MAX));
	vector<vector<char>>  path(s1.length()+1,vector<char>(s2.length()+1));
	dp[0][0]=0;
	for(int i=1;i<s1.length()+1;i++){
		dp[i][0]=dp[i-1][0]+mismatch.get(s1[i-1],'-');
		path[i][0]='u';
	} 
	for(int j=1;j<s2.length()+1;j++){
		dp[0][j]=dp[0][j-1]+mismatch.get('-',s2[j-1]);
		path[0][j]='l';
	}
	for(int i=1;i<s1.length()+1;i++)
		for(int j=1;j<s2.length()+1;j++){
			int c1,c2,c3,c;
			c1=mismatch.get(s1[i-1],s2[j-1])+dp[i-1][j-1];
			c2=mismatch.get('-',s2[j-1])+dp[i-1][j];
			c3=mismatch.get(s1[i-1],'-')+dp[i][j-1];
			c=min(min(c1,c2),c3);
			if(c==c1)path[i][j]='d';
			else{
				if(c==c2)path[i][j]='u';
				else path[i][j]='l';	
			}
			dp[i][j]=c;
	}
	string row="",column="";
	int i=s1.length(),j=s2.length();
	while(i>=1 ||j>=1){
		if(path[i][j]=='d'){
			string tmp;
			tmp=s2[j-1];
			row.insert(0,tmp);
			tmp=s1[i-1];
			column.insert(0,tmp);
			i--;
			j--;
		}
		else{
			if(path[i][j]=='u'){
				row.insert(0,"-");
				string tmp;
				tmp=s1[i-1];
				column.insert(0,tmp);
				i--;
			}
			else{
				if(path[i][j]=='l'){
					column.insert(0,"-");
					string tmp;
					tmp=s2[j-1];
					row.insert(0,tmp);
					j--;					
				}
			}
		}
	}
	pair<string,string> ans;
	ans.first=row;
	ans.second=column;
	return ans;
}

pair<pair<string,string>,int> Hirscheberg(string s1, string s2,Mismatch mismatch){
	string row="",column="";
	int cost=0;
	if(s1.length()==0||s2.length()==0){
		if(s1.length()==0){
			for(int i=0;i<s2.length();i++)column.append("-");
			row=s2;
		}
		else{
			for(int i=0;i<s1.length();i++)row.append("-");
			column=s1;
		}
	}
	else{
		if(s1.length()==1||s2.length()==1){
			pair<string,string> tmp;
			tmp=dynamicProgramming(s1,s2,mismatch);
			row=tmp.first;
			column=tmp.second;
		}
		else{
			int n=s2.length(),m=s1.length();
			int s2mid=n/2;
			string tmp1,tmp2;
			vector<int> left(m+1),right(m+1);
			left=f(s1,s2.substr(0,s2mid),mismatch);
			tmp1=s1;reverse(tmp1.begin(),tmp1.end());
			tmp2=s2.substr(s2mid);reverse(tmp2.begin(),tmp2.end());
			right=f(tmp1,tmp2,mismatch);
			int s1mid=get_q(left,right);
			cost=left[s1mid]+right[m-s1mid];
			pair<pair<string,string>,int> lhalf,rhalf;
			lhalf=Hirscheberg(s1.substr(0,s1mid),s2.substr(0,s2mid),mismatch);
			rhalf=Hirscheberg(s1.substr(s1mid),s2.substr(s2mid),mismatch);
			string row_l,row_r,column_u,column_d;
			row_l=lhalf.first.first;column_u=lhalf.first.second;
			row_r=rhalf.first.first;column_d=rhalf.first.second;
			row.append(row_l);row.append(row_r);
			column.append(column_u);column.append(column_d);
		}
	}
	pair<pair<string,string>,int> ans;
	ans.first.first=row;
	ans.first.second=column;
	ans.second=cost;
	return ans;
}


int main(){
	/* initialization */
	ifstream infile;
	string s1,s2;
	infile.open("Data-P2a.txt");
	infile>>s1;
	infile.close();
	infile.open("Data-P2b.txt");
	infile>>s2;
	Mismatch mismatch;	
	 
	/* calculate */
	pair<pair<string,string>,int> ans;
	ans=Hirscheberg(s1,s2,mismatch);
	int cost=0;
	cost=ans.second;
	s1=ans.first.first;
	s2=ans.first.second;
	
	/* write answer */ 
	ofstream outfile;
	outfile.open("Answer-P20.txt");
	outfile<<"cost is "<<cost<<endl;
	outfile.close();
	outfile.open("Answer-P2a0.txt");
	outfile<<s1;
	outfile.close();
	outfile.open("Answer-P2b0.txt");
	outfile<<s2;
	outfile.close();
	return 0;
}
