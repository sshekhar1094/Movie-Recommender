// Recommendation system program which trains itself against 90,000 data and then predicts ratings of 10,000 data finally calculating mean average error
// Then asks the user to rate a set of most popular movies and based on it gives a recommendation of 20 movies.

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

#define totalData 9430

ifstream fin;
ofstream fout;

char 	training[] = "dataset/ua.base.txt",	
		test[] = "dataset/ua.test.txt",
		similaritytxt[] = "userSimilarity.txt";


map< int, vector< pair<int, float> > > ratings;				
//ratings[userId][i].[itemId, rating]

map< int, vector< int > > topNeighbours;
//topNeighbours[u] is a vector of top neighbours of user u

float testActual[1000][2000], testPredicted[1000][2000];
//Stores the actual and predicted values of user-item ratings

map<int, vector<int> > itemList;
//Stores the list of movies for each user to be checked for

map< int, string > itemDetails;
//itemDetails[itemId].[itemName]
vector< pair<int, int> > mostRated;
//Contains the count of no of ratings of each movie
int dummy[2000] = {0};

float inputMatrix[1000][2000] = {0};
float userSimilarity[1000][1000] = {0};
//similarity[i][j] = simarity btwn objects i and j (item or user)

float avgRating[1000]; //Stores the average rating by each user.
vector<pair<float, int> > neighbours, rec;
float sim[1000] = {0};

void getData(){

	char line[500];
	string s;
	pair<long long, long long> pr;
	long long timestamp, user, item, rating;

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
		dummy[item]++;	
	}
	fin.close();
	for(int i=0; i<2000; i++) mostRated.push_back(make_pair( dummy[i], i ));	
	//cout<<"Matrix built\n";

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

	/*cout<<"No of items="<<itemDetails.size();
	for(int i=0; i<15; i++)
		cout<<i<<":"<<itemDetails[i]<<endl;*/

	//getting the top 30 user similarities
	fin.open("topNeighbours.txt");
	int user1, user2;
	float rat;
	while(!fin.eof()){
		fin>>user1;
		for(int i=0; i<300; i++){
			fin>>rat>>user2;
			if(user2<=1 || user2>944) continue;
			topNeighbours[user1].push_back(user2);
			userSimilarity[user1][user2] = rat;
		}
		//cout<<user1<<" ";
	}
	fin.close();
	//cout<<"Similarity taken\n";

}

void getTestData(){
	long long timestamp;
	int user, item;
	float rating;
	fin.open("dataset/ua.test.txt");
	while(fin>>user>>item>>rating>>timestamp){
		testActual[user][item] = rating;
		itemList[user].push_back(item);
	}
	fin.close();
}

float calcAvgRating(int userId){
	float total = (float)ratings[userId].size();
	float sum = (float)0;
	for(int i=0; i<ratings[userId].size(); i++)
		sum = sum + (float)ratings[userId][i].second;
	float avg = sum/total;
	return avg;
}

void getAvg(){
	for(int i=1; i<=ratings.size(); i++)
		avgRating[i] = calcAvgRating(i);
}

float predictRating(int user, int item){
	float rating, num=0, denom=0, avg_user;
	rating = avgRating[user];

	//Finding numerator: summation( sim(user, i)*( rat[i,p] - avgRating[i] ) )
	//Denominator: summation( sim(user, i) )
	for(int i=1; i<=ratings.size(); i++){
		if(user == i) continue;
		num = num + (userSimilarity[user][i] * (inputMatrix[i][item] - avgRating[i]));
		denom = denom + abs(userSimilarity[user][i]);
	}
	if(denom==0) return 0;
	rating = num/denom;
	if(rating < 0) rating = rating * (-1);
	return rating;
}

float findMAE(){
	float mae = 0;
	for(int i=1; i<=ratings.size(); i++){
		for(int j=0; j<itemList[i].size(); j++){
			mae = mae + abs(testPredicted[i][ itemList[i][j] ] - testActual[i][ itemList[i][j] ]);
		}
	}
	mae = mae/((float)(totalData));
	return mae;
}

