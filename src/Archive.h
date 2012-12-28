#include "BitIO.h"
#include "Lzw.h"
#include "DebugLog.h"
#include "BasicTypes.h"
#include <memory>
#include <vector>
#include <algorithm>

#define M_MAKE_MAGIC(s) (s[3] << 24)|(s[2] << 16) | (s[1] << 8) | (s[0])

class Archive
{
private:
	struct Record 
	{
		u32 			size;
		u32 			packedSize;
		u32  			checksum;
		std::shared_ptr<Codec> 	codec;
		std::string 		filename;
		std::ios::streamoff 	offset;	
	};
	std::vector<Record> 		m_contents;
	std::shared_ptr<BitWriter> 	m_writer;
	std::shared_ptr<BitReader> 	m_reader;

	enum {  ArchiveMagic 	= M_MAKE_MAGIC("jas!"), 
		RecordMagic 	= M_MAKE_MAGIC("rcrd"), 
		CodecId 	= M_MAKE_MAGIC("lzw!"), 
		VersionMaj 	= 0, 	
		VersionMin 	= 1
	};

	bool readRecord(BitReader* r, Record& record) const
	{
		//w->write<u32>(RecordMagic);   // 4
		//w->write<u32>(r.size);        // 4
		//w->write<u32>(r.packedSize);  // 4
		//w->write<u32>(r.checksum);    // 4
		//w->write<u32>(CodecId);       // 4
		//w->write(r.filename);	        // []	

		std::streamoff header_pos = r->tell();
		(void)header_pos;	
	
		u32 magic;
		if(!r->read(&magic) || RecordMagic != magic)
		{
			M_DEBUGTRACE("Record Magic number not found, bailing after reading 0x" << std::hex << magic << std::dec);
			return false;
		}
		if(!r->read(&record.size))
		{
			M_DEBUGTRACE("Failed to read record size");
			return false;
		}
		if(!r->read(&record.packedSize))
		{
			M_DEBUGTRACE("Failed to read packed size");
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

		// Data starts here.
		record.offset = r->tell();

		// Seek over it.
		std::streamoff off = (r->tell() + record.packedSize);
		r->seek(off);
			
		// TODO: seek manually here peeking for the header.

		M_DEBUGTRACE(" read: " << record.filename <<
			     " size: " << record.size <<
  			     " packedSize: " << record.packedSize <<
			     " header @ offset: " << header_pos <<
			     " data @ offset: " << record.offset <<
			     " seeking to pos: " << off 	     
		);
	
		// Success.
		return true; }

	bool readRecords(BitReader* r)
	{
		// Keep reading records until we can't read anymore.
		Record record;
		while(!r->endOfData() && readRecord(r,record))
		{
			m_contents.push_back(record);	
			r->roundUp();
		}
		return !m_contents.empty();
	}

	bool readHeader(BitReader*r )
	{
		u32 magic;
		if(!r->read(&magic) || ArchiveMagic != magic)
		{
			M_DEBUGTRACE("Header magic number not found. Is this really a valid Archive? : Read: " << magic);
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
		return true;
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
		//M_DEBUGTRACE("writing record " << r.filename << " at offset " << w->tell());
		w->write<u32>(RecordMagic);
		w->write(r.size);
		w->write(r.packedSize);
		w->write(r.checksum);
		w->write<u32>(CodecId);
		w->write(r.filename);		
	}
	
protected:
	Archive(std::shared_ptr<BitReader> r)
		: m_reader(r)
	{}
	Archive(std::shared_ptr<BitWriter> w)
		: m_writer(w)
	{}
public:
	~Archive() 
	{
	}

	struct Info
	{
		std::string 	filename;
		size_t  	packedSize;
		size_t 		size;
	};

	// Factory.
	static std::shared_ptr<Archive> create(std::shared_ptr<BitWriter> w)
	{
		std::shared_ptr<Archive> ptr(new Archive(w));
		ptr->writeHeader(w.get());
		return ptr;
	}

	static std::shared_ptr<const Archive> load(std::shared_ptr<BitReader> reader)
	{
		std::shared_ptr<Archive> ptr(new Archive(reader));
		if(!ptr->readHeader(reader.get()))
			return nullptr;
		
		// Try and read some records. Archive might be empty.
		ptr->readRecords(reader.get());
			
		return ptr;
	}

	void add(std::string filename,BitReader* reader)
	{
		//M_DEBUGTRACE("adding file..." << filename);
		
		// Remember where we are.
		std::ios::streamoff srcStart = reader->tell();
		std::ios::streamoff header = m_writer->tell();
				
		// Write blank record.
		Record r;
		r.codec 	= std::shared_ptr<Codec>(new Lzw());
		r.filename 	= filename;
		writeRecord(m_writer.get(),r);

		// Remember where data is to start.
		r.offset 	= m_writer->tell();

		// Write actual data
		r.codec->encode(reader,m_writer.get());

		std::streamoff dataEnd = m_writer->tell();
		r.packedSize 	= dataEnd-r.offset;

		// Rewind to write header and back.
		m_writer->seek(header);
		r.size 		= reader->tell() - srcStart;			
		writeRecord(m_writer.get(),r);	
		
		M_DEBUGTRACE("writing " << r.filename << " size: " << r.size << " packedSize: " << r.packedSize 
			<< " header @ " << header  
			<< " offsetData: " << r.offset << " seeking to: " << dataEnd);
		
		m_writer->seek(dataEnd);
	}

	bool get(std::string filename, std::shared_ptr<BitWriter> out) const
	{
		// Find it?
		auto i = std::find_if(m_contents.begin(), m_contents.end(), [&filename] (const Record& i) { return i.filename == filename; } );
		if(i== m_contents.end())
		{
			M_DEBUGTRACE("failed to find " << filename << " in the archive. ");
			return false;
		}

		// Seek to position of our record.
		m_reader->seek(i->offset);
	
		// Decode.
		std::streamoff b = out->tell();
		i->codec->decode(m_reader.get(),out.get());
		std::streamoff e = out->tell();

		// Decoded correct ammount of data?
		return e-b == i->size;
	}

	std::vector<Info> contents() const
	{
		std::vector<Info> files;	
		for(auto i:m_contents)
		{
			Info info;
			info.filename 	= i.filename;
			info.size     	= i.size;
			info.packedSize = i.packedSize;
			files.push_back(info);
		}
		return files;
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


