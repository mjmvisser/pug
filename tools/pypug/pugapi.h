#include <string>
#include <boost/python/dict.hpp>

class Root;

class PugAPI
{
public:
    PugAPI(const std::string config);
    ~PugAPI();

    boost::python::dict parse(const std::string node, const std::string path) const;
    const std::string map(const std::string node, const boost::python::dict& dict) const;

private:
    Root* m_root;
};