int main(int argc, char *argv[]){
	cout<<"Processing...";
	getData();
	getTestData();
	getAvg();
	for(int i=1; i<ratings.size(); i++){
		for(int j=0; j<itemList[i].size(); j++){
			testPredicted[i][ itemList[i][j] ] = predictRating(i, itemList[i][j]);
			//if(i%300 == 0) cout<<i<<":"<<itemList[i][j]<<":"<<testPredicted[i][ itemList[i][j] ]<<endl;
		}
	}
	float mae = findMAE();
	cout<<"\nMean average error is "<<mae<<endl;

	char c;
	cout<<"\nDo you want movie recommendations for you? (y/n):";
	cin>>c;
	float avg_user;
	if(c == 'y' || c == 'Y'){
		float userRating[2000];
		float rat;
		int no;
		cout<<"To give recommendations we need a dataset about your preferences and need you to rate some movies on a scale of 1 to 5\nHow many movies are you willing to rate at the moment?(minimum 10)\n";
		cin>>no;
		sort(mostRated.rbegin(), mostRated.rend());
		float sum=0;
		for(int i=0; i<no; i++){
			cout<<i+1<<"."<<itemDetails[mostRated[i].second]<<":";
			cin>>rat;
			sum = sum + rat;
			userRating[mostRated[i].second] = rat;
		}
		avg_user = sum/(float)(no);

		//Now finding similarity between our user and other users
		for(int i=1; i<944; i++){
			float num=0, denom1=0, denom2=0;
			int common=0;
			for(int j=0; j<no; j++){
				if(inputMatrix[i][mostRated[j].second] != 0){
					common++;
					num = num + (float)(userRating[mostRated[j].second] - avg_user) * (float)(inputMatrix[i][mostRated[j].second] - avgRating[i]);
					denom1 = denom1 + (float)(userRating[mostRated[j].second] - avg_user) * (float)(userRating[mostRated[j].second] - avg_user);
					denom2 = denom2 + (float)(inputMatrix[i][mostRated[j].second] - avgRating[i]) * (float)(inputMatrix[i][mostRated[j].second] - avgRating[i]);
				}
			}
			denom1 = sqrt(denom1);
			denom2 = sqrt(denom2);
			sim[i] = num/(denom1 * denom2);
			sim[i] *= ((float)min(5, common))/(float)5;
			if(sim[i] != sim[i]) sim[i] = 0;
			//if(i<20) cout<<num<<":"<<denom1<<":"<<denom2<<":"<<sim[i]<<endl;
			neighbours.push_back(make_pair(sim[i], i));
		}
		sort(neighbours.rbegin(), neighbours.rend());
		//for(int i=0; i<30; i++) cout<<neighbours[i].second<<":"<<sim[neighbours[i].second]<<":"<<neighbours[i].first<<endl;

		//Now taking the top 300 users and calculating the predicted rating for each item
		for(int i=1; i<1683; i++){
			if(userRating[i]) continue;
			float rating, num=0, denom=0;
			rating = avg_user;

			for(int j=0; j<300; j++){
				num = num + sim[ neighbours[j].second ] * (inputMatrix[neighbours[j].second][i] - avgRating[neighbours[j].second]);
				denom = denom + abs(sim[ neighbours[j].second ]);
				//if(i%100==0 && j%10==0)
					//cout<<i<<":"<<num<<":"<<denom<<endl;
			}
			rating = abs(num/denom);
			if(rating != rating) rating = 0;	//nan
			//if(i%100 == 0) cout<<i<<":"<<num<<":"<<denom<<endl;
			rec.push_back(make_pair(rating, i));

		}

		//Sort the rec to find top rated items
		sort(rec.rbegin(), rec.rend());

		//Now recommend the top 10 movies
		cout<<"\n\nTop movies for you:\n";
		for(int i=0; i<20; i++){
			//cout<<i+1<<"."<<itemDetails[rec[i].second]<<":"<<rec[i].first<<endl;
			cout<<i+1<<"."<<itemDetails[rec[i].second]<<endl;
		}
	}

	return 0;
}