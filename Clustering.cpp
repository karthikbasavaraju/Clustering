#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <limits>
#include <iterator>
#include <string>
#include <algorithm>

#include "Kmeans++.cpp"
using namespace std;

double **distanceMatrix;
map<double, map<int, int>> distanceMap;

//To read data from csv file
void getData(const string &fileName, vector<vector<double>> &dataList, map<string, int> &clusterName, vector<string> &clusterType)
{
	ifstream file(fileName);
	string line = "";
	int i = 0;
	while (getline(file, line))
	{
		vector<double> vec;
		stringstream ss(line);
		string token;
		while (getline(ss, token, ',')) {
			vec.push_back(atof(token.c_str()));
		}
		clusterName.insert(pair<string, int>{token, 0});
		clusterType.push_back(token);
		vec.pop_back();
		dataList.push_back(vec);
		i++;
	}
	clusterType.erase(clusterType.begin());
	dataList.erase(dataList.begin());
	file.close();
	/*
	int count = 0;
	for (vector<double> &vec : dataList)
	{
	for (double &data : vec)
	{
	cout << data << " , ";
	}
	count++;
	cout << std::endl;
	}
	cout << "count=" << count;
	*/
}



//to free the allocated memory
void freemem(int count) {
	for (int i = 0; i < count; i++) {
		delete distanceMatrix[i];
	}
	delete distanceMatrix;
}

//to allocate memory for the matrix
inline void constructMatrix(int count) {
	try {
		distanceMatrix = new double*[count];
		for (int i = 0; i < count; i++)
			distanceMatrix[i] = new double[count];
	}
	catch (const std::bad_alloc) {
		cout << "No new memory" << endl;
	}
}

//Finding distance between all the points and storing them in a matrix
void findLinkageDistance(vector<vector<double> > &dataList, vector<vector<int>> &subCluster) {

	constructMatrix(dataList.size());

	/*
	creating cluster for every node
	*/
	for (int i = 0; i < dataList.size(); i++) {
		vector<int> t;
		t.push_back(i);
		subCluster.push_back(t);
	}

	for (int i = 0; i < dataList.size(); i++) {
		for (int j = i; j < dataList.size(); j++) {
			if (i == j)distanceMatrix[i][j] = 0;
			else {
				vector<double> &tempX = dataList.at(i);
				vector<double> &tempY = dataList.at(j);
				double sum = 0.0;
				for (int k = 0; k < tempX.size(); k++) {
					sum += pow(tempX.at(k) - tempY.at(k), 2);
				}
				pair<int, int> p;
				p.first = i;
				p.second = j;
				pair<double, pair<int, int>> temp;
				temp.first = sum;
				temp.second = p;
				distanceMap[sum][i] = j;
				distanceMatrix[i][j] = sum;
				distanceMatrix[j][i] = sum;
			}
		}
	}
}


inline void mergeCluster(vector<vector<int>> &subCluster, int &ci, int &cj) {
	vector<int> &temp = subCluster.at(cj);
	for (int cj_elements : temp)
		subCluster.at(ci).push_back(cj_elements);
	subCluster.erase(subCluster.begin() + cj);
}

//To merge two closest clusters
void mergeCiCj(int ic, int jc, vector<vector<int>> &subCluster) {
	int ci;
	int cj;
	int count = 0;
	for (int z = 0; z < subCluster.size(); z++) {

		vector<int> &temp2 = subCluster.at(z);
		for (int j = 0; j < temp2.size(); j++) {
			if (temp2.at(j) == ic)
			{
				ci = z;
				count++;
			}
			if (temp2.at(j) == jc)
			{
				cj = z;
				count++;
			}
		}

		if (count == 2)
			break;
	}

	vector<int> &temp = subCluster.at(cj);
	for (int cj_elements : temp)
		subCluster.at(ci).push_back(cj_elements);
	subCluster.erase(subCluster.begin() + cj);


}


//Checks whether two points chosen are in same cluster
//if 2 points are in same distance then it returns false
//else true
bool notSameCluster(int ci, int cj, vector<vector<int>> &subCluster) {
	int z = 0;
	if (subCluster.size() == 49) {
		z = z;
	}

	for (int i = 0; i<subCluster.size(); i++) {
		int count = 0;
		vector<int> &temp = subCluster.at(i);
		for (int j = 0; j < temp.size(); j++) {
			if (temp.at(j) == ci || temp.at(j) == cj)
				count++;
		}
		if (count == 2)
			return false;

	}
	return true;
}


//To find the smallest distance between two points
void findMinDistance(vector<vector<int>> &subCluster, int count) {

	map<double, map<int, int>>::iterator itr;
	for (itr = distanceMap.begin(); itr != distanceMap.end(); itr++) {
		map<int, int> index = itr->second;
		map<int, int>::iterator itr1;
		itr1 = index.begin();
		int i = itr1->first;
		int j = itr1->second;
		if (notSameCluster(i, j, subCluster)) {
			mergeCiCj(i, j, subCluster);
		}
		if (subCluster.size() == count)
			break;
	}
}

