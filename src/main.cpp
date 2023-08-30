#include <iostream>
#include <vector>
#include "dim3vectors.h"
#include "fileInterface.h"
#include "fillingFactorAnalysis.h"
#include "contactInfoAnalysis.h"
#include "generalAlgorithms.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <string>

int main(int argc, char** argv)
{
	auto configPath = std::filesystem::path("config.json");
	if (argc == 2)
	{
		configPath = std::filesystem::path(argv[1]);
	}

	nlohmann::json config;
	{
		std::ifstream ifs(configPath);
		ifs >> config;
		ifs.close();
	}

	// Open new files for the mean and median values, which are shared
	std::ofstream mm_ff, mm_contact;
	if (config.count("ff_analysis") != 0)
	{
		std::filesystem::path ffPath = std::string(config["ff_analysis"]["output_path"]);
		std::filesystem::create_directories(ffPath);
		mm_ff.open(ffPath / "means_medians.csv", std::ios_base::out);
		mm_ff << std::format("File name, Box size, Mean, Median\n");
	}
	if (config.count("contact_analysis") != 0)
	{
		std::filesystem::path contactPath = std::string(config["contact_analysis"]["output_path"]);
		std::filesystem::create_directories(contactPath);
		mm_contact.open(contactPath / "means_medians.csv", std::ios_base::out);
		mm_contact << std::format("File name, Mean, Median\n");
	}


	process_all_files(config["base_path"],
		[&config, &mm_ff, &mm_contact](std::filesystem::path filePath) {
			std::filesystem::path basePath = std::string(config["base_path"]);
			double R = config["radius"];
			auto positions = import_from_csv<double3, 3>(filePath.string());
			
			if (config.count("ff_analysis") != 0)
			{
				std::filesystem::path outputPath = std::string(config["ff_analysis"]["output_path"]);
				double binSize = config["ff_analysis"]["hist_bin_size"];
				std::vector<int> smoothingRadii = config["ff_analysis"]["box_sizes"];

				std::vector<std::vector<double>> allFillingFactors;
				auto hists = analyze_local_filling_factor(positions, R, smoothingRadii, binSize, allFillingFactors);

				for (int i = 0; i < smoothingRadii.size(); ++i)
				{
					auto histPath = 
						outputPath / 
						std::format("box_{}", smoothingRadii[i]) / 
						std::filesystem::relative(filePath, basePath);
					std::filesystem::create_directories(histPath.parent_path());
					export_histogram(histPath, hists[i], 0, std::ceil(1.0 / binSize), binSize);

					std::filesystem::path cumulativePath =
						histPath.parent_path() / 
						"cumulative" / 
						filePath.filename();
					std::filesystem::create_directories(cumulativePath.parent_path());
					export_cumulative_histograms(cumulativePath, allFillingFactors[i]);

					auto meanMedian = get_mean_median(allFillingFactors[i]);
					mm_ff <<
						std::format("{},{},{:.5f},{:.5f}\n", 
							filePath.filename().string(), 
							smoothingRadii[i], meanMedian.first, meanMedian.second);
				}
			}

			std::vector<contactInfo> sphereContacts;
			if (config.count("contact_analysis") != 0 || config.count("contact_direction") != 0)
			{
				sphereContacts = generate_contact_info(positions, R, 2.0001 * R);
			}

			if (config.count("contact_analysis") != 0)
			{
				std::filesystem::path outputPath = std::string(config["contact_analysis"]["output_path"]);

				std::vector<int> allContacts;

				auto [median, mean, hist] = analyze_contacts(positions, sphereContacts);

				auto histPath = outputPath / std::filesystem::relative(filePath, basePath);

				std::filesystem::create_directories(histPath.parent_path());

				auto [minBin, maxBin] = get_min_max(hist);

				export_histogram(histPath, hist, minBin, maxBin, 1);

				
				mm_contact << std::format("{},{:.5f},{:.5f}\n", filePath.filename().string(), mean, median);
			}

			if (config.count("contact_direction") != 0)
			{
				std::filesystem::path outputPath = std::string(config["contact_direction"]["output_path"]);

				std::vector<double3> allContactDirections;
				std::vector<double> xDirection, yDirection, zDirection;
				for (int ID1 = 0; ID1 < sphereContacts.size(); ++ID1)
				{
					for (auto& ID2 : sphereContacts[ID1].contacts)
					{
						double3 dPos = positions[ID1] - positions[ID2];
						dPos = dPos / dPos.abs();
						dPos.x = std::abs(dPos.x);
						dPos.y = std::abs(dPos.y);
						dPos.z = std::abs(dPos.z);
						allContactDirections.emplace_back(dPos);
						xDirection.emplace_back(dPos.x);
						yDirection.emplace_back(dPos.y);
						zDirection.emplace_back(dPos.z);
					}
				}

				double binSize = 0.005;

				auto xHist = calculate_histogram(xDirection, binSize);
				auto yHist = calculate_histogram(yDirection, binSize);
				auto zHist = calculate_histogram(zDirection, binSize);

				auto [xMinBin, xMaxBin] = get_min_max(xHist);
				auto [yMinBin, yMaxBin] = get_min_max(yHist);
				auto [zMinBin, zMaxBin] = get_min_max(zHist);

				int minBin = std::min({ xMinBin, yMinBin, zMinBin });
				int maxBin = std::max({ xMaxBin, yMaxBin, zMaxBin });

				auto histPath = outputPath / std::filesystem::relative(filePath, basePath);
				std::filesystem::create_directories(histPath.parent_path());

				export_histogram(histPath, { xHist, yHist, zHist }, minBin, maxBin, binSize);

				std::filesystem::path cumulativePath =
					histPath.parent_path() /
					"cumulative" /
					histPath.filename();
				std::filesystem::create_directories(cumulativePath.parent_path());
				export_cumulative_histograms(cumulativePath, allContactDirections, 10000);
			}
		}
	);

	mm_ff.close();
	mm_contact.close();

	return 0;
}
