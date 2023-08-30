#include "contactInfoAnalysis.h"
#include <numeric>

bool customCompare::operator() (const double3& lhs, const double3& rhs) const
{
	return lhs.z < rhs.z;
}

bool customCompare::operator() (const double3* lhs, const double3* rhs) const
{
	return lhs->z < rhs->z;
}

int raster::get_spot_id(const double& p)
{
	return std::floor(p / rasterSize);
}

raster::raster(double3 _posMin, double3 _posMax, double _rasterSize)
{
	posMin = _posMin;
	posMax = _posMax;
	rasterSize = _rasterSize;
	xDim = std::ceil((posMax.x - posMin.x) / rasterSize) + 1;
	yDim = std::ceil((posMax.y - posMin.y) / rasterSize) + 1;
	rasterSize = _rasterSize;
	space.resize(xDim,
		std::vector<std::vector<double3*>>(yDim,
			std::vector<double3*>()));
}

void raster::emplace_back(double3& pos)
{
	int x = get_spot_id(pos.x - posMin.x);
	int y = get_spot_id(pos.y - posMin.y);
	space[x][y].emplace_back(&pos);
}

std::vector<std::reference_wrapper<std::vector<double3*>>> raster::get_spheres(double3& basePos, int range)
{
	std::vector<std::reference_wrapper<std::vector<double3*>>> columns;

	int xId = get_spot_id(basePos.x - posMin.x);
	int yId = get_spot_id(basePos.y - posMin.y);

	int xMin = std::max(xId - range, 0);
	int xMax = std::min(xId + range, xDim - 1);
	int yMin = std::max(yId - range, 0);
	int yMax = std::min(yId + range, yDim - 1);

	for (int myX = xMin; myX <= xMax; ++myX)
	{
		for (int myY = yMin; myY <= yMax; ++myY)
		{
			if (!space[myX][myY].empty())
			{
				columns.emplace_back(space[myX][myY]);
			}
		}
	}

	return columns;
}

void raster::pop_back(double3& basePos)
{
	int xId = get_spot_id(basePos.x - posMin.x);
	int yId = get_spot_id(basePos.y - posMin.y);
	space[xId][yId].pop_back();
}

std::vector<contactInfo> generate_contact_info(std::vector<double3>& positions, double R, double contactDistance)
{
	auto doOverlap = [&contactDistance](const double3& p1, const double3& p2) {
		double dist = (p1 - p2).abs();
		return (dist < contactDistance);
	};

	double3 minPos, maxPos;
	get_min_max(positions, minPos, maxPos);

	customCompare myCompare;

	std::sort(positions.begin(), positions.end(), myCompare);

	raster myRaster(minPos, maxPos, 2.0 * R);

	for (auto& pos : positions)
	{
		myRaster.emplace_back(pos);
	}

	std::vector<contactInfo> allContacts;
	allContacts.resize(positions.size());

	for (int i = positions.size() - 1; i >= 0; --i)
	{
		double3 basePos = positions[i];
		myRaster.pop_back(basePos);
		auto neighbouringSpace = myRaster.get_spheres(basePos, 1);

		for (auto& column : neighbouringSpace)
		{
			for (auto it = column.get().rbegin();
				it != column.get().rend() && (*it)->z >= basePos.z - 2.0 * R;
				++it)
			{
				if (doOverlap(basePos, **it))
				{
					auto contactingId = std::distance(&positions.front(), *it);
					allContacts[i].contacts.emplace_back(contactingId);
					allContacts[contactingId].contacts.emplace_back(i);
				}
			}
		}

	}

	return allContacts;
}

std::vector<std::vector<int>> generate_group_info(std::vector<contactInfo>& allContacts)
{
	int groupId = -1;
	for (int i = 0; i < allContacts.size(); ++i)
	{
		std::vector<int> idsToCheck;

		if (allContacts[i].group == NO_GROUP)
		{
			idsToCheck.reserve(100);
			idsToCheck.emplace_back(i);
			++groupId;
		}

		while (!idsToCheck.empty())
		{
			int id = idsToCheck.back();
			idsToCheck.pop_back();
			if (allContacts[id].group == NO_GROUP)
			{
				allContacts[id].group = groupId;
				idsToCheck.insert(idsToCheck.end(), allContacts[id].contacts.begin(), allContacts[id].contacts.end());
			}
		}
	}

	std::vector<std::vector<int>> groups;
	groups.resize(groupId + 1);

	for (int id = 0; id < allContacts.size(); ++id)
	{
		groups[allContacts[id].group].emplace_back(id);
	}

	return groups;
}

std::tuple<double, double, Histogram> analyze_contacts(std::vector<double3> positions, std::vector<contactInfo> sphereContacts)
{
	std::vector<int> coordinateNumbers;
	coordinateNumbers.resize(sphereContacts.size());
	std::transform(
		sphereContacts.begin(),
		sphereContacts.end(),
		coordinateNumbers.begin(),
		[](const contactInfo& ci) { return ci.contacts.size(); }
	);
	std::sort(coordinateNumbers.begin(), coordinateNumbers.end());

	auto [mean, median] = get_mean_median(coordinateNumbers);

	auto hist = calculate_histogram(coordinateNumbers);
	return std::make_tuple(median, mean, hist);
}

std::map<int, int> analyze_contact_groups(std::vector<double3>& positions, double R, double contactDistance)
{
	auto sphereContacts = generate_contact_info(positions, R, contactDistance);
	auto sphereGroups = generate_group_info(sphereContacts);
	std::map<int, int> groupNumbers;
	for (auto& group : sphereGroups)
	{
		std::map<int, int>::iterator it;
		if ((it = groupNumbers.find(group.size())) != groupNumbers.end())
		{
			++(it->second);
		}
		else {
			groupNumbers[group.size()] = 1;
		}
	}
	return groupNumbers;;
}