#include "BitIO.h"
#include "Lzw.h"
#include "DebugLog.h"
#include "BasicTypes.h"
#include <memory>
#include <vector>

#define M_MAKE_MAGIC(s) (s[0] << 24)|(s[1] << 16) | (s[2] << 8) | (s[3])

class Archive
{
private:
	struct Record 
	{
		u32 			size;
		u32  			checksum;
		std::shared_ptr<Codec> 	codec;
		std::string 		filename;
		std::ios::off_type      offset;	
	};
	std::vector<Record> m_contents;

	enum { ArchiveMagic = M_MAKE_MAGIC("jas!"), RecordMagic = M_MAKE_MAGIC("rcrd"), VersionMaj = 0, VersionMin = 1, CodecId=M_MAKE_MAGIC("Lzw!") };

	bool readRecord(BitReader* r, Record& record)
	{
		u32 magic;
		if(!r->read(&magic) || RecordMagic == magic)
		{
			M_DEBUGTRACE("Record Magic number not found, bailing...");
			return false;
		}
		if(!r->read(&record.size))
		{
		
			M_DEBUGTRACE("Failed to read record size");
			return false;
		}
		if(!r->read(&record.checksum))
		{
			
			M_DEBUGTRACE("Failed to read checksum data");
			return false;
		}
		u32 codec;
		if(!r->read(&codec) || codec != CodecId)
		{
		
			M_DEBUGTRACE("Failed to read codec");
			return false;
		}

		// TODO: use factory to create correct codec, here.
		// hardcode for now to use Lzw, which is our only codec.
		record.codec = std::shared_ptr<Codec>(new Lzw());
		
		if(!r->read(&record.filename))
		{
			M_DEBUGTRACE("Failed to read filename");
			return false;		
		}

		// Success.
		return true; }

	bool readRecords(BitReader* r)
	{
		// Keep reading records until we can't read anymore.
		Record record;
		while(!r->endOfData() && readRecord(r,record))
			m_contents.push_back(record);	
		return !m_contents.empty();
	}

	bool readHeader(BitReader*r )
	{
		u32 magic;
		if(!r->read(&magic) || ArchiveMagic != magic)
		{
			M_DEBUGTRACE("Header magic number not found. Is this really a valid Archive?");
			return false;
		}
		u8 version_maj;
		if(!r->readBits(&version_maj,4) && version_maj != VersionMaj )
		{
			M_DEBUGTRACE("Major Version number failed: Read Version:" << version_maj << 
				     " Minimimum version required: " << VersionMaj);
			return false;
		}
		u8 version_min;
		if(!r->readBits(&version_min,4) || version_min > VersionMin)
		{
		
			M_DEBUGTRACE("Minor Version number failed: Read Version:" << version_min << 
				     " Minimimum version required: " << VersionMin);
			return false;
		}
	}

	void writeHeader(BitWriter* w)
	{
		// Write header.
		w->write<u32>(ArchiveMagic);
		w->writeBits<u32>(VersionMaj,4);
		w->writeBits<u32>(VersionMin,4);
	}

	void writeRecord(BitWriter* w, Record& r)
	{
		w->write<u32>(RecordMagic);
		w->write(r.size);
		w->write(r.checksum);
		w->write<u32>(CodecId);
		w->write(r.filename);
	}
	
	void writeRecords(BitWriter* w)
	{
		for(auto i:m_contents)
			writeRecord(w,i);
	}

protected:
	Archive()
	{}
public:
	static std::shared_ptr<const Archive> serialize(BitReader* reader)
	{
		std::shared_ptr<Archive> ptr(new Archive());
		if(!ptr->readHeader(reader))
			return nullptr;
		
		if(!ptr->readRecords(reader))
			return nullptr;		
			
		return ptr;
	}

	void serialize(BitWriter* writer)
	{
		// write out archive.
		writeHeader(writer);
		writeRecords(writer);
	}

	// Structure:
	// MAGIC ID.
	// Version number.
	// RECORD

	// Record:
	// Size 
	// CheckSum
	// Filename 
	//
	//
	// Filename

};


