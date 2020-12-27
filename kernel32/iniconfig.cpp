#include "iniconfig.h"

#include <charconv>
#include <fstream>
#include <map>

#include <fmt/format.h>

#include <boost/lexical_cast.hpp>
#include <utility>

#include "utils.h"

template <typename TK, typename TV, typename Compare>
std::vector<TK> extract_keys(std::map<TK, TV, Compare> const& input_map) {
  std::vector<TK> retval;
  retval.reserve(input_map.size());

  for(const auto& element : input_map)
    retval.push_back(element.first);

  return retval;
}

template <typename TK, typename TV, typename Compare>
std::vector<TV> extract_values(std::map<TK, TV, Compare> const& input_map) {
  std::vector<TV> retval;
  retval.reserve(input_map.size());

  for(const auto& element : input_map)
    retval.push_back(element.second);

  return retval;
}

INIConfiguration::Section::Section(std::string sName) : m_Name(std::move(sName)), m_PropertyMap() {}

std::optional<std::string> INIConfiguration::Section::getStringProperty(std::string_view name) const {
  try {
    return m_PropertyMap.at(str::to_lower(name)).m_Value;
  } catch(std::out_of_range& oorexcept) { return std::nullopt; }
}

std::optional<std::vector<std::string>> INIConfiguration::Section::getStringListProperty(std::string_view name) const {
  try {
    std::string_view propVal = m_PropertyMap.at(str::to_lower(name)).m_Value;

    std::vector<std::string> list = str::tokenize(propVal, ",");
    std::for_each(list.begin(), list.end(), [](std::string& s) {
      s = str::view::trim(s);
    });

    return list;
  } catch(std::out_of_range& oorexcept) { return std::nullopt; }
}

std::optional<int32_t> INIConfiguration::Section::getIntProperty(std::string_view name) const {
  std::string_view prop = m_PropertyMap.at(str::to_lower(name)).m_Value;

  int32_t base = 10;
  if(str::view::starts_with(prop, "0x") || str::view::starts_with(prop, "0X")) {
    base = 16;
    prop = prop.substr(2);
  }
  //  else if(str::view::starts_with(prop, "0")) {
  //    base = 8;
  //    prop = prop.substr(1);
  //  }

  int32_t val;
  auto [ptr, ec] = std::from_chars(prop.data(), prop.data() + prop.size(), val, base);
  if(ec == std::errc())
    return val;
  else
    return std::nullopt;
}

std::optional<double> INIConfiguration::Section::getFloatProperty(std::string_view name) const {
  try {
    return boost::lexical_cast<double>(m_PropertyMap.at(str::to_lower(name)).m_Value);
  } catch(std::out_of_range& oorexcept) { return std::nullopt; }
}

std::vector<std::string> INIConfiguration::Section::getEntries() const {
  std::vector<std::string> entries;
  entries.reserve(m_PropertyMap.size());

  for(const auto& entry : m_PropertyMap)
    entries.push_back(fmt::format("{0} = {1}", entry.first, entry.second.m_Value));

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
  while(std::getline(inStream, line)) {
    if(line[0] == '[') {
      currSectionName = line.substr(1, line.find_last_of(']') - 1);
      addSection(currSectionName);
    } else if(line[0] != '#' && line.length() > 2) {
      unsigned long crloc = line.length() - 1;

      if(crloc >= 0 && line[crloc] == '\r')    //check for Windows-style newline
        line = line.substr(0, crloc);          //and correct

      std::string_view key, val;
      if(parseProperty(line, key, val))
        addProperty(currSectionName, key, val);
    }
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
      fmt::format_to(std::ostream_iterator<char>(outStream), "[{0}]\n", selem.second.m_Name);

    std::for_each(selem.second.m_PropertyMap.begin(), selem.second.m_PropertyMap.end(), [&outStream](const Section::property_map::value_type& pelem) {
      fmt::format_to(std::ostream_iterator<char>(outStream), "{0} = {1}\n", pelem.second.m_Name, pelem.second.m_Value);
    });

    fmt::format_to(std::ostream_iterator<char>(outStream), "\n");
  });

  return true;
}

void INIConfiguration::clearSection(std::string_view sName) {
  m_SectionMap.erase(m_SectionMap.find(sName));
}

