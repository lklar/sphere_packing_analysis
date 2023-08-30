#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <format>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <map>
#include "dim3vectors.h"

template<typename T, int N>
std::vector<T> import_from_csv(std::filesystem::path filePath)
{
	std::string content;
	{
		std::ifstream ifs(filePath, std::ios_base::in);

		ifs.seekg(0, ifs.end);
		size_t fileSize = ifs.tellg();
		ifs.seekg(0, ifs.beg);

		content.resize(fileSize, '\0');
		ifs.read(content.data(), fileSize);
		ifs.close();
	}

	std::vector<T> allData;

	size_t lineStart = 0;
	size_t lineEnd;

	{
		// In case there are any non numerical characters in the first line,
		// it is assumed this is a header line and is skipped
		auto firstLineEnd = content.find('\n', 0);
		std::string_view firstLine(content.data(), firstLineEnd);
		auto nonDataEntry = firstLine.find_first_not_of("0123456789, \r\n");
		if (nonDataEntry != std::string::npos)
		{
			lineStart = firstLineEnd + 1;
		}
	}

	while ((lineEnd = content.find('\n', lineStart)) != std::string::npos)
	{
		std::string_view line(content.data() + lineStart, lineEnd - lineStart);

		std::vector<double> fields;
		auto lEnd = line.data() + line.size();
		fields.emplace_back();
		for (auto fc_result = std::from_chars(line.data(), lEnd, fields.back()); fc_result.ptr < lEnd && fields.size() < N;)
		{
			fields.emplace_back();
			fc_result = std::from_chars(fc_result.ptr + 1, lEnd, fields.back());
		}

		if (fields.size() == N)
		{
			allData.emplace_back(fields);
		}
		lineStart = lineEnd + 1;
	}

	return allData;
}

void write_to_csv(std::filesystem::path filePath, std::vector<double3>& positions);

void export_histogram(std::filesystem::path filePath, std::unordered_map<int, double> histogram, int minBin, int maxBin, double binSize);

void export_histogram(std::filesystem::path filePath, std::vector<std::unordered_map<int, double>> histograms, int minBin, int maxBin, double binSize);

void export_histogram(std::filesystem::path filePath, std::map<int, int> histogram, int totalNumberOfSpheres);

void export_cumulative_histograms(std::filesystem::path filePath, std::vector<double> data);

void export_cumulative_histograms(std::filesystem::path filePath, std::vector<int> data);

void export_cumulative_histograms(std::filesystem::path filePath, std::vector<double3> data, int N);

void process_all_files(std::filesystem::path basePath, std::function<void(const std::filesystem::path)> func);