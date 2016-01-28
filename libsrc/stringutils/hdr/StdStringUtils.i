//
// Copyright (C) 2012, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//

template <class ContainerType>
void
split(const std::string &str, 
      ContainerType &tokens,
      const std::string &delimiters, 
      bool trim_empty)
{
    std::string::size_type pos;
    std::string::size_type last_pos = 0;

    while (true)
    {
        pos = str.find_first_of(delimiters, last_pos);
        if (pos == std::string::npos)
        {
            pos = str.length();

            if (pos != last_pos || !trim_empty)
            {
                tokens.push_back(
                    std::vector<std::string>::value_type(str.data()+last_pos,
                   (std::vector<std::string>::value_type::size_type)pos-last_pos));
            }
            break;
        }
        else
        {
            if (pos != last_pos || !trim_empty)
            {
                tokens.push_back(
                    std::vector<std::string>::value_type(str.data()+last_pos,
                   (std::vector<std::string>::value_type::size_type)pos-last_pos ));
            }
        }
        last_pos = pos + 1;
    }
}