//To find the smallest distance between two clusters(Brute force)
void findMinDistance(vector<vector<int>> &subCluster) {
	int ci, cj;
	double min = numeric_limits<double>::max();

	for (int i = 0; i < subCluster.size() - 1; i++) {
		for (int j = i + 1; j < subCluster.size(); j++) {
			for (int &x : subCluster.at(i)) {
				for (int &y : subCluster.at(j)) {
					if (distanceMatrix[x][y] < min) {
						min = distanceMatrix[x][y];
						ci = i;
						cj = j;
					}
				}
			}
		}
	}
	mergeCluster(subCluster, ci, cj);
}


//This function calculates longest distance bewteen two clusters and choses the two clusters with smallest longest distance out of all longest distance
void findMaxDistance(vector<vector<int>> &subCluster) {
	int ci, cj;
	double min = numeric_limits<double>::max();

	for (int i = 0; i < subCluster.size() - 1; i++) {
		for (int j = i + 1; j < subCluster.size(); j++) {
			double max = -1.0;
			for (int &x : subCluster.at(i)) {
				for (int &y : subCluster.at(j)) {
					if (distanceMatrix[x][y] > max)
						max = distanceMatrix[x][y];
				}
			}
			if (max < min) {
				min = max;
				ci = i;
				cj = j;
			}
		}
	}

	mergeCluster(subCluster, ci, cj);
}

//This function finds avg distace between clusters and choses the two clusters with smallest avg distance
void findAvgDistance(vector<vector<int>> &subCluster) {
	int ci, cj;
	double min = numeric_limits<double>::max();

	for (int i = 0; i < subCluster.size() - 1; i++) {
		for (int j = i + 1; j < subCluster.size(); j++) {
			double sum = 0.0;
			for (int &x : subCluster.at(i)) {
				for (int &y : subCluster.at(j))
					sum = sum + distanceMatrix[x][y];
			}
			double sizeX = static_cast<double>(subCluster.at(i).size());
			double sizeY = static_cast<double>(subCluster.at(j).size());
			double avgVal = (1 / (sizeX*sizeY))*sum;

			if (avgVal < min) {
				min = avgVal;
				ci = i;
				cj = j;
			}
		}
	}

	mergeCluster(subCluster, ci, cj);			//To merge the clusers with smallest avg distance
}


void findAvgDistance(vector<vector<int>> &subCluster,int count) {
	int ci, cj;
	double min = numeric_limits<double>::max();

	for (int i = 0; i < subCluster.size() - 1; i++) {
		for (int j = i + 1; j < subCluster.size(); j++) {
			double sum = 0.0;
			for (int &x : subCluster.at(i)) {
				for (int &y : subCluster.at(j))
					sum = sum + distanceMatrix[x][y];
			}
			double sizeX = static_cast<double>(subCluster.at(i).size());
			double sizeY = static_cast<double>(subCluster.at(j).size());
			double avgVal = (1 / (sizeX*sizeY))*sum;

			if (avgVal < min) {
				min = avgVal;
				ci = i;
				cj = j;
			}
		}
	}

	mergeCluster(subCluster, ci, cj);			//To merge the clusers with smallest avg distance
}


// TO print clusters
void printClusters(vector<vector<int>> &linkageSubCluster) {
	int i = 0, clusterElem = 0;
	for (vector<int> &inner : linkageSubCluster) {
		cout << "cluster : " << ++i << endl;
		clusterElem = 0;
		for (int &ele : inner) {
			cout << ele << ", ";
			clusterElem++;
		}
		cout << endl << "count = " << clusterElem << "\n" << endl;
	}
}

//To find hamilton distance
int hammiltonDistance(vector<vector<int>> &linkageSubClusterX, vector<vector<int>> &linkageSubClusterY) {

	int errCount = 0;

	/*This finds a edge which is inside a cluster of ClusterX and in-between clusters of ClusterY
	if found then errCount is increased by 1
	*/
	for (int iX = 0; iX < linkageSubClusterX.size(); iX++) {
		vector<int> &tempX = linkageSubClusterX.at(iX);

		for (int jX = 0; jX < tempX.size() - 1; jX++) {
			for (int kX = jX + 1; kX < tempX.size(); kX++) {
				int flag = 0;
				for (int iY = 0; iY < linkageSubClusterY.size(); iY++) {
					vector<int> &tempY = linkageSubClusterY.at(iY);
					int jflag = 0, kflag = 0;
					int count = 0;
					for (int jY = 0; jY < tempY.size(); jY++) {
						if (tempX.at(jX) == tempY.at(jY)) {
							jflag = 1;
						}
						else if (tempX.at(kX) == tempY.at(jY)) {
							kflag = 1;
						}
					}
					if (jflag && kflag) {
						flag = 1;
						break;
					}
				}
				if (flag != 1)
					errCount++;
			}
		}
	}
	return errCount;
}


