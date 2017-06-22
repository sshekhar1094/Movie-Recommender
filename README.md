# Recommendation-System
A collaborative filtering based recommendation system
This does not use any pre-existing library

The program uses [Pearson Correlation Function](https://en.wikipedia.org/wiki/Pearson_product-moment_correlation_coefficient#For_a_sample) to find similarity between every pair of users
The program trains itself against 90,000 test data which is a set of 1682 movies by 943 users and then tests itself against 10,000 test data, finally given the mean average error found over the test data.

It then asks the user if he wants some movie recommendations for himself, if yes then asks the user to rate a set of 10 most popular movies (10 movies rated by maximum users), and then based on the data obtained it predicts the rating of each of the other movies. It then gives a reccommendation of 20 movies with the highest ratings

Prediction function used for user *u* and item *p* =
```
	avgRating[u] + (for all users v )((summation of)( similarity[u, v]*(rating[v,p] - avgRating[v]) ) / (summation of)(similarity[u,v]))
```
where:
```
avgRating[u] = average rating by user *u*
```
```
similarity[u,v] = similarity between users *u* and *v* found by correlation function
```
```
rating[v,p] = rating of item *p* by user *u*
```

##Usage of different programs

The repository consists of 3 programs:
```
1. userSimilarity.cpp: This programs reads the user-item rating matrix from the data set and finds similarity between every pair of users using the Pearson Correlation. It then stores the calculated similarities in the text file userSimilarity.txt.
2. topNeighbours.cpp: This program reads the userSimilarities from the userSimilarity.txt file and finds the top 300 neighbours for each user storing the result in topNeighbours.txt
3. recommend.cpp: This is our main file. It reads the data from the data set and topNeighbours.txt file and tests itself against the testing data finally giving the mean average error. It then asks the user if he would want some movie recommendations and if yes asks the user to rate a set of 10 most rated movies from our data set. It then predicts the ranking of all the movies finally displaying the 20 top rated movies.