std::string INIConfiguration::getStringProperty(std::string_view sName, std::string_view name, std::string_view def) const {
  auto sectionOptional = getSection(sName);

  if(!sectionOptional)
    return std::string(def);

  const Section& section = *sectionOptional;

  auto propOptional = section.getStringProperty(name);

  if(!propOptional)
    return std::string(def);

  return (*propOptional);
}

std::vector<std::string> INIConfiguration::getStringListProperty(std::string_view sName, std::string_view name, const std::vector<std::string>& def) const {
  auto sectionOptional = getSection(sName);

  if(!sectionOptional)
    return def;

  const Section& section = *sectionOptional;

  auto propOptional = section.getStringListProperty(name);

  if(!propOptional)
    return def;

  return (*propOptional);
}

int32_t INIConfiguration::getIntProperty(std::string_view sName, std::string_view name, int32_t def) const {
  auto sectionOptional = getSection(sName);

  if(!sectionOptional)
    return def;

  const Section& section = *sectionOptional;

  auto propOptional = section.getIntProperty(name);

  if(!propOptional)
    return def;

  return (*propOptional);
}

double INIConfiguration::getFloatProperty(std::string_view sName, std::string_view name, double def) const {
  auto sectionOptional = getSection(sName);

  if(!sectionOptional)
    return def;

  const Section& section = *sectionOptional;

  auto propOptional = section.getFloatProperty(name);

  if(!propOptional)
    return def;

  return (*propOptional);
}

void INIConfiguration::addSection(std::string_view sName) {
  if(!getSection(str::to_lower(sName)))
    m_SectionMap.emplace(str::to_lower(sName), Section(std::string(sName)));
}

void INIConfiguration::addRawProperty(std::string_view sName, std::string_view raw) {
  std::string_view key, val;
  if(parseProperty(raw, key, val))
    addProperty(sName, key, val);
}

void INIConfiguration::setStringProperty(std::string_view sName, std::string_view name, std::string_view val) {
  if(!getSection(sName))
    addSection(sName);

  addProperty(sName, name, val);
}

void INIConfiguration::setStringListProperty(std::string_view sName, std::string_view name, const std::vector<std::string>& val) {
  if(!getSection(sName))
    addSection(sName);

  addProperty(sName, name, fmt::format("{0}", fmt::join(val, ",")));
}

void INIConfiguration::setIntProperty(std::string_view sName, std::string_view name, int32_t val) {
  if(!getSection(sName))
    addSection(sName);

  addProperty(sName, name, fmt::format("{0}", val));
}

void INIConfiguration::setFloatProperty(std::string_view sName, std::string_view name, double val) {
  if(!getSection(sName))
    addSection(sName);

  addProperty(sName, name, fmt::format("{0}", val));
}

std::vector<std::string> INIConfiguration::getSectionNames() const {
  return extract_keys(m_SectionMap);
}

std::vector<std::string> INIConfiguration::getEntries(std::string_view sName) const {
  auto sectionOptional = getSection(sName);

  if(!sectionOptional)
    return std::vector<std::string>();

  return sectionOptional->get().getEntries();
}

std::vector<std::string> INIConfiguration::getKeyNames(std::string_view sName) const {
  auto sectionOptional = getSection(sName);

  if(!sectionOptional)
    return std::vector<std::string>();

  return sectionOptional->get().getKeyNames();
}

bool INIConfiguration::parseProperty(std::string_view raw, std::string_view& key, std::string_view& val) {
  size_t equalsPos = raw.find_first_of('=');

  if(equalsPos != std::string::npos) {
    key = raw.substr(0, equalsPos);
    val = raw.substr(equalsPos + 1);

    if(val.front() == '"' || val.front() == '\'')
      val = val.substr(1);

    if(val.back() == '"' || val.back() == '\'')
      val.substr(0, val.length() - 1);

    return true;
  }

  return false;
}

void INIConfiguration::addProperty(std::string_view sName, std::string_view name, std::string_view val) {
  Section::Property p;
  p.m_Name = str::view::trim(name);
  p.m_Value = str::view::trim(val);

  m_SectionMap.at(str::to_lower(sName)).m_PropertyMap[str::to_lower(p.m_Name)] = p;
}

std::optional<std::reference_wrapper<const INIConfiguration::Section>> INIConfiguration::getSection(std::string_view sName) const {
  try {
    return m_SectionMap.at(str::to_lower(sName));
  } catch(std::out_of_range& ex) { return std::nullopt; }
}
