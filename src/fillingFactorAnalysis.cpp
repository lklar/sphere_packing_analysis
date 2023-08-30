#include "fillingFactorAnalysis.h"

void d3sort(double3& d3)
{
	std::vector<double> buffer{
		std::abs(d3.x),
		std::abs(d3.y),
		std::abs(d3.z)
	};
	std::sort(buffer.begin(), buffer.end());
	d3.x = buffer[2];
	d3.y = buffer[1];
	d3.z = buffer[0];
}

std::vector<int3> get_interacting_bins(int3& mainBin, double3& remainderInfo)
{
	std::vector<int3> bins;
	int dx = remainderInfo.x >= 0.5 ? 1 : -1;
	int dy = remainderInfo.y >= 0.5 ? 1 : -1;
	int dz = remainderInfo.z >= 0.5 ? 1 : -1;

	bins.emplace_back(mainBin.x, mainBin.y, mainBin.z);
	bins.emplace_back(mainBin.x, mainBin.y, mainBin.z + dz);
	bins.emplace_back(mainBin.x, mainBin.y + dy, mainBin.z);
	bins.emplace_back(mainBin.x, mainBin.y + dy, mainBin.z + dz);
	bins.emplace_back(mainBin.x + dx, mainBin.y, mainBin.z);
	bins.emplace_back(mainBin.x + dx, mainBin.y, mainBin.z + dz);
	bins.emplace_back(mainBin.x + dx, mainBin.y + dy, mainBin.z);
	bins.emplace_back(mainBin.x + dx, mainBin.y + dy, mainBin.z + dz);

	return bins;
}

std::vector<std::vector<std::vector<double>>> get_base_filling_factors(std::vector<double3>& spherePositions, double R)
{
	double3 minPos, maxPos;
	get_min_max(spherePositions, minPos, maxPos);
	minPos = minPos - R;
	maxPos = maxPos + R;

	int xDim = std::ceil((maxPos.x - minPos.x) / (2.0 * R));
	int yDim = std::ceil((maxPos.y - minPos.y) / (2.0 * R));
	int zDim = std::ceil((maxPos.z - minPos.z) / (2.0 * R));

	std::vector<std::vector<std::vector<double>>> bins(xDim,
		std::vector<std::vector<double>>(yDim,
			std::vector<double>(zDim, 0.0)));

	for (auto& pos : spherePositions)
	{
		double3 Ps = pos - minPos;
		int3 mainBin;
		double3 sub;
		sub.x = remquo(Ps.x, 2.0 * R, &mainBin.x);
		sub.y = remquo(Ps.y, 2.0 * R, &mainBin.y);
		sub.z = remquo(Ps.z, 2.0 * R, &mainBin.z);

		// Get the 8 bins that this sphere interacts with
		auto interactingBins = get_interacting_bins(mainBin, sub);
		double totalVol = 0.0;
		for (auto& bin : interactingBins)
		{
			double3 binCenter(
				R * (2.0 * bin.x + 1.0),
				R * (2.0 * bin.y + 1.0),
				R * (2.0 * bin.z + 1.0)
			);
			double3 Ps_rel = Ps - binCenter;
			d3sort(Ps_rel);

			volumeFractions mf(R, Ps_rel);
			double myVol = mf.get_volume();
			totalVol += myVol;
			bins[bin.x][bin.y][bin.z] += myVol;
		}
	}

	double ratioSphereBin = (4.0 / 3.0) * std::numbers::pi / 8.0;
	for (auto& row : bins)
	{
		for (auto& col : row)
		{
			for (auto& bin : col)
			{
				bin *= ratioSphereBin;
			}
		}
	}

	return bins;
}

std::vector<std::vector<std::vector<double>>> compute_average_filling_factors(std::vector<std::vector<std::vector<double>>>& baseFF, int dN)
{
	int xDim = std::ceil(baseFF.size() - 2) / dN;
	int yDim = std::ceil(baseFF[0].size() - 2) / dN;
	int zDim = std::ceil(baseFF[0][0].size() - 2) / dN;
	std::vector<std::vector<std::vector<double>>> avgFF(xDim,
		std::vector<std::vector<double>>(yDim,
			std::vector<double>(zDim, 0.0)));

	for (int x = 0; x < xDim; ++x)
	{
		for (int y = 0; y < yDim; ++y)
		{
			for (int z = 0; z < zDim; ++z)
			{
				double buffer = 0.0;
				for (int dx = 1; dx <= dN; ++dx)
				{
					for (int dy = 1; dy <= dN; ++dy)
					{
						for (int dz = 1; dz <= dN; ++dz)
						{
							buffer += baseFF[x * dN + dx][y * dN + dy][z * dN + dz];
						}
					}
				}
				buffer *= 1.0 / std::pow(dN, 3);

				avgFF[x][y][z] = buffer;
			}
		}
	}

	return avgFF;
}


std::vector<double> unravel_vector(std::vector<std::vector<std::vector<double>>>& vecIn)
{
	std::vector<double> vecOut;
	vecOut.reserve(vecIn.size() * vecIn.front().size() * vecIn.front().front().size());

	for (auto& row : vecIn)
	{
		for (auto& col : row)
		{
			for (auto& elem : col)
			{
				vecOut.emplace_back(elem);
			}
		}
	}
	return vecOut;
}

std::vector<std::unordered_map<int, double>> analyze_local_filling_factor(std::vector<double3>& positions, double radius, std::vector<int> smoothingRadii, double histBinSize, std::vector<std::vector<double>>& resultingFillingFactors)
{
	resultingFillingFactors.clear();

	auto baseFF = get_base_filling_factors(positions, radius);
	std::vector<std::unordered_map<int, double>> allHistograms;
	for (auto& binSize : smoothingRadii)
	{
		auto avgFF = compute_average_filling_factors(baseFF, binSize);
		resultingFillingFactors.emplace_back(unravel_vector(avgFF));
		auto hist = calculate_histogram(resultingFillingFactors.back(), histBinSize);
		allHistograms.emplace_back(hist);
	}
	
	return allHistograms;
}