//To get the actual clustering from csv file and store them in actualCluster
void getActualClusters(map<string, int> &clusterName, vector<string> &clusterType, vector<vector<int>> &actualCluster) {
	map<string, int>::iterator itr;
	for (itr = clusterName.begin(); itr != clusterName.end(); itr++) {
		string a = itr->first;
		vector<int> temp;
		for (int i = 0; i < clusterType.size(); i++) {
			if (a == clusterType.at(i)) {
				temp.push_back(i);
			}
		}
		actualCluster.push_back(temp);
	}

	for (int i = 0; i < actualCluster.size(); i++) {
		vector<int> temp = actualCluster.at(i);
		cout << "cluster : " << i << endl;
		for (int j = 0; j < temp.size(); j++) {
			cout << temp.at(j) << ", ";
		}
		cout << endl;
	}
}

void findR(int noOfClusters) {
	map<double, map<int, int>>::iterator itr;
	int i = 0;
	int minP1 = -1;
	int minP2 = -1;
	for (itr = distanceMap.begin(); itr != distanceMap.end() && i<noOfClusters; itr++) {

		map<int, int> temp = itr->second;
		map<int, int>::iterator itr1;
		itr1 = temp.begin();
		if (i == 0) {
			cout << itr->first << "--";
			minP1 = itr1->first;
			minP2 = itr1->second;
			cout << itr1->first << "-->" << itr1->second << "\n" << endl;
			i++;
		}
		else if (minP1 == itr1->first || minP1 == itr1->second || minP2 == itr1->first || minP2 == itr1->second) {
			cout << itr->first << "--";
			cout << itr1->first << "-->" << itr1->second << "\n" << endl;
			i++;
		}
	}
	cout << endl;
}

int main()
{
	vector<vector<double>> dataList;											//to store all the points
	map<string, int> clusterName;												//to get names of clusters
	vector<string> clusterType;													//to map all points to its cluster
	getData("iris.csv", dataList, clusterName, clusterType);					//iris.csv(3 clusters), seed.csv(3 clusters), userKnowledge.csv(4 clusters)
	int noOfClusters = 3;

	vector<vector<int>> linkageSubCluster;										//To store the value of every edge
	findLinkageDistance(dataList, linkageSubCluster);							//To find distance of every node wich everyother node
	
	//findR(noOfClusters);

	vector<vector<int>> singleLinkageSubCluster(linkageSubCluster);
	cout << "Single Linkage clustering :" << endl;
	while (singleLinkageSubCluster.size() > noOfClusters)						//run till cluster count is greater than noOfClusters specified
		findMinDistance(singleLinkageSubCluster, noOfClusters);					//SingleLinkage algorithm
	printClusters(singleLinkageSubCluster);

	vector<vector<int>> completeLinkageSubCluster(linkageSubCluster);
	cout << "Complete Linkage clustering :" << endl;
	while (completeLinkageSubCluster.size() > noOfClusters)						//run till cluster count is greater than noOfClusters specified
		findMaxDistance(completeLinkageSubCluster);								//Complete Linkage algorithm
	printClusters(completeLinkageSubCluster);

	vector<vector<int>> averageLinkageSubCluster(linkageSubCluster);
	cout << "Average Linkage clustering :" << endl;
	while (averageLinkageSubCluster.size() > noOfClusters)						//run till cluster count is greater than noOfClusters specified
		findAvgDistance(averageLinkageSubCluster);								//Average Linkage algorithm
	printClusters(averageLinkageSubCluster);
	
	freemem(dataList.size());													//To free allocated data for matrix

	vector<vector<int>> actualCluster;
	getActualClusters(clusterName, clusterType, actualCluster);					//To get the actualClusters 

	vector<vector<int>> lloydClusters;
	lloyd(dataList, clusterName, clusterType, lloydClusters, noOfClusters);		//Lloyd algorithm


	vector<vector<int>> kmeanClusters;
	kMeansPP(dataList, clusterName, clusterType, kmeanClusters, noOfClusters);	//KMeans++ algorithm


	
	vector<vector<vector<int>>> hamDist;
	cout << "\nSingle Linkage\t\t\t";
	hamDist.push_back(singleLinkageSubCluster);
	cout << "Complete Linkage\t\t";
	hamDist.push_back(completeLinkageSubCluster);
	cout << "Average Linkage\t\t";
	hamDist.push_back(averageLinkageSubCluster);
	cout << "Lloyd\t\t\t";
	hamDist.push_back(lloydClusters);
	cout << "kmean++\n";
	hamDist.push_back(kmeanClusters);


	double  n = dataList.size();
	double nC2 = ((n*(n - 1)) / 2);
	for (int i = 0; i < hamDist.size(); i++) {
		double a = hammiltonDistance(hamDist.at(i), actualCluster);
		double b = hammiltonDistance(actualCluster, hamDist.at(i));
		cout << ((a + b) / nC2)<< "\t\t\t";
	}
	cout << endl;
	system("PAUSE");
	return 0;
}