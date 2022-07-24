#pragma once

#include <fstream>
#include <thread>
#include <unordered_map>
#include <iostream>

#ifndef GOTHICLIB_DISABLE

namespace GothicLib
{
	//
	// Error logging system
	//

#ifdef _DEBUG
#define LOG_DEBUG(M)	GothicLib::Log::Message(GothicLib::Log::MESSAGE_DEBUG, (M))
#else
#define LOG_DEBUG(M)
#endif
#define LOG_INFO(M)		GothicLib::Log::Message(GothicLib::Log::MESSAGE_INFO, (M))
#define LOG_WARN(M)		GothicLib::Log::Message(GothicLib::Log::MESSAGE_WARN, (M))
#define LOG_ERROR(M)	GothicLib::Log::Message(GothicLib::Log::MESSAGE_ERROR, (M))

	class Log
	{
	public:

		Log() = delete;
		~Log() = delete;

		enum MESSAGE_LEVEL
		{
			MESSAGE_DEBUG,
			MESSAGE_INFO,
			MESSAGE_WARN,
			MESSAGE_ERROR
		};

		typedef void (*LogFunc)(MESSAGE_LEVEL, std::string);

		static void SetCallback(LogFunc);
		static void Message(MESSAGE_LEVEL, std::string);

	private:

		static inline LogFunc callback = nullptr;

	};

	//
	// Memory pool
	//

#define DECLARE_MEMORY_POOL(C)														\
	inline static MemoryPool<C> memPool;											\
														 							\
	void* operator new(size_t size)						 							\
	{													 							\
		return memPool.Alloc(1);						 							\
	}													 							\
														 							\
	void* operator new[](size_t size)					 							\
	{													 							\
		return memPool.Alloc(size / sizeof(C));			 							\
	}													 							\
														 							\
	void operator delete(void* p)						 							\
	{													 							\
		return memPool.Free((C*)p);					 								\
	}													 							\
														 							\
	void operator delete[](void* p, size_t size)		 							\
	{													 							\
		return memPool.Free((C*)p, size / sizeof(C));	 							\
	}

	template <class T> class MemoryPool
	{
	public:

		MemoryPool()	{}
		~MemoryPool()	{}

		struct Allocation
		{
			size_t	size	= 0;
			size_t	used	= 0;
			T*		block	= nullptr;
			bool*	slots	= nullptr;
		};

		size_t blockSize = 100;

		std::vector<Allocation> allocations;

		T* Alloc(size_t allocSize = 1)
		{
			// First to try find a free slot
			for (size_t i = 0; i < allocations.size(); i++)
			{
				Allocation& allocation = allocations[i];

				if (allocation.used + allocSize <= allocation.size)
				{
					// Find a free block of memory which suits the
					// needed criteria.
					size_t SubBlockOffset	= 0;
					size_t SubBlockSize = 0;

					for (size_t i = 0; i < allocation.size; i++)
					{
						if (!allocation.slots[i])
						{
							SubBlockSize++;

							if (SubBlockSize == allocSize)
							{
								break;
							}
						}
						else
						{
							SubBlockSize		= 0;
							SubBlockOffset	= i + 1;
						}
					}

					if (SubBlockSize == allocSize)
					{
						// Mark the block as used
						allocation.used += allocSize;

						for (size_t i = SubBlockOffset; i < SubBlockOffset + SubBlockSize; i++)
						{
							allocation.slots[i] = true;
						}

						return &allocation.block[SubBlockOffset];
					}
				}
				else
				{
					// Attempt expansion
					// todo: see if this slows things down
					const size_t oldSize = allocation.size;

					size_t newAllocSize = allocSize;

					if (newAllocSize < blockSize)
						newAllocSize = blockSize;

					T* expandedAlloc = (T*)_expand(allocation.block, sizeof(T) * (oldSize + newAllocSize));

					if (expandedAlloc == allocation.block)
					{
						allocation.size += newAllocSize;
						allocation.used += newAllocSize;
					
						bool* newSlots = new bool[oldSize + newAllocSize];
					
						memcpy(newSlots, allocation.slots, oldSize);
						memset(&newSlots[oldSize], 1, newAllocSize);
					
						delete[] allocation.slots;
						allocation.slots = newSlots;
										
						return &allocation.block[oldSize];
					}
				}
			}

			// If that fails, allocate a new one
			const size_t a = allocations.size();
			allocations.emplace_back();
			Allocation& allocation = allocations[a];

			size_t currBlockSize = allocSize > blockSize ? allocSize : blockSize;

			allocation.size		= currBlockSize;
			allocation.used		= allocSize;
			allocation.block	= (T*)malloc(sizeof(T) * currBlockSize);
			allocation.slots	= new bool[currBlockSize];
			
			memset(allocation.slots, 0, currBlockSize);

			for (size_t i = 0; i < allocSize; i++)
			{
				allocation.slots[i] = true;
			}

			return allocation.block;
		}

		void Free(T* ptr, size_t freeSize = 1)
		{
			for (size_t i = 0; i < allocations.size(); i++)
			{
				Allocation& allocation = allocations[i];

				// Find if the memory to be freed is within
				// the current's block's memory range.
				if (ptr >= allocation.block && ptr <= &allocation.block[allocation.size - 1])
				{
					size_t offset = ((size_t)ptr - (size_t)allocation.block) / sizeof(T);

					for (size_t i = offset; i < offset + freeSize; i++)
					{
						allocation.slots[i] = false;
					}

					allocation.used -= freeSize;

					// Check if the block is empty and can
					// be safely deleted.
					if (allocation.used == 0)
					{
						free(allocation.block);
						delete[] allocation.slots;

						allocations.erase(allocations.begin() + i);
					}

					return;
				}
			}
		}

	private:

	};
	
	//
	// File stream abstraction which allows reading
	// both from a file and a memory buffer.
	//

#define FILE_ARGS(V)		&(V), sizeof(V)

	class FileStream
	{
	public:

		FileStream()			{ iSubThreadId = std::this_thread::get_id(); }
		virtual ~FileStream()	{ Close(); }

		FileStream(const FileStream&) = delete;
		FileStream& operator=(const FileStream&) = delete;

		/*
			Stream creation / destruction
		*/

		bool			Open(std::string, char);				// File
		bool			Open(std::wstring, char);				// File
		bool			Open(char*, size_t, char, bool = true);	// Buffer
		bool			Open(FileStream*, uint64_t, uint64_t);	// Substream

		void			Close();

		/*
			Basic stream operations	
		*/

		bool			Read(void*, uint64_t);
		virtual bool	ReadString(std::string&);
		bool			ReadNullString(std::string&);
		bool			ReadLine(std::string&);

		void			Seek(uint64_t pos);
		uint64_t		Tell();
		uint64_t		TotalSize();

		inline bool		Error()		{ return error; }

	protected:

		virtual bool	OnOpen()	{ return true; }

		bool			error = false;

	private:

		void			ForkSubStream(FileStream*, uint64_t);

		enum STREAM_MODE
		{
			STREAM_MODE_READ,
			STREAM_MODE_WRITE,

			STREAM_MODE_NONE = -1
		};

		STREAM_MODE		mode = STREAM_MODE_NONE;

		// In
		enum STREAM_SOURCE
		{
			STREAM_SOURCE_RAWFILE,
			STREAM_SOURCE_BUFFER,
			STREAM_SOURCE_SUBSTREAM,

			STREAM_SOURCE_NONE = -1
		};

		STREAM_SOURCE	iSource = STREAM_SOURCE_NONE;

		std::ifstream	iFile;
		std::wstring	iPath;

		char*			iBuffer;
		bool			iDisposeBuffer;

		uint64_t		iTotalSize;
		uint64_t		iPosition;

		FileStream*		iSubStream;
		uint64_t		iSubOffset = 0;
		std::thread::id	iSubThreadId;

		// Out
		std::ofstream	oFile;

	};
};

#endif // !GOTHICLIB_DISABLE
