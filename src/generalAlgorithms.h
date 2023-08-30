#pragma once
#include "dim3vectors.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <numeric>

typedef std::unordered_map<int, double> Histogram;

void get_min_max(std::vector<double3> pos, double3& minPos, double3& maxPos);

std::tuple<int,int> get_min_max(std::unordered_map<int, double>& hist);

template <typename T>
std::pair<double, double> get_mean_median(std::vector<T> vec)
{
	std::sort(vec.begin(), vec.end());
	double median =
		vec.size() % 2 == 0 ?
		(vec[vec.size() / 2] + vec[(vec.size() / 2) - 1]) / 2.0 :
		vec[std::floor(0.5 * vec.size())];

	double mean = std::accumulate(vec.begin(), vec.end(), 0.0) / (double)vec.size();
	return std::make_pair(mean, median);
};

Histogram calculate_histogram(std::vector<double>& vectorIn, double binSize);

Histogram calculate_histogram(std::vector<int>& vectorIn);

std::vector<double3> repack_into_double3(std::vector<std::vector<double>>& vec, int3 columnsToUse);