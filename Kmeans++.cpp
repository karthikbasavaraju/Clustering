#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>
#include <string>
#include<map>
#include<set>
#include <algorithm>
#include <random>
using namespace std;

void findDistanceKmeans(vector<vector<double> > &dataList, vector<int> &kPoints, vector<vector<double>> &centroid) {

	for (int k : kPoints) {
		centroid.push_back(dataList.at(k));
	}

	cout << "\nInitials centroids:" << endl;
	for (int i = 0; i < centroid.size(); i++) {
		for (double k : centroid.at(i)) {
			cout << k << "\t";
		}
		cout << endl;
	}

	int flag = 0;
	do {
		vector<vector<double>> centroidCalc;     //To store sum of xi which are inside the cluster(all points nearest to this centroid)
		vector<double> centroidCount;			//To store count of points inside the cluster(All points nearest to this centroid)

												/*
												Initializing intermediate centroids to be at 0
												and centroid count to 0
												*/
		for (int i = 0; i < centroid.size(); i++) {
			vector<double> temp;
			for (int j = 0; j < dataList.at(0).size(); j++) {
				temp.push_back(0.0);
			}
			centroidCalc.push_back(temp);
			centroidCount.push_back(0);
		}


		for (int j = 0; j < dataList.size(); j++) {
			double minDist = numeric_limits<double>::max();
			int centroidNo = -1;
			for (int i = 0; i < centroid.size(); i++) {
				vector<double> &tempX = centroid.at(i);
				vector<double> &tempY = dataList.at(j);
				double dist = 0.0;
				for (int k = 0; k < tempX.size(); k++) {
					dist += pow(tempX.at(k) - tempY.at(k), 2);
				}
				if (dist < minDist) {
					minDist = dist;
					centroidNo = i;
				}
			}

			centroidCount.at(centroidNo) = centroidCount.at(centroidNo) + 1;
			for (int k = 0; k < dataList.at(0).size(); k++) {
				centroidCalc.at(centroidNo).at(k) = centroidCalc.at(centroidNo).at(k) + dataList.at(j).at(k);
			}
		}
		flag = 0;
		for (int i = 0; i < centroid.size(); i++) {
			for (int j = 0; j < centroid.at(0).size(); j++) {
				double temp = static_cast<int>(((centroidCalc.at(i).at(j) / centroidCount.at(i)) + 0.0005) * 1000) / 1000.0;
				if (centroid.at(i).at(j) != temp) {
					flag = 1;
				}
				centroid.at(i).at(j) = temp;
			}
		}
		/*
		cout << "\nIntermediate centroids : " << endl;
		for (int i = 0; i < centroid.size(); i++) {
		for (double t :  centroid.at(i)) {
		cout << t << "\t";
		}
		cout << endl;
		}
		*/

	} while (flag == 1);
}

void assignCluster(vector<vector<double>> &dataList, vector<vector<double>> &centroid, vector<vector<int>> &clusters) {

	clusters.resize(centroid.size());

	for (int j = 0; j < dataList.size(); j++) {
		double minDist = numeric_limits<double>::max();
		int centroidNo = 0;
		for (int i = 0; i < centroid.size(); i++) {
			vector<double> &tempX = centroid.at(i);
			vector<double> &tempY = dataList.at(j);
			double dist = 0.0;
			for (int k = 0; k < tempX.size(); k++) {
				dist += pow(tempX.at(k) - tempY.at(k), 2);
			}
			if (dist < minDist) {
				minDist = dist;
				centroidNo = i;
			}
		}
		clusters.at(centroidNo).push_back(j);
	}
}

void findAccuracy(vector<vector<double>> &dataList, vector<vector<double>> &centroid, map<string, int> &clusterName, vector<string> &clusterType, vector<vector<int>> &clusters) {
	cout << "\nFinal centroid points:" << endl;
	for (int i = 0; i < centroid.size(); i++) {
		for (double k : centroid.at(i)) {
			cout << k << "\t";
		}
		cout << endl;
	}
	assignCluster(dataList, centroid, clusters);


	int overAllPositiveCount = 0;
	for (int i = 0; i < clusters.size(); i++) {

		cout << "\nCentroid : " << i << "\t" << "Count : " << clusters.at(i).size() << endl;
		int pcount = 0;
		for (int points : clusters.at(i)) {
			cout << points << ", ";
		}
		overAllPositiveCount += pcount;
	}
	cout << endl;
}

void getKPointsUsingKPP(int size, vector<int> &kPoints, vector<vector<double>> &dataList, int clusterSize) {
std:random_device rd;
	int randomNo = rd() % size;
	kPoints.push_back(randomNo);

	vector<vector<double>> sse;
	for (int z = 0; z < dataList.size(); z++) {
		vector<double> temp;
		sse.push_back(temp);
	}

	int i = 0;
	do {
		double prev = 0.0;
		vector<double> cumulativeSum;

		for (int j = 0; j < dataList.size(); j++) {
			vector<double> &clusteri = sse.at(j);
			vector<double> &tempY = dataList.at(j);

			for (int l = i; l < kPoints.size(); l++) {
				vector<double> &tempX = dataList.at(kPoints.at(l));
				double dist = 0.0;
				double min = numeric_limits<double>::max();

				for (int k = 0; k < tempX.size(); k++) {
					dist += pow(tempX.at(k) - tempY.at(k), 2);
					if (dist < min)
						min = dist;
				}
				clusteri.push_back(dist);
			}
		}

		double max = -1.0;
		int nextCentroid = -1;
		for (int z = 0; z < sse.size(); z++) {
			vector<double> &clusteri = sse.at(z);
			double min = numeric_limits<double>::max();
			for (double no : clusteri) {
				if (no <= min) {
					min = no;
				}
			}
			if (min >= max) {
				max = min;
				nextCentroid = z;
			}
		}

		kPoints.push_back(nextCentroid);
		i++;

		/*		for (int j = 0; j < cumulativeSum.size(); j++) {
		cout << cumulativeSum.at(j) << "\n";
		}
		cout << "\n" << endl;*/

	} while (i < clusterSize - 1);
}


void getKPointsUsingProbabilityOpti(int size, vector<int> &kPoints, vector<vector<double>> &dataList, int clusterSize) {
std:random_device rd;
	int randomNo = rd() % size;
	kPoints.push_back(randomNo);

	vector<vector<double>> sse;
	for (int z = 0; z < dataList.size(); z++) {
		vector<double> temp;
		sse.push_back(temp);
	}

	int i = 0;
	do {
		double prev = 0.0;
		vector<double> cumulativeSum;

		for (int j = 0; j < dataList.size(); j++) {
			vector<double> &clusteri = sse.at(j);
			vector<double> &tempY = dataList.at(j);

			for (int l = i; l < kPoints.size(); l++) {
				vector<double> &tempX = dataList.at(kPoints.at(l));
				double dist = 0.0;
				double min = numeric_limits<double>::max();

				for (int k = 0; k < tempX.size(); k++) {
					dist += pow(tempX.at(k) - tempY.at(k), 2);
					if (dist < min)
						min = dist;
				}
				clusteri.push_back(dist);
				prev = prev + (min*min);
				cumulativeSum.push_back(prev);
			}
		}



		int max = static_cast<int>(cumulativeSum.at(cumulativeSum.size() - 1));
		std::random_device rd;
		randomNo = rd() % max;

		double randomNum = static_cast<double>(randomNo);
		int nextCluster = -1;

		for (int j = 0; j < cumulativeSum.size(); j++) {
			if (cumulativeSum.at(j)>randomNum) {
				if (j != 0) {
					if (j > 2 && (cumulativeSum.at(j - 1) == cumulativeSum.at(j - 2)))
						nextCluster = j - 2;
					else
						nextCluster = j - 1;
				}
				else
					nextCluster = 0;
				break;
			}
		}

		kPoints.push_back(nextCluster);
		i++;

		/*		for (int j = 0; j < cumulativeSum.size(); j++) {
		cout << cumulativeSum.at(j) << "\n";
		}
		cout << "\n" << endl;*/

	} while (i < clusterSize - 1);
}


void getRandomKpoints(int size, vector<int> &kPoints, int clusters) {
	std::random_device rd;
	while (kPoints.size() != clusters) {
		int randomNo = rd() % size;
		int flag = 0;
		for (int no : kPoints) {
			if (no == randomNo) {
				flag = 1;
				break;
			}
		}
		if (flag != 1)
			kPoints.push_back(randomNo);
	}
}

void getWorstPoints(vector<int> &kPoints, int count) {
	for (int i = 0; i < count; i++) {
		kPoints.push_back(i);
	}
}

void lloyd(vector<vector<double>> &dataList, map<string, int> &clusterName, vector<string> &clusterType, vector<vector<int>> &clusters, int noOfClusters)
{

	vector<int> kPoints;
	getRandomKpoints(dataList.size(), kPoints, noOfClusters);
	//getWorstPoints(kPoints,noOfClusters);
	cout << "Kpoints : \n";
	for (int i = 0; i < kPoints.size(); i++)
	{
		cout << kPoints.at(i) << "\n";
	}
	cout << endl;



	vector<vector<double>> centroid;
	findDistanceKmeans(dataList, kPoints, centroid);

	findAccuracy(dataList, centroid, clusterName, clusterType, clusters);
}

void kMeansPP(vector<vector<double>> &dataList, map<string, int> &clusterName, vector<string> &clusterType, vector<vector<int>> &clusters, int noOfClusters)
{

	vector<int> kPoints;
	getKPointsUsingProbabilityOpti(dataList.size(), kPoints, dataList, noOfClusters);
//	getKPointsUsingKPP(dataList.size(), kPoints, dataList, noOfClusters);

/*	cout << "Kpoints : \n";
	for (int i = 0; i < kPoints.size(); i++)
	{
		cout << kPoints.at(i) << "\n";
	}
	cout << endl;
	*/


	vector<vector<double>> centroid;
	findDistanceKmeans(dataList, kPoints, centroid);

	findAccuracy(dataList, centroid, clusterName, clusterType, clusters);
}