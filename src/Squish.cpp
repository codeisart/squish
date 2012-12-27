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
	std::string target;
};

class ListCommand : public ICommand
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


class AddCommand : public ICommand
{
public:
	virtual bool execute(const SquishState& state) 
	{
		if(state.target.empty())
		{
			std::cout << " add requires a target archive" << std::endl;
			return false;	
		}

		auto w = std::shared_ptr<BitWriter>(new BitFileWriter(state.target.c_str()));
		auto a = Archive::create(w);
		std::cout << "creating " << state.target << std::endl;
		for(auto i:state.files)
		{
			std::cout << "adding [" << i << "]" << std::endl;
			auto r = std::shared_ptr<BitReader>(new BitFileReader(i.c_str()));
			a->add(i,r.get());
		}
		return true;
	}
};


class ExtractCommand : public ICommand
{
public:
	virtual bool execute(const SquishState& state) 
	{
		for(auto i:state.files)
		{
			std::cout << "[" << i << "]" << std::endl;
			auto f = std::shared_ptr<BitReader>(new BitFileReader(i.c_str()));
			auto a = Archive::load(f);
			auto c = a->contents();
			
			for(auto j:c)
			{
				std::cout << "extracting: " << j << std::endl;
				auto w = std::shared_ptr<BitWriter>(new BitFileWriter(j.c_str()));
				a->get(j,w);

			}
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
		case 'x': return std::shared_ptr<ICommand>(new ExtractCommand());
	
		// add.
		case 'a': return std::shared_ptr<ICommand>(new AddCommand());

		// list contents.
		case 'l': return std::shared_ptr<ICommand>(new ListCommand());

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
		// Target file?
		else if('=' == i[0] && i.size() >1)
		{
			state.target = i.substr(1,std::string::npos);
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
