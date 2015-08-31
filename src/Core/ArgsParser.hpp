#ifndef ARGSPARSER_HPP_
#define ARGSPARSER_HPP_

#include <vector>
#include <map>
#include <string>
#include <iosfwd>
#include "boost/smart_ptr.hpp"
#include "VerificationOptions.hpp"
#include "boost/lexical_cast.hpp"

namespace VerifyTAPN
{
	const int WIDTH = 40; // TODO: determine this based on registered switches

	struct Version
	{
	public:
		Version(unsigned int maj, unsigned int min, unsigned int build) : maj(maj), min(min), build(build) { };
		unsigned int maj;
		unsigned int min;
		unsigned int build;
		// TODO: maybe add revision?
	};

	std::ostream& operator <<(std::ostream& out, const Version& version);


	typedef std::map<std::string, std::string> option_map;
	typedef option_map::value_type option;

	class Switch
	{
		public:
			Switch(const std::string& name, const std::string& long_name, const std::string& description) : name(name), long_name(long_name), description(description), handled_option(false) { };
			virtual ~Switch() { };
		public:
			inline const std::string& getShortName() const { return name; };
			inline const std::string& getLongName() const { return long_name; };
			inline const std::string& getDescription() const { return description; };
			virtual bool handles(const std::string& flag) const;
			virtual option parse(const std::string& flag);
			virtual void print(std::ostream& out) const;
			inline bool handledOption() const { return handled_option; };
			virtual option getDefaultOption() const { return option(long_name, "0"); };
		private:
			std::string name;
			std::string long_name;
			std::string description;
		protected:
			bool handled_option;
	};

	std::ostream& operator<<(std::ostream& out, const Switch& flag);

	class SwitchWithStringArg : public Switch
	{
	public:
		SwitchWithStringArg(const std::string& name, const std::string& long_name, const std::string& description, const std::string& default_value) : Switch(name, long_name, description), default_value(default_value) { };
		virtual ~SwitchWithStringArg() { };
		virtual option parse(const std::string& flag);
		virtual void print(std::ostream& out) const;
		virtual option getDefaultOption() const { return option(getLongName(), default_value); };
	private:
		std::string default_value;
	};

	class SwitchWithArg : public Switch
	{
	public:
		SwitchWithArg(const std::string& name, const std::string& long_name, const std::string& description, unsigned int default_value) : Switch(name, long_name, description), default_value(default_value) { };
		virtual ~SwitchWithArg() { };
		virtual option parse(const std::string& flag);
		virtual void print(std::ostream& out) const;
		virtual option getDefaultOption() const { return option(getLongName(), boost::lexical_cast<std::string>(default_value)); };
	private:
		unsigned int default_value;
	};

	class ArgsParser {
		typedef std::vector< boost::shared_ptr<Switch> > parser_vec;
	public:
		ArgsParser() : parsers(), version(3,2,0) { initialize(); };
		virtual ~ArgsParser() {};

		VerificationOptions parse(int argc, char* argv[]) const;
	private:
		VerificationOptions createVerificationOptions(const option_map& map) const;
                VerificationOptions verifyInputFiles(VerificationOptions, std::string model_file, std::string query_file) const;
		unsigned int tryParseInt(const option& option) const;
		unsigned long long tryParseLongLong(const option& option) const;
		std::vector<std::string> parseIncPlaces(const std::string& string) const;
		void initialize();
		void printHelp() const;
		void printVersion() const;
	private: // data
		parser_vec parsers;
		VerifyTAPN::Version version;
	};
}

#endif /* ARGSPARSER_HPP_ */
