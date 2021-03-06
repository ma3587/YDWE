
#include <slk/reader/WesReader.hpp>
#include <slk/reader/CommonReader.hpp>

namespace slk
{
	void WesReader::Read(buffer_reader& reader, WesTable& table)
	{
		bool is_WorldEditStrings = false;
		reader::utility::remove_bom(reader);
		reader::utility::each_line(reader, [&](boost::string_ref& line)
		{
			size_t pos = line.find("//");
			if (pos != boost::string_ref::npos)
			{
				line.remove_prefix(pos);
			}

			trim_left(line, ctype::is_space());

			if ((line.size() >= 2) && (line[0] == '['))
			{
				auto ItBeg = find_begin(line, char_equal(']'));
				if (ItBeg != line.end())
				{
					is_WorldEditStrings = "WorldEditStrings" == trim_copy<boost::string_ref>(line.begin()+1, ItBeg);
				}
			}
			else
			{
				if (is_WorldEditStrings)
				{
					auto ItBeg = find_begin(line, char_equal('='));

					if (ItBeg != line.end())
					{
						boost::string_ref key = trim_copy<boost::string_ref>(line.begin(), ItBeg);
						boost::string_ref val = trim_copy<boost::string_ref>(ItBeg+1, line.end());
						if (!val.empty() && !key.empty())
						{
							table[key.to_string()] = val.to_string();
						}
					}
				}
			}
		});
	};
}
