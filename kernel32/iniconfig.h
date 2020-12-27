#ifndef INICONFIG_H
#define INICONFIG_H

#include <filesystem>
#include <iostream>
#include <map>
#include <optional>
#include <vector>

class INIConfiguration {
  friend class INIParser;

  class Section {
    friend class INIConfiguration;

    friend class INIParser;

    struct Property {
      std::string m_Name;
      std::string m_Value;
    };

    typedef std::map<std::string, Property, std::less<>> property_map;

   public:
    Section(const Section& s) = default;
    Section(Section&& s) = default;

    std::optional<std::string> getStringProperty(std::string_view name) const;
    std::optional<std::vector<std::string>> getStringListProperty(std::string_view name) const;
    std::optional<int32_t> getIntProperty(std::string_view name) const;
    std::optional<double> getFloatProperty(std::string_view name) const;

    std::vector<std::string> getEntries() const;
    std::vector<std::string> getKeyNames() const;

   private:
    explicit Section(std::string sname);

    std::string m_Name;
    property_map m_PropertyMap;
  };

  typedef std::map<std::string, Section, std::less<>> section_map;

 public:
  bool load(const std::filesystem::path& filepath);
  bool load(std::istream& inStream);

  bool save(const std::filesystem::path& filepath) const;
  bool save(std::ostream& outStream) const;

  void clearSection(std::string_view sname);

  std::string getStringProperty(std::string_view sname, std::string_view name, std::string_view def = "") const;
  std::vector<std::string> getStringListProperty(std::string_view sName, std::string_view name,
                                                 const std::vector<std::string>& def = std::vector<std::string>()) const;
  int32_t getIntProperty(std::string_view sName, std::string_view name, int32_t def = 0) const;
  double getFloatProperty(std::string_view sName, std::string_view name, double def = 0.0) const;

  void addSection(std::string_view sname);
  void addRawProperty(std::string_view sname, std::string_view raw);

  void setStringProperty(std::string_view sname, std::string_view name, std::string_view val);
  void setStringListProperty(std::string_view sname, std::string_view name, const std::vector<std::string>& val);
  void setIntProperty(std::string_view sname, std::string_view name, int32_t val);
  void setFloatProperty(std::string_view sname, std::string_view name, double val);

  std::vector<std::string> getSectionNames() const;
  std::vector<std::string> getEntries(std::string_view sname) const;
  std::vector<std::string> getKeyNames(std::string_view sname) const;

 protected:
  static bool parseProperty(std::string_view raw, std::string_view& key, std::string_view& val);

  void addProperty(std::string_view sname, std::string_view name, std::string_view val);

  std::optional<std::reference_wrapper<const Section>> getSection(std::string_view sname) const;

 private:
  section_map m_SectionMap;
};

#endif    // CONFIGURATION_H
