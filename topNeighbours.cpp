//This program stores the top 30 neighbours of each user in the file topNeighbours.txt
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <map>
#include <queue>
#include <algorithm>
#include <iostream>
#include <climits>
#include <set>
#include <utility>
#include <fstream>
#include <sstream>
using namespace std;

int no_neighbours = 300;

ifstream fin;
ofstream fout;

char 	training[] = "dataset/ua.base.txt",	
		test[] = "dataset/ua.test.txt",
		items[] = "dataset/u.item.txt",
		similaritytxt[] = "userSimilarity.txt";


map< int, vector< pair<int, float> > > ratings;				
//ratings[userId][i].[itemId, rating]

map< int, string > itemDetails;
//itemDetails[itemId].[itemName]

map< int, vector< pair<float, int> > > topNeighbours;
//topNeighbours[u] is a vector of top neighbours of user u

float inputMatrix[1000][2000] = {0};
float userSimilarity[1000][1000] = {0};
float itemSimilarity[2000][2000] = {0};
//similarity[i][j] = simarity btwn objects i and j (item or user)

void getData(){

	char line[500];
	string s;
	long long timestamp, user, item, rating;
	pair<int, float> pr;

	//getting the user-item-rating matrix
	fin.open("dataset/ua.base.txt");
	while(!fin.eof()){
		fin>>user;
		fin>>item;
		fin>>rating;
		fin>>timestamp;
		inputMatrix[user][item] = rating;
		pr = make_pair(item, rating);
		ratings[user].push_back(pr);
	}
	fin.close();
	cout<<"Matrix built\n";

	/*cout<<"No of users = "<<ratings.size()<<endl;
	for(int i=0; i<5; i++){
		cout<<"User "<<i<<endl;
		for(int j=0; j<ratings[i].size(); j++){
			cout<<"\t"<<"item= "<<ratings[i][j].first<<", rating = "<<ratings[i][j].second;
		}
	}*/

	//getting the item-itemName mapping
	fin.open("dataset/u.item.txt");
	while(fin.getline(line, 500)){
		stringstream ss(line);
		getline(ss, s, '|');
		char str[100];
		for(int i=0; s[i]!='\0'; i++) str[i] = s[i];
		item = atoi(str);
		getline(ss, s, '|');
		itemDetails[item] = s;
		//Skipping over other values as not using them
	}
	fin.close();
	cout<<"user details taken\n";

	/*cout<<"No of items="<<itemDetails.size();
	for(int i=0; i<15; i++)
		cout<<i<<":"<<itemDetails[i]<<endl;*/

	//getting the user similarities
	fin.open(similaritytxt);
	int user1, user2;
	float rat;
	int z=0;
	cout<<"Finding Similarity...";
	while(fin>>user1>>user2>>rat){
		rat = abs(rat);
		userSimilarity[user1][user2] = rat;
		if(rat > 0){
			topNeighbours[user1].push_back(make_pair(rat, user2));
			//if(user2>1000 || user2<0) cout<<user1<<":"<<user2<<endl;
		}
		if(z%200000 == 0) cout<<".";
		z++;
	}
	fin.close();

}

void getTopNeighbours(){
	//Finding the top neighbours of each user
	cout<<"\nSorting...";
	fout.open("topNeighbours.txt");
	for(int i=1; i<=ratings.size(); i++){
		sort(topNeighbours[i].rbegin(), topNeighbours[i].rend());
		fout<<i<<endl;
		for(int j=0; j<no_neighbours; j++){
			fout<<topNeighbours[i][j].first<<" "<<topNeighbours[i][j].second<<endl;
		}
		//if(i%100 == 0) cout<<i<<":"<<topNeighbours[i][0].first<<":"<<topNeighbours[i][940].first<<endl;
	}
	fout.close();
	//We will be considering only the top 30  neighbours of each user for our prediction

}



int main(int argc, char *argv[]){
	getData();
	getTopNeighbours();
	cout<<endl<<"Done!\n";
	return 0;
}