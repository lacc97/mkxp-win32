#include "iniconfig.h"

#include <fstream>
#include <map>

#include <boost/lexical_cast.hpp>
#include <utility>

#include "utils.h"

template<typename TK, typename TV>
std::vector<TK> extract_keys(std::map<TK, TV> const& input_map) {
    std::vector<TK> retval;
    retval.reserve(input_map.size());

    for(const auto& element : input_map)
        retval.push_back(element.first);

    return retval;
}

template<typename TK, typename TV>
std::vector<TV> extract_values(std::map<TK, TV> const& input_map) {
    std::vector<TV> retval;
    retval.reserve(input_map.size());

    for(const auto& element : input_map)
        retval.push_back(element.second);

    return retval;
}

INIConfiguration::Section::Section(std::string  sName) : m_Name(std::move(sName)), m_PropertyMap() {
}

std::optional<std::string> INIConfiguration::Section::getStringProperty(const std::string& name) const {
    try {
        return m_PropertyMap.at(str::to_lower(name)).m_Value;
    } catch(std::out_of_range& oorexcept) {
        return std::nullopt;
    }
}

std::optional<std::vector<std::string>>
INIConfiguration::Section::getStringListProperty(const std::string& name) const {
    try {
        std::string_view propVal = m_PropertyMap.at(str::to_lower(name)).m_Value;

        std::vector<std::string> list = str::tokenize(propVal, ",");
        std::for_each(list.begin(), list.end(), [](std::string& s) {
          s = str::view::trim(s);
        });

        return list;
    } catch(std::out_of_range& oorexcept) {
        return std::nullopt;
    }
}

std::optional<int32_t> INIConfiguration::Section::getIntProperty(const std::string& name) const {
    try {
        return boost::lexical_cast<int32_t>(m_PropertyMap.at(str::to_lower(name)).m_Value);
    } catch(std::out_of_range& oorexcept) {
        return std::nullopt;
    }
}

std::optional<double> INIConfiguration::Section::getFloatProperty(const std::string& name) const {
    try {
        return boost::lexical_cast<double>(m_PropertyMap.at(str::to_lower(name)).m_Value);
    } catch(std::out_of_range& oorexcept) {
        return std::nullopt;
    }
}

std::vector<std::string> INIConfiguration::Section::getEntries() const {
    std::vector<std::string> entries;
    entries.reserve(m_PropertyMap.size());

    for(const auto& entry : m_PropertyMap) {
        std::stringstream ss;
        ss << entry.first << "=" << entry.second.m_Value;
        entries.push_back(ss.str());
    }

    return entries;
}

std::vector<std::string> INIConfiguration::Section::getKeyNames() const {
    return extract_keys(m_PropertyMap);
}

bool INIConfiguration::load(const std::filesystem::path& filepath) {
    std::ifstream fis(filepath);

    return load(fis);
}

bool INIConfiguration::load(std::istream& inStream) {
    if(!inStream.good())
        return false;

    std::string currSectionName;

    std::string line;
    std::getline(inStream, line);

    while(!inStream.eof() && !inStream.bad()) {
        if(line[0] == '[') {
            currSectionName = line.substr(1, line.find_last_of(']') - 1);
            addSection(currSectionName);
        } else if(line[0] != '#' && line.length() > 2) {
            unsigned long crloc = line.length() - 1;

            if(crloc >= 0 && line[crloc] == '\r') //check for Windows-style newline
                line = line.substr(0, crloc); //and correct

            std::string key, val;

            if(parseProperty(line, key, val))
                addProperty(currSectionName, key, val);
        }

        std::getline(inStream, line);
    }

    return !inStream.bad();
}

bool INIConfiguration::save(const std::filesystem::path& filepath) const {
    std::ofstream os(filepath);

    return save(os);
}

bool INIConfiguration::save(std::ostream& outStream) const {
    if(!outStream.good())
        return false;

    std::for_each(m_SectionMap.begin(), m_SectionMap.end(), [&](const std::pair<std::string, Section>& selem) {
        if(!selem.second.m_Name.empty())
            outStream << "[" << selem.second.m_Name << "]" << std::endl;

        std::for_each(selem.second.m_PropertyMap.begin(), selem.second.m_PropertyMap.end(),
                      [&](const Section::property_map::value_type& pelem) {
                          outStream << pelem.second.m_Name << "=" << pelem.second.m_Value << std::endl;
                      });

        outStream << std::endl;
    });

    return true;
}

void INIConfiguration::clearSection(const std::string& sName) {
    m_SectionMap.erase(sName);
}

std::string
INIConfiguration::getStringProperty(const std::string& sName, const std::string& name, const std::string& def) const {
    auto sectionOptional = getSection(sName);

    if(!sectionOptional)
        return def;

    const Section& section = *sectionOptional;

    auto propOptional = section.getStringProperty(name);

    if(!propOptional)
        return def;

    return (*propOptional);
}

std::vector<std::string> INIConfiguration::getStringListProperty(const std::string& sName, const std::string& name,
                                                                 const std::vector<std::string>& def) const {
    auto sectionOptional = getSection(sName);

    if(!sectionOptional)
        return def;

    const Section& section = *sectionOptional;

    auto propOptional = section.getStringListProperty(name);

    if(!propOptional)
        return def;

    return (*propOptional);
}

int32_t INIConfiguration::getIntProperty(const std::string& sName, const std::string& name, int32_t def) const {
    auto sectionOptional = getSection(sName);

    if(!sectionOptional)
        return def;

    const Section& section = *sectionOptional;

    auto propOptional = section.getIntProperty(name);

    if(!propOptional)
        return def;

    return (*propOptional);
}

double INIConfiguration::getFloatProperty(const std::string& sName, const std::string& name, double def) const {
    auto sectionOptional = getSection(sName);

    if(!sectionOptional)
        return def;

    const Section& section = *sectionOptional;

    auto propOptional = section.getFloatProperty(name);

    if(!propOptional)
        return def;

    return (*propOptional);
}

void INIConfiguration::addSection(const std::string& sName) {
    if(!getSection(str::to_lower(sName)))
        m_SectionMap.emplace(str::to_lower(sName), Section(sName));
}

void INIConfiguration::addRawProperty(const std::string& sName, const std::string& raw) {
    std::string key, val;

    if(parseProperty(raw, key, val))
        addProperty(sName, key, val);
}

void INIConfiguration::setStringProperty(const std::string& sName, const std::string& name, const std::string& val) {
    if(!getSection(sName))
        addSection(sName);

    addProperty(sName, name, val);
}

void INIConfiguration::setStringListProperty(const std::string& sName, const std::string& name,
                                             const std::vector<std::string>& val) {
    if(!getSection(sName))
        addSection(sName);

    std::stringstream ss;

    for(auto it = val.begin(); it != val.end(); it++) {
        if(it != val.begin())
            ss << ",";

        ss << (*it);
    }

    addProperty(sName, name, ss.str());
}

void INIConfiguration::setIntProperty(const std::string& sName, const std::string& name, int32_t val) {
    if(!getSection(sName))
        addSection(sName);

    addProperty(sName, name, std::to_string(val));
}

void INIConfiguration::setFloatProperty(const std::string& sName, const std::string& name, double val) {
    if(!getSection(sName))
        addSection(sName);

    addProperty(sName, name, std::to_string(val));
}

std::vector<std::string> INIConfiguration::getSectionNames() const {
    return extract_keys(m_SectionMap);
}

std::vector<std::string> INIConfiguration::getEntries(const std::string& sName) const {
    auto sectionOptional = getSection(sName);

    if(!sectionOptional)
        return std::vector<std::string>();

    return sectionOptional->get().getEntries();
}

std::vector<std::string> INIConfiguration::getKeyNames(const std::string& sName) const {
    auto sectionOptional = getSection(sName);

    if(!sectionOptional)
        return std::vector<std::string>();

    return sectionOptional->get().getKeyNames();
}

bool INIConfiguration::parseProperty(const std::string& raw, std::string& key, std::string& val) {
    size_t equalsPos = raw.find_first_of('=');

    if(equalsPos != std::string::npos) {
        key = raw.substr(0, equalsPos);
        val = raw.substr(equalsPos + 1);

        if(val[0] == '"' || val[0] == '\'')
            val = val.substr(1);

        if(val[val.length() - 1] == '"' || val[val.length() - 1] == '\'')
            val.resize(val.length() - 1);

        return true;
    }

    return false;
}

void INIConfiguration::addProperty(const std::string& sName, const std::string& name, const std::string& val) {
    Section::Property p;
    p.m_Name = str::view::trim(name);
    p.m_Value = str::view::trim(val);

    m_SectionMap.at(str::to_lower(sName)).m_PropertyMap[str::to_lower(p.m_Name)] = p;
}

std::optional<std::reference_wrapper<const INIConfiguration::Section>> INIConfiguration::getSection(const std::string& sName) const {
    try {
        return m_SectionMap.at(str::to_lower(sName));
    } catch(std::out_of_range& ex) {
        return std::nullopt;
    }
}
