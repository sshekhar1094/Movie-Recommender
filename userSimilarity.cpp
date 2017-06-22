//This program finds similarity between users by pearson correlation and store the similarity result in the text file userSimilarity.txt 
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

ifstream fin;
ofstream fout;

char 	training[] = "dataset/ua.base.txt",
		test[] = "dataset/ua.test.txt",
		items[] = "dataset/u.item.txt";
		//training data contains userid, itemid, rating, timestamp.

map< int,  vector< pair<int, float> > > ratings;				
//ratings[userId][i].[itemId, rating]

map< int, string > itemDetails;
//itemDetails[itemId].[itemName]

float inputMatrix[1000][2000] = {0};
float userSimilarity[1000][1000] = {0};
float avgRating[1000] = {0};	//avg rating by each user
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

	/*cout<<"No of users = "<<ratings.size()<<endl;
	for(int i=0; i<5; i++){
		cout<<"User "<<i<<endl;
		for(int j=0; j<ratings[i].size(); j++){
			cout<<"\t"<<"item= "<<ratings[i][j].first<<", rating = "<<ratings[i][j].second;
		}
	}*/

}

float calcAvgRating(int userId){
	float total = (float)ratings[userId].size();
	float sum = (float)0;
	for(int i=0; i<ratings[userId].size(); i++)
		sum = sum + (float)ratings[userId][i].second;
	float avg = sum/total;
	return avg;
}

void findUserSimilarity(){
	float num, denom1, denom2;
	int common=0;
	fout.open("userSimilarity2.txt");
	for(int i=1; i<=ratings.size(); i++){		//ratings.size gives total no of users
		for(int j=1; j<=ratings.size(); j++){
			if(i==j) continue;			//sim[i,i] = 0;
			else if(userSimilarity[j][i] != 0) userSimilarity[i][j] = userSimilarity[j][i];			//if already calculated, since transverse matrix
			else{
				denom1=0;denom2=0;
				num=0; common=0;
				for(int k=0; k<ratings[i].size(); k++){
					if(inputMatrix[j][ ratings[i][k].first ] != 0){
						//i & j both have rated item ratings[i,k].first
						//cout<<i<<":"<<j<<":"<<k<<"\t";
						common++;
						float avg_i = avgRating[i], avg_j = avgRating[j];
						num = num + (float)(inputMatrix[i][ratings[i][k].first] - avg_i) * (float)(inputMatrix[j][ratings[i][k].first] - avg_j);
						denom1 = denom1 + (float)(inputMatrix[i][ratings[i][k].first] - avg_i) * (float)(inputMatrix[i][ratings[i][k].first] - avg_i);
						denom2 = denom2 + (float)(inputMatrix[j][ratings[i][k].first] - avg_j) * (float)(inputMatrix[j][ratings[i][k].first] - avg_j);
					}
				}
				denom1 = sqrt(denom1);
				denom2 = sqrt(denom2);
				userSimilarity[i][j] = num/(denom1 * denom2);
				userSimilarity[i][j] *= ((float)min(50, common))/(float)50;
			}
			//cout<<i<<":"<<j<<":"<<userSimilarity[i][j]<<endl;
			if(num==0 || (denom1 + denom2 == 0) || common==0 || userSimilarity[i][j]!=userSimilarity[i][j]) fout<<i<<" "<<j<<" "<<"0"<<endl;
			else fout<<i<<" "<<j<<" "<<userSimilarity[i][j]<<endl;
		}
		if(i%100 == 0) cout<<i<<endl;
	}
	fout.close();
}

int main(int argc, char *argv[]){
	getData();
	for(int i=1; i<ratings.size(); i++)
		avgRating[i] = calcAvgRating(i);
	findUserSimilarity();
	return 0;
}