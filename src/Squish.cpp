#include "BasicTypes.h"
#include "BitIO.h"
#include "Codec.h"
#include "Lzw.h"
#include "Archive.h"

#include <memory>
#include <iostream>
#include <exception>
#include <vector>

struct SquishState;

class ICommand
{
public:
	virtual bool execute(const SquishState&) = 0;
};

struct SquishState
{
	std::vector<std::string> files;
	std::shared_ptr<ICommand> command;
};

class ListContentsCommand : public ICommand
{
public:
	virtual bool execute(const SquishState& state) 
	{
		for(auto i:state.files)
		{
			std::cout << "[" << i << "]" << std::endl;
			auto f = std::shared_ptr<BitFileReader>(new BitFileReader(i.c_str()));
			auto a = Archive::load(f);
			auto c = a->contents();
			for(auto j:c)
				std::cout << j << std::endl;
		}
		return true;
	}
};
std::shared_ptr<ICommand> processCommand(std::string& cmd)
{
	if(cmd.size() < 2)
	{
		std::cout << cmd << " not recognised" << std::endl;
		return nullptr;
	}		
	switch(cmd[1])
	{
		// extract.
		case 'x':
		{
			std::cout << "not yet implemented... " << std::endl;
			return nullptr;
		}
	
		// add.
		case 'a':
		{
			std::cout << "not yet implemented... " << std::endl;
			return nullptr;
		}

		// list contents.
		case 'l':
		{
			return std::shared_ptr<ICommand>(new ListContentsCommand());
		}

		// error.
		default:
		{
			std::cout << cmd << " not recognised" << std::endl;
			return nullptr;
		}
	}
	return nullptr;	
}

bool processArgs(std::vector<std::string>& args, SquishState& state)
{
	for(auto i:args)
	{
		// Command?
		if('-' == i[0] || '/' == i[0])
		{
			if(state.command)
			{
				std::cout << "error: only one operation at any one time." << std::endl;
				return false;
			}

			state.command = processCommand(i);
		}
		else
		{		
			// Otherwise assume filename.
			state.files.push_back(i);
		}
	}	
	
	// Success.
	return true;
}


int main(int argc, char** argv)
{ 	
	SquishState state;

	// Process args.
	std::vector<std::string> args(argv+1,argv+argc);
	if(!processArgs(args,state))
		return -1;

	// Good to go?
	if(!state.command)
		return -1;

	if(!state.command->execute(state))
		return -1;	

	return 0;
}
