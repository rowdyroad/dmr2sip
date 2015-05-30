#pragma once

#include <libconfig.h++>
#include "Exception.h"

namespace Commutator {

	class ConfigurationException : public Exception { };

	class Configuration
	{

	    private:
	        size_t device_;
	        std::string db_filename_;
	        Config cfg_;
	        std::string configuration_filename_;

	    public:

	        Configuration(const std::string& filename)
	            :configuration_filename_(filename)
	        {
	            try {
	                cfg.readFile(configuration_filename_);
	                cfg.lookupValue("system.device", this->device_);
	                cfg.lookupValue("system.db.filename", this->db_filename_);
	            }
	            catch(const FileIOException &fioex) {
	                throw new ConfigurationException(1, "I/O error while reading file.");
	            }
	            catch(const ParseException &pex) {
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