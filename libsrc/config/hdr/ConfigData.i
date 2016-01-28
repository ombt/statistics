//
// Copyright (C) 2012, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//

// ctors and dtor
template <class ConfigDataType>
ConfigData<ConfigDataType>::ConfigData()
{
    initialize();
}

template <class ConfigDataType>
ConfigData<ConfigDataType>::ConfigData(
    const ConfigData<ConfigDataType> &src)
{
    assign(src);
}

template <class ConfigDataType>
ConfigData<ConfigDataType>::~ConfigData()
{
}

// assignment
template <class ConfigDataType>
ConfigData<ConfigDataType> &
ConfigData<ConfigDataType>::operator=(const ConfigData<ConfigDataType> &rhs)
{
    if (this != &rhs)
    {
        assign(rhs);
    }
    return *this;
}

// init data
template <class ConfigDataType>
void
ConfigData<ConfigDataType>::initialize()
{
    for (int i=0; _parser_table[i]._type != Last; ++i)
    {
        const ParserTableEntry &pte = _parser_table[i];
        switch (pte._type)
        {
        case Char:
        {
            DEFAULT_THIS_FLD(ConfigDataType, char, 0, pte);
            break;
        }
        case UnsignedChar:
        {
            DEFAULT_THIS_FLD(ConfigDataType, unsigned char, 0, pte);
            break;
        }
        case Short:
        {
            DEFAULT_THIS_FLD(ConfigDataType, short, 0, pte);
            break;
        }
        case UnsignedShort:
        {
            DEFAULT_THIS_FLD(ConfigDataType, unsigned short, 0, pte);
            break;
        }
        case Integer:
        {
            DEFAULT_THIS_FLD(ConfigDataType, int, 0, pte);
            break;
        }
        case UnsignedInteger:
        {
            DEFAULT_THIS_FLD(ConfigDataType, unsigned int, 0, pte);
            break;
        }
        case Long:
        {
            DEFAULT_THIS_FLD(ConfigDataType, long, 0, pte);
            break;
        }
        case UnsignedLong:
        {
            DEFAULT_THIS_FLD(ConfigDataType, unsigned long, 0, pte);
            break;
        }
        case Double:
        {
            DEFAULT_THIS_FLD(ConfigDataType, double, 0, pte);
            break;
        }
        case Float:
        {
            DEFAULT_THIS_FLD(ConfigDataType, float, 0, pte);
            break;
        }
        case String:
        {
            if (pte._default_value != NULL)
            {
                THIS_FLD(ConfigDataType, std::string, pte) = pte._default_value;
            }
            else
            {
                THIS_FLD(ConfigDataType, std::string, pte) = "";
            }
            break;
        }
        }
    }
}

// assign data
template <class ConfigDataType>
void
ConfigData<ConfigDataType>::assign(
    const ConfigData<ConfigDataType> &other)
{
    for (int i=0; _parser_table[i]._type != Last; ++i)
    {
        const ParserTableEntry &pte = _parser_table[i];
        switch (pte._type)
        {
        case Char:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, char, pte);
            break;
        }
        case UnsignedChar:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, unsigned char, pte);
            break;
        }
        case Short:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, short, pte);
            break;
        }
        case UnsignedShort:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, unsigned short, pte);
            break;
        }
        case Integer:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, int, pte);
            break;
        }
        case UnsignedInteger:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, unsigned int, pte);
            break;
        }
        case Long:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, long, pte);
            break;
        }
        case UnsignedLong:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, unsigned long, pte);
            break;
        }
        case Float:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, float, pte);
            break;
        }
        case Double:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, double, pte);
            break;
        }
        case String:
        {
            ASSIGN_THIS_FLD(ConfigDataType, other, std::string, pte);
            break;
        }
        }
    }
}

