#pragma once
#include <sstream>
#include <libconfig.h++>
#include "Exception.h"

namespace Commutator {

	class ConfigurationException : public Exception { 
	    public:
		ConfigurationException(size_t code, const std::string& message)
		    : Exception(code, message)
		{}
	};

	class Configuration
	{

	    private:
	        std::string db_filename_;
	        libconfig::Config cfg_;
	        std::string configuration_filename_;

	    public:

	        Configuration(const std::string& filename)
	            :configuration_filename_(filename)
	        {
	            try {
	                cfg_.readFile(configuration_filename_.c_str());
	                cfg_.lookupValue("system.db.filename", this->db_filename_);
	            }
	            catch(const libconfig::FileIOException &fioex) {
	                throw new ConfigurationException(1, "I/O error while reading file.");
	            }
	            catch(const libconfig::ParseException &pex) {
	                std::stringstream ss;
	                ss << "Parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError();
	                throw new ConfigurationException(2, ss.str());
	            }
	        }

	        const std::string& getDbFilename() const
	        {
	            return db_filename_;
	        }
	};

}