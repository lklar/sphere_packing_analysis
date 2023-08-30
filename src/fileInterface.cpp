#pragma once
#include "fileInterface.h"
#include <random>

void write_to_csv(std::filesystem::path filePath, std::vector<double3>& positions)
{
	std::ofstream ofs(filePath, std::ios_base::out);

	for (auto& pos : positions)
	{
		ofs << std::format("{:.7f},{:.7f},{:.7f}\n", pos.x, pos.y, pos.z);
	}

	ofs.close();
}

void export_histogram(std::filesystem::path filePath, std::unordered_map<int, double> histogram, int minBin, int maxBin, double binSize)
{
	std::ofstream ofs(filePath, std::ios_base::out);

	for (int i = minBin; i <= maxBin; ++i)
	{
		double bin = binSize * i;
		ofs << std::format("{:.5f}", bin);
		double value = histogram.contains(i) ? histogram[i] : 0.0;
		ofs << std::format(",{:.7f}", value);
		ofs << "\n";
	}

	ofs.close();
}


void export_histogram(std::filesystem::path filePath, std::vector<std::unordered_map<int, double>> histograms, int minBin, int maxBin, double binSize)
{
	std::ofstream ofs(filePath, std::ios_base::out);

	for (int i = minBin; i <= maxBin; ++i)
	{
		double bin = binSize * i;
		ofs << std::format("{:.5f}", bin);
		for (auto& histogram : histograms)
		{
			double value = histogram.contains(i) ? histogram[i] : 0.0;
			ofs << std::format(",{:.7f}", value);
		}
		ofs << "\n";
	}

	ofs.close();
}

void export_histogram(std::filesystem::path filePath, std::map<int, int> histogram, int totalNumberOfSpheres)
{
	std::ofstream ofs(filePath, std::ios_base::out);

	for(auto& elem : histogram)
	{
		double value = (double)(elem.first * elem.second) / (double)totalNumberOfSpheres;
		ofs << std::format("{},{:.7f}\n", elem.first, value);
	}

	ofs.close();
}

void export_cumulative_histograms(std::filesystem::path filePath, std::vector<double> data)
{
	std::sort(data.begin(), data.end());

	std::ofstream ofs(filePath, std::ios_base::out);

	for (int i = 0; i < data.size(); ++i)
	{
		double val = (double)i / (double)data.size();
		ofs << std::format("{:7f},{:.7f}\n", data[i], val);
	}
}

void export_cumulative_histograms(std::filesystem::path filePath, std::vector<int> data)
{
	std::sort(data.begin(), data.end());

	std::ofstream ofs(filePath, std::ios_base::out);

	for (int i = 0; i < data.size(); ++i)
	{
		double val = (double)i / (double)data.size();
		ofs << std::format("{},{:.7f}\n", data[i], val);
	}
}

void export_cumulative_histograms(std::filesystem::path filePath, std::vector<double3> data, int N)
{
	std::vector<double> xData, yData, zData;
	xData.reserve(data.size());
	yData.reserve(data.size());
	zData.reserve(data.size());
	for (auto& elem : data)
	{
		xData.emplace_back(elem.x);
		yData.emplace_back(elem.y);
		zData.emplace_back(elem.z);
	}
	std::shuffle(xData.begin(), xData.end(), std::default_random_engine(time(NULL)));
	std::shuffle(yData.begin(), yData.end(), std::default_random_engine(time(NULL)));
	std::shuffle(zData.begin(), zData.end(), std::default_random_engine(time(NULL)));

	xData.resize(N);
	yData.resize(N);
	zData.resize(N);

	std::sort(xData.begin(), xData.end());
	std::sort(yData.begin(), yData.end());
	std::sort(zData.begin(), zData.end());

	std::ofstream ofs(filePath, std::ios_base::out);

	for (int i = 0; i < N; ++i)
	{
		double val = (double)i / (double)N;
		ofs << std::format("{:5f},{:.5f},{:.5},{:.7f}\n", xData[i], yData[i], zData[i], val);
	}
}

void process_all_files(std::filesystem::path basePath, std::function<void(const std::filesystem::path)> func)
{
	for (const auto& subPath : std::filesystem::recursive_directory_iterator(basePath))
	{
		if (subPath.is_regular_file())
		{
			func(subPath);
		}
	}
}