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

    typedef std::map<std::string, Property> property_map;

   public:
    Section(const Section& s) = default;
    Section(Section&& s) = default;

    std::optional<std::string> getStringProperty(const std::string& name) const;
    std::optional<std::vector<std::string>> getStringListProperty(const std::string& name) const;
    std::optional<int32_t> getIntProperty(const std::string& name) const;
    std::optional<double> getFloatProperty(const std::string& name) const;

    std::vector<std::string> getEntries() const;
    std::vector<std::string> getKeyNames() const;

   private:
    explicit Section(std::string sname);

    std::string m_Name;
    property_map m_PropertyMap;
  };

  typedef std::map<std::string, Section> section_map;

 public:
  bool load(const std::filesystem::path& filepath);
  bool load(std::istream& inStream);

  bool save(const std::filesystem::path& filepath) const;
  bool save(std::ostream& outStream) const;

  void clearSection(const std::string& sname);

  std::string getStringProperty(const std::string& sname, const std::string& name, const std::string& def = "") const;
  std::vector<std::string> getStringListProperty(const std::string& sName, const std::string& name,
                                                 const std::vector<std::string>& def = std::vector<std::string>()) const;
  int32_t getIntProperty(const std::string& sName, const std::string& name, int32_t def = 0) const;
  double getFloatProperty(const std::string& sName, const std::string& name, double def = 0.0) const;

  void addSection(const std::string& sname);
  void addRawProperty(const std::string& sname, const std::string& raw);

  void setStringProperty(const std::string& sname, const std::string& name, const std::string& val);
  void setStringListProperty(const std::string& sname, const std::string& name, const std::vector<std::string>& val);
  void setIntProperty(const std::string& sname, const std::string& name, int32_t val);
  void setFloatProperty(const std::string& sname, const std::string& name, double val);

  std::vector<std::string> getSectionNames() const;
  std::vector<std::string> getEntries(const std::string& sname) const;
  std::vector<std::string> getKeyNames(const std::string& sname) const;

 protected:
  static bool parseProperty(const std::string& raw, std::string& key, std::string& val);

  void addProperty(const std::string& sname, const std::string& name, const std::string& val);

  std::optional<std::reference_wrapper<const Section>> getSection(const std::string& sname) const;

 private:
  section_map m_SectionMap;
};

#endif    // CONFIGURATION_H
