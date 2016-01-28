//
// Copyright (C) 2012, OMBT LLC and Mike A. Rumore
// All rights reserved.
// Contact: Mike A. Rumore, (mike.a.rumore@gmail.com)
//
#ifndef __OMBT_CONFIG_DATA_H
#define __OMBT_CONFIG_DATA_H

// generic config data class

// os headers
#include <stdio.h>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

// headers
#include "atomic/BaseObject.h"
#include "atomic/ExtUseCntPtr.h"
#include "errors/Errors.h"
#include "stringutils/StdStringUtils.h"

namespace ombt {

#define PARSERENTRY(TYPE_NAME, \
                    FIELD_NAME, \
                    FIELD_TYPE, \
                    DEFAULT_VALUE, \
                    ENV_VAR) \
{ \
    #FIELD_NAME, \
    FIELD_TYPE, \
    DEFAULT_VALUE, \
    ENV_VAR, \
    (long TYPE_NAME::*)&TYPE_NAME::_##FIELD_NAME \
}

#define PARSERCHARENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::Char, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERUCHARENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::UnsignedChar, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERSHORTENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::Short, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERUSHORTENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::UnsignedShort, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERINTEGERENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::Integer, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERUINTEGERENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::UnsignedInteger, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERLONGENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::Long, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERULONGENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::UnsignedLong, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERLOATENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::Float, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERDOUBLEENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::Double, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERSTRINGENTRY(TYPE_NAME, FIELD_NAME, DEFAULT_VALUE, ENV_VAR) \
        PARSERENTRY(TYPE_NAME, FIELD_NAME, ConfigData::String, \
                    DEFAULT_VALUE, ENV_VAR)

#define PARSERLASTENTRY() \
	{ NULL, ConfigData::Last, NULL, false, 0 }

#undef THIS_FLD
#define THIS_FLD(CONFIG_DATA_TYPE, FLD_TYPE, TABLE_ENTRY) \
    (this->*(FLD_TYPE CONFIG_DATA_TYPE::*)TABLE_ENTRY._offset)

#undef OBJECT_FLD
#define OBJECT_FLD(CONFIG_DATA_TYPE, OBJECT, FLD_TYPE, TABLE_ENTRY) \
    (OBJECT.*(FLD_TYPE CONFIG_DATA_TYPE::*)TABLE_ENTRY._offset)

#undef ASSIGN_THIS_FLD
#define ASSIGN_THIS_FLD(CONFIG_DATA_TYPE, OBJECT, FLD_TYPE, TABLE_ENTRY) \
    THIS_FLD(CONFIG_DATA_TYPE, FLD_TYPE, TABLE_ENTRY) = \
    OBJECT_FLD(CONFIG_DATA_TYPE, OBJECT, FLD_TYPE, TABLE_ENTRY)

#undef DEFAULT_THIS_FLD
#define DEFAULT_THIS_FLD(CONFIG_DATA_TYPE, FLD_TYPE, \
                         FLD_DEFAULT, TABLE_ENTRY) \
{ \
    FLD_TYPE dflt_val = FLD_DEFAULT; \
    if (TABLE_ENTRY._default_value != NULL) \
    { \
        std::istringstream iss(pte._default_value); \
        iss >> dflt_val; \
    } \
    THIS_FLD(CONFIG_DATA_TYPE, FLD_TYPE, TABLE_ENTRY) = dflt_val; \
}

#undef STRING_THIS_FLD
#define STRING_THIS_FLD(CONFIG_DATA_TYPE, FLD_TYPE, \
                        STRING_VALUE, TABLE_ENTRY) \
{ \
    std::istringstream iss(STRING_VALUE); \
    iss >> THIS_FLD(CONFIG_DATA_TYPE, FLD_TYPE, TABLE_ENTRY); \
}

#undef PUTENV_THIS_FLD
#define PUTENV_THIS_FLD(CONFIG_DATA_TYPE, FLD_TYPE, TABLE_ENTRY) \
{ \
    std::ostringstream oss; \
    std::string name(TABLE_ENTRY._name); \
    ombt::to_uppercase(name); \
    oss << name \
        << "=" \
        << THIS_FLD(CONFIG_DATA_TYPE, FLD_TYPE, TABLE_ENTRY); \
    char *pnv = new char [oss.str().size()+1]; \
    ::strcpy(pnv, oss.str().c_str()); \
    ::putenv(pnv); \
}

#undef WRITE_THIS_FLD
#define WRITE_THIS_FLD(CONFIG_DATA_TYPE, OFS, FLD_TYPE, TABLE_ENTRY) \
{ \
    std::string name(TABLE_ENTRY._name); \
    ombt::to_uppercase(name); \
    ofs << name \
        << "=" \
        << THIS_FLD(CONFIG_DATA_TYPE, FLD_TYPE, TABLE_ENTRY) \
        << std::endl; \
}

// config file class
template <class ConfigDataType>
class ConfigData: public ConfigDataType
{
public:
    // basic types
    enum BasicTypes
    {
        Char,
        UnsignedChar,
        Short,
        UnsignedShort,
        Integer,
        UnsignedInteger,
        Long,
        UnsignedLong,
        Float,
        Double,
        String,
        Last
    };

    struct ParserTableEntry
    {
        const char *_name;
        BasicTypes  _type;
        const char *_default_value;
        bool        _env_var;
        long ConfigDataType::* _offset;
    };

public:
    // ctors and dtor
    ConfigData();
    ConfigData(const ConfigData &src);
    ~ConfigData();

    // assignment
    ConfigData &operator=(const ConfigData &rhs);

    // loading and writing data
    void initialize();
    void assign(const ConfigData &src);
    int read(const std::string &cfgfile);
    void put_env() const;
    int write(const std::string &cfgfile) const;
    int write(std::ostream &os) const;

    static const ParserTableEntry _parser_table[];
};

#include "config/ConfigData.i"

}

#endif

