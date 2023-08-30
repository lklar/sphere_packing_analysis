#include "generalAlgorithms.h"

void get_min_max(std::vector<double3> pos, double3& minPos, double3& maxPos)
{
	minPos.x = pos.front().x;
	minPos.y = pos.front().y;
	minPos.z = pos.front().z;
	maxPos = minPos;
	for (auto& Ps : pos)
	{
		minPos.x = minPos.x < Ps.x ? minPos.x : Ps.x;
		minPos.y = minPos.y < Ps.y ? minPos.y : Ps.y;
		minPos.z = minPos.z < Ps.z ? minPos.z : Ps.z;
		maxPos.x = maxPos.x > Ps.x ? maxPos.x : Ps.x;
		maxPos.y = maxPos.y > Ps.y ? maxPos.y : Ps.y;
		maxPos.z = maxPos.z > Ps.z ? maxPos.z : Ps.z;
	}
}

std::tuple<int,int> get_min_max(std::unordered_map<int, double>& hist)
{
	int minBin = hist.begin()->first;
	int maxBin = minBin;
	for (auto& bin : hist)
	{
		if(bin.first < minBin)
			minBin = bin.first;
		if(bin.first > maxBin)
			maxBin = bin.first;
	}
	return { minBin, maxBin };
}

Histogram calculate_histogram(std::vector<double>& vectorIn, double binSize)
{
	double dx = 1.0 / (double)vectorIn.size();
	std::unordered_map<int, double> result;
	for (auto& elem : vectorIn)
	{
		int bin = std::round(elem / binSize);
		std::unordered_map<int, double>::iterator it;
		if ((it = result.find(bin)) != result.end())
		{
			it->second += dx;
		}
		else {
			result[bin] = dx;
		}
	}
	return result;
}

Histogram calculate_histogram(std::vector<int>& vectorIn)
{
	double dx = 1.0 / (double)vectorIn.size();
	std::unordered_map<int, double> result;
	for (auto& bin : vectorIn)
	{
		std::unordered_map<int, double>::iterator it;
		if ((it = result.find(bin)) != result.end())
		{
			it->second += dx;
		}
		else {
			result[bin] = dx;
		}
	}
	return result;
}

std::vector<double3> repack_into_double3(std::vector<std::vector<double>>& vec, int3 columnsToUse)
{
	std::vector<double3> result;
	result.reserve(vec.size());
	
	int x = columnsToUse.x;
	int y = columnsToUse.y;
	int z = columnsToUse.z;

	for (auto& elem : vec)
	{
		result.emplace_back(elem[x], elem[y], elem[z]);
	}

	return result;
}