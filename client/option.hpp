/*
 * options.hpp
 *
 *  Created on: 2018年2月11日
 *      Author: luguanglong
 */

#ifndef OPTION_HPP_
#define OPTION_HPP_

namespace shark{

	typedef enum {
		COMMAND_BLANK = 0,
		COMMAND_CREATE,
		COMMAND_DELETE,
		COMMAND_EXEC,
		COMMAND_LIST,
		COMMAND_MAX
	} COMMAND_TYPE;

	struct Command{
		std::string id;
		COMMAND_TYPE type;
		//struct Config cfg;
		std::string execCmd;
	};

	class Option{
	public:
		const char *process(int argc, char *argv[]);
		struct Command& getCmd(){
			return cmd;
		};

		std::string& getCmdJson(){
			return jsonCmd;
		};

	private:
		int listProcess(char *data);
		int printHelp();

		std::string cmdGenerate(struct Command &opt);

		struct Command cmd;
		std::string jsonCmd;
	};
}

#endif /* OPTION_HPP_ */