// loading and writing data
template <class ConfigDataType>
int
ConfigData<ConfigDataType>::read(const std::string &cfgfile)
{
    // was a config file name given?
    if (cfgfile.size() == 0)
    {
        return -1;
    }

    // open file for read
    std::ifstream ifs(cfgfile.c_str(), std::ifstream::in);

    // read data in and save
    char buf[2*BUFSIZ+1];
    while (ifs.good())
    {
        // get next line in config file.
        ifs.getline(buf, BUFSIZ, '\n');

        // split line into name-value pairs using '=' as
        // a delimiter. convert name to upper case for 
        // consistency. do *not* convert or transform
        // value at all. we take the value exactly as is.
        std::string sbuf(buf);
        sbuf = trim(sbuf);
        std::istringstream iss(sbuf);

        std::string name;
        getline(iss, name, '=');
        name = trim(name);
        to_uppercase(name);

        std::string value;
        getline(iss, value, '=');
        value = trim(value);

        // check for valid pairs and skip comments, ie, 
        // any line with a '#' as the first non-whitespace char.
        if ((name != "") && (value != "") && (name[0] != '#'))
        {
            // scan for a match and assign to internal data if we find
            // a match. any thing that is not known is ignored.
            for (int i=0; _parser_table[i]._type != Last; ++i)
            {
                const ParserTableEntry &pte = _parser_table[i];
                if (pte._name != NULL)
                {
                    std::string pte_name(pte._name);
                    to_uppercase(pte_name);
                    if (name == pte_name)
                    {
                        switch (pte._type)
                        {
                        case Char:
                        {
                            STRING_THIS_FLD(ConfigDataType, char, 
                                            value, pte);
                            break;
                        }
                        case UnsignedChar:
                        {
                            STRING_THIS_FLD(ConfigDataType, unsigned char, 
                                            value, pte);
                            break;
                        }
                        case Short:
                        {
                            STRING_THIS_FLD(ConfigDataType, short, 
                                            value, pte);
                            break;
                        }
                        case UnsignedShort:
                        {
                            STRING_THIS_FLD(ConfigDataType, unsigned short, 
                                            value, pte);
                            break;
                        }
                        case Integer:
                        {
                            STRING_THIS_FLD(ConfigDataType, int, 
                                            value, pte);
                            break;
                        }
                        case UnsignedInteger:
                        {
                            STRING_THIS_FLD(ConfigDataType, unsigned int, 
                                            value, pte);
                            break;
                        }
                        case Long:
                        {
                            STRING_THIS_FLD(ConfigDataType, long, 
                                            value, pte);
                            break;
                        }
                        case UnsignedLong:
                        {
                            STRING_THIS_FLD(ConfigDataType, unsigned long, 
                                            value, pte);
                            break;
                        }
                        case Double:
                        {
                            STRING_THIS_FLD(ConfigDataType, double, 
                                            value, pte);
                            break;
                        }
                        case Float:
                        {
                            STRING_THIS_FLD(ConfigDataType, float, 
                                            value, pte);
                            break;
                        }
                        case String:
                        {
                            THIS_FLD(ConfigDataType, std::string, pte) = value;
                            break;
                        }
                        }
                    }
                }
            }
        }
    }

    ifs.close();

    return 0;
}

template <class ConfigDataType>
void
ConfigData<ConfigDataType>::put_env() const
{
    // scan fields which are marked and *env* and put into env.
    for (int i=0; _parser_table[i]._type != Last; ++i)
    {
        std::string value;
        const ParserTableEntry &pte = _parser_table[i];

        if (pte._env_var == true)
        {
            switch (pte._type)
            {
            case Char:
            {
                PUTENV_THIS_FLD(ConfigDataType, char, pte);
                break;
            }
            case UnsignedChar:
            {
                PUTENV_THIS_FLD(ConfigDataType, unsigned char, pte);
                break;
            }
            case Short:
            {
                PUTENV_THIS_FLD(ConfigDataType, short, pte);
                break;
            }
            case UnsignedShort:
            {
                PUTENV_THIS_FLD(ConfigDataType, unsigned short, pte);
                break;
            }
            case Integer:
            {
                PUTENV_THIS_FLD(ConfigDataType, int, pte);
                break;
            }
            case UnsignedInteger:
            {
                PUTENV_THIS_FLD(ConfigDataType, unsigned int, pte);
                break;
            }
            case Long:
            {
                PUTENV_THIS_FLD(ConfigDataType, long, pte);
                break;
            }
            case UnsignedLong:
            {
                PUTENV_THIS_FLD(ConfigDataType, unsigned long, pte);
                break;
            }
            case Double:
            {
                PUTENV_THIS_FLD(ConfigDataType, double, pte);
                break;
            }
            case Float:
            {
                PUTENV_THIS_FLD(ConfigDataType, float, pte);
                break;
            }
            case String:
            {
                PUTENV_THIS_FLD(ConfigDataType, std::string, pte);
                break;
            }
            }
        }
    }
}

template <class ConfigDataType>
int
ConfigData<ConfigDataType>::write(const std::string &cfgfile) const
{
    // was a config file name given?
    if (cfgfile.size() == 0)
    {
        return -1;
    }

    // open file for write
    std::ofstream ofs(cfgfile.c_str(), 
                      std::ofstream::out | std::ofstream::trunc);

    return write(ofs);
}

template <class ConfigDataType>
int
ConfigData<ConfigDataType>::write(std::ostream &ofs) const
{
    for (int i=0; _parser_table[i]._type != Last; ++i)
    {
        const ParserTableEntry &pte = _parser_table[i];
        switch (pte._type)
        {
        case Char:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, char, pte);
            break;
        }
        case UnsignedChar:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, unsigned char, pte);
            break;
        }
        case Short:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, short, pte);
            break;
        }
        case UnsignedShort:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, unsigned short, pte);
            break;
        }
        case Integer:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, int, pte);
            break;
        }
        case UnsignedInteger:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, unsigned int, pte);
            break;
        }
        case Long:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, long, pte);
            break;
        }
        case UnsignedLong:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, unsigned long, pte);
            break;
        }
        case Float:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, float, pte);
            break;
        }
        case Double:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, double, pte);
            break;
        }
        case String:
        {
            WRITE_THIS_FLD(ConfigDataType, ofs, std::string, pte);
            break;
        }
        }
    }

    return 0;
}

