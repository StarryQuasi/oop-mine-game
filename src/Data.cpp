#include <unordered_map>
#include <vector>

#include "Blocks.h"
#include "Data.h"

std::vector<Data::TreeType> Data::treeTypes{};

void Data::init()
{
	const auto makePattern =
		[](const std::initializer_list<std::pair<char, const Block&>> mappings,
		   const std::initializer_list<std::string_view> pattern)
	{
		std::vector<std::vector<const Block*>> res;
		std::unordered_map<char, const Block*> map;
		map[' '] = &Blocks::air;
		for (const auto& [c, block] : mappings)
			map[c] = &block;
		for (const auto& row : pattern)
		{
			std::vector<const Block*> resultRow;
			for (const char c : row)
				resultRow.push_back(map.at(c));
			res.push_back(std::move(resultRow));
			assert(res.back().size() == res.front().size());
		}
		return res;
	};
	// spell-checker:disable
	{
		std::vector<std::vector<std::vector<const Block*>>> patterns{};
		patterns.push_back(makePattern(
			{{'S', Blocks::oakLeaves}, {'H', Blocks::oakLog}},
			{"   S   ",
			 "  SSS  ",
			 " SSHSS ",
			 "SSSHSSS",
			 "SSSHSSS",
			 "   H   ",
			 "   H   "}));
		treeTypes.push_back(
			Data::TreeType{
				.log = &Blocks::oakLog,
				.planks = &Blocks::oakPlanks,
				.leaves = &Blocks::oakLeaves,
				.sapling = &Blocks::oakSapling,
				.color = 0xFF00FF00,
				.patterns = std::move(patterns),
			});
	}
	{
		std::vector<std::vector<std::vector<const Block*>>> patterns{};
		patterns.push_back(makePattern(
			{{'S', Blocks::cherryLeaves}, {'H', Blocks::cherryLog}},
			{"    SSSSS    ",
			 "  SSSSSSSSS  ",
			 " SSSSSSSSSHS ",
			 "SSSHSSSHHHSSS",
			 " SSSHHHHSSSS ",
			 "  SS  H  SS  ",
			 "      H      ",
			 "      H      "}));
		treeTypes.push_back(
			Data::TreeType{
				.log = &Blocks::cherryLog,
				.planks = &Blocks::cherryPlanks,
				.leaves = &Blocks::cherryLeaves,
				.sapling = &Blocks::cherrySapling,
				.color = 0xFFFFC0CB,
				.patterns = std::move(patterns),
			});
	}
	// spell-checker:enable
}

const std::vector<Data::TreeType>& Data::getTreeTypes()
{
    return treeTypes;
}