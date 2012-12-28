#include "BasicTypes.h"
#include "BitIO.h"
#include "Codec.h"
#include "Lzw.h"
#include "Archive.h"
#include "config.h"

#include <memory>
#include <iostream>
#include <exception>
#include <vector>
#include <iomanip>


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

			auto bigName = std::max_element(c.begin(),c.end(), 
				[](const Archive::Info& l, const Archive::Info& r)  { return l.filename.size() < r.filename.size(); } );
			
			auto bigSize = std::max_element(c.begin(),c.end(), 
				[](const Archive::Info& l, const Archive::Info& r)  { return std::to_string(l.size).size() < std::to_string(r.size).size(); } );
			
			auto bigPacked = std::max_element(c.begin(),c.end(), 
				[](const Archive::Info& l, const Archive::Info& r)  { return std::to_string(l.packedSize).size() < std::to_string(r.packedSize).size(); } );

			size_t nameWidth = bigName->filename.size() + 1;
			size_t sizeWidth = std::to_string(bigSize->size).size() + 1;
			size_t packedWidth = std::to_string(bigPacked->packedSize).size() + 1;

			for(auto j:c)
			{
				float ratio = (float)j.packedSize / j.size;
				float cents = ratio * 100.0f;
				std::cout << std::left << std::setw(nameWidth) << j.filename;
				std::cout << std::left << std::setw(sizeWidth) << std::to_string(j.size);
				std::cout << std::left << std::setw(packedWidth) << std::to_string(j.packedSize);
				std::cout << std::left << std::setw(2) << std::setprecision(2) << cents << "%"; 
				std::cout << std::endl;
			}
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
				std::cout << "extracting: " << j.filename << std::endl;
				auto w = std::shared_ptr<BitWriter>(new BitFileWriter(j.filename.c_str()));
				a->get(j.filename,w);

			}
		}
		return true;
	}
};


class HelpCommand : public ICommand
{
public:
	virtual bool execute(const SquishState& state) 
	{
		std::cout << " -x Extract Archive." << std::endl;
		std::cout << " -a Add files to a new Archive." << std::endl;
		std::cout << " -l List Archive." << std::endl;
		std::cout << " -h This help text." << std::endl;
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

		// help
		case 'h': 
		case '?': return std::shared_ptr<ICommand>(new HelpCommand());

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

	// Display banner.
	std::cout << PACKAGE_STRING << " - please send feeback to " << PACKAGE_BUGREPORT << std::endl;

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
