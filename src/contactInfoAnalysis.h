#pragma once
#include <iostream>
#include "dim3vectors.h"
#include <math.h>
#include "generalAlgorithms.h"
#include <algorithm>
#include <map>


constexpr auto NO_GROUP = -1;

struct customCompare {
	bool operator() (const double3& lhs, const double3& rhs) const;

	bool operator() (const double3* lhs, const double3* rhs) const;
};

struct raster {
private:
	double rasterSize;
	double3 posMin;
	double3 posMax;
	int xDim;
	int yDim;
	std::vector<std::vector<std::vector<double3*>>> space;

	int get_spot_id(const double& p);

public:
	raster(double3 _posMin, double3 _posMax, double _rasterSize);

	void emplace_back(double3& pos);

	std::vector<std::reference_wrapper<std::vector<double3*>>> get_spheres(double3& basePos, int range);

	void pop_back(double3& basePos);
};

struct contactInfo {
	int group = NO_GROUP;
	std::vector<int> contacts;
};

std::vector<contactInfo> generate_contact_info(std::vector<double3>& positions, double R, double contactDistance);

std::vector<std::vector<int>> generate_group_info(std::vector<contactInfo>& allContacts);

std::tuple<double, double, Histogram> analyze_contacts(std::vector<double3> positions, std::vector<contactInfo> sphereContacts);

std::map<int, int> analyze_contact_groups(std::vector<double3>& positions, double R, double contactDistance);