namespace easy_options {
struct nil {};
struct UnrecognizedOption {
};

struct FlagPresent {
	int present;
	FlagPresent() : present(0) {}
};
struct StringArgument {
	std::string arg;
	StringArgument() {}
};
struct RequiredArg {
	static const int is_arg_required = 1;
};

template <class T> struct Flag;


template <class T> struct MyGetOpt;
template <       > struct MyGetOpt<nil> {
	static string stringForGetOpt() { return ""; }
	bool tryToAccept(const int option, const char * optarg) {
		return false;
	}
};
template <class T> struct MyGetOpt : public Flag<typename T::first_type>, public MyGetOpt<typename T::second_type>    {
	static string stringForGetOpt() {
		ostringstream sstr;
		sstr << Flag<typename T::first_type>::option_character;
		if (Flag<typename T::first_type>:: is_arg_required)
			sstr << ":";
		sstr << MyGetOpt<typename T::second_type> :: stringForGetOpt();
		return sstr.str();
       	}
	template <typename arg_tag>
	std::string & get() { return this->Flag<arg_tag>::arg; }
	template <typename arg_tag>
	std::string get_(const std::string the_default) { return this->Flag<arg_tag>::present ? this->Flag<arg_tag>::arg : the_default; }
	bool tryToAccept(const int option, const char * optarg) {
		if(option ==  Flag<typename T::first_type>::option_character) {
			this->Flag<typename T::first_type> :: arg = optarg;
			this->Flag<typename T::first_type> :: present = 1;
			return true;
		} else {
			return this->MyGetOpt<typename T::second_type>::tryToAccept(option, optarg);
		}
	}
	void processOptions(int argc, char ** argv) {
		const std::string list_of_options = this->stringForGetOpt();
		while(1) {
			int option = getopt_long(argc, argv, list_of_options.c_str(), NULL, NULL);
			if(option == EOF)
				break;
			if(option == '?') {
				throw easy_options:: UnrecognizedOption();
				break;
			}
			bool wasAccepted =
				this->tryToAccept(option, optarg);
			if(!wasAccepted) {
				throw easy_options:: UnrecognizedOption();
				break;
			}
		}
	}
};
} // namespace easy_options
