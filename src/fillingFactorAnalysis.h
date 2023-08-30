#pragma once
#include <iostream>
#include <numbers>
#include <algorithm>
#include "dim3vectors.h"
#include "generalAlgorithms.h"


class volumeFractions {
private:
	int binId;
	bool intersectsXYedge;
	bool intersectsXZedge;
	bool intersectsYZedge;
	bool intersectsCorner;
	bool doesNotIntersect = false;
	double d1r;
	double d2r;
	double d3r;

	double fFace(double deltaOverR)
	{
		return 0.25 * (3.0 * std::pow(deltaOverR, 2.0) - std::pow(deltaOverR, 3.0));
	}

	double fEdge(double delta1OverR, double delta2OverR)
	{
		if (delta1OverR == 1.0)
		{
			return 0.5 * fFace(delta2OverR);
		}
		double a = 1.0 - delta1OverR;
		double b = 1.0 - delta2OverR;
		double x = std::sqrt(1.0 - std::pow(a, 2.0) - std::pow(b, 2.0));
		double vol = (0.25 / std::numbers::pi) *
			(
				2.0 * a * b * x -
				(3.0 * a - std::pow(a, 3.0)) * std::atan2(x, b) -
				(3.0 * b - std::pow(b, 3.0)) * std::atan2(x, a) +
				2.0 * atan2(x * a, b) + 2.0 * std::atan2(x * b, a)
				);
		return vol;
	}

	double fEdgeApprox(double delta1OverR, double delta2OverR)
	{
		double a1 = -0.010987;
		double a12 = 1.0383;
		double f1 = fFace(delta1OverR);
		double f2 = fFace(delta2OverR);
		return a1 * (f1 + f2) + a12 * f1 * f2;
	}

	double fCorner(double delta1OverR, double delta2OverR, double delta3OverR)
	{
		double a = 1.0 - delta1OverR;
		double b = 1.0 - delta2OverR;
		double c = 1.0 - delta3OverR;
		double A = std::sqrt(1.0 - std::pow(a, 2.0) - std::pow(c, 2.0));
		double B = std::sqrt(1.0 - std::pow(b, 2.0) - std::pow(c, 2.0));

		double vol = 0.5 * fEdge(delta1OverR, delta2OverR) - (0.125 / std::numbers::pi) * (
			6.0 * a * b * c - 2.0 * a * A * c - 2.0 * b * B * c -
			(3.0 * b - std::pow(b, 3.0)) * std::atan(c / B) -
			(3.0 * a - std::pow(a, 3.0)) * std::atan(c / A) +
			(3.0 * c - std::pow(c, 3.0)) * (std::atan(A / a) - std::atan(b / B)) +
			2.0 * (std::atan(c * a / A) + std::atan(c * b / B))
			);
		return vol;
	}

	double fCornerApprox(double delta1OverR, double delta2OverR, double delta3OverR)
	{
		double a1 = -0.001918;
		double a12 = -0.006951;
		double a123 = 1.0355;
		double f1 = fFace(delta1OverR);
		double f2 = fFace(delta2OverR);
		double f3 = fFace(delta3OverR);
		double vol = a1 * (f1 + f2 + f3) +
			a12 * (f1 * f2 + f1 * f3 + f2 * f3) +
			a123 * f1 * f2 * f3;
		return vol;
	}

	double get_volume_from_bin_0()
	{
		double vol = 1.0 - fFace(d1r) - fFace(d2r) - fFace(d3r);

		if (intersectsXYedge)
			vol += fEdge(d1r, d2r);
		if (intersectsXZedge)
			vol += fEdge(d1r, d3r);
		if (intersectsYZedge)
			vol += fEdge(d2r, d3r);
		if (intersectsCorner)
			vol -= fCorner(d1r, d2r, d3r);

		return vol;
	}

	double get_volume_from_bin_1()
	{
		double vol = fFace(d1r);

		if (intersectsXYedge)
			vol -= fEdge(d1r, d2r);
		if (intersectsXZedge)
			vol -= fEdge(d1r, d3r);
		if (intersectsCorner)
			vol += fCorner(d1r, d2r, d3r);

		return vol;
	}

	double get_volume_from_bin_2()
	{
		double vol = 0.0;

		if (intersectsXYedge)
			vol += fEdge(d1r, d2r);
		if (intersectsCorner)
			vol -= fCorner(d1r, d2r, d3r);

		return vol;
	}

	double get_volume_from_bin_3()
	{
		double vol = 0.0;

		if (intersectsCorner)
			vol += fCorner(d1r, d2r, d3r);

		return vol;
	}

public:
	volumeFractions(double r, double3 d)
	{

		if (d.x >= 2.0 * r)
		{
			doesNotIntersect = true;
			return;
		}

		if (d.z >= r)
		{
			// Sphere is in bin 3 with center (r, r, r)
			d1r = (2.0 * r - d.x) / r;
			d2r = (2.0 * r - d.y) / r;
			d3r = (2.0 * r - d.z) / r;
			binId = 3;
		}
		else if (d.y >= r)
		{
			// Sphere is in bin 2 with center (r, r, 0)
			d1r = (2.0 * r - d.x) / r;
			d2r = (2.0 * r - d.y) / r;
			d3r = d.z / r;
			binId = 2;
		}
		else if (d.x >= r)
		{
			// Sphere is in bin 1 with center (r, 0, 0)
			d1r = (2.0 * r - d.x) / r;
			d2r = d.y / r;
			d3r = d.z / r;
			binId = 1;
		}
		else
		{
			// Sphere is inside our main bin with center (0, 0, 0)
			d1r = d.x / r;
			d2r = d.y / r;
			d3r = d.z / r;
			binId = 0;
		}

		double aHat = 1.0 - d1r;
		double bHat = 1.0 - d2r;
		double cHat = 1.0 - d3r;
		double aHatSquared = aHat * aHat;
		double bHatSquared = bHat * bHat;
		double cHatSquared = cHat * cHat;
		double xySquared = 1.0 - aHatSquared - bHatSquared;
		double xzSquared = 1.0 - aHatSquared - cHatSquared;
		double yzSquared = 1.0 - bHatSquared - cHatSquared;

		intersectsXYedge = xySquared > 0.0;
		intersectsXZedge = xzSquared > 0.0;
		intersectsYZedge = yzSquared > 0.0;
		intersectsCorner = aHatSquared + bHatSquared + cHatSquared < 1.0;
	}

	double get_volume()
	{
		if (doesNotIntersect)
			return 0.0;
		switch (binId)
		{
		case 0:
			return get_volume_from_bin_0();
		case 1:
			return get_volume_from_bin_1();
		case 2:
			return get_volume_from_bin_2();
		case 3:
			return get_volume_from_bin_3();
		}
	}
};

void d3sort(double3& d3);

std::vector<int3> get_interacting_bins(int3& mainBin, double3& remainderInfo);

std::vector<std::vector<std::vector<double>>> get_base_filling_factors(std::vector<double3>& spherePositions, double r);

std::vector<std::vector<std::vector<double>>> compute_average_filling_factors(std::vector<std::vector<std::vector<double>>>& baseFF, int dN);

std::vector<double> unravel_vector(std::vector<std::vector<std::vector<double>>>& vecIn);

std::vector<std::unordered_map<int, double>> analyze_local_filling_factor(std::vector<double3>& positions, double radius, std::vector<int> smoothingRadii, double histBinSize, std::vector<std::vector<double>>& resultingFillingFactors);
