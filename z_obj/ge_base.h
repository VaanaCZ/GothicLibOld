#pragma once

#include <string>
#include <vector>

namespace GE
{
/*	// Reflections
	class bCPropertyObjectsingleton
	{
		std::string base;
		std::string name;
		std::vector<bCPropertyObjectTypeBase> properties;
	};

	class bCPropertyObjectTypeBase
	{
		std::string name;
		std::string type;
		size_t size;
		size_t offset;
	};

//#define DECLARE_CLASS(_C, _B)												\
//	static inline bTPropertyObject<_C, _B> ms_PropertyObjectInstance_ ##_C	

#define DECLARE_CLASS(_C, _B)												\
	static inline bTPropertyObject<_C, _B> ms_PropertyObjectInstance

	class bCPropertyObjectBase
	{

	};

	template <class C, class B> class bTPropertyObject : public bCPropertyObjectBase
	{

	};

//#define DECLARE_PROPERTY(_T, _N)									\
//	static inline bTPropertyTypeBase<typeid(this), _T> ms_PropertyMember_m_ ##_N

#define DECLARE_PROPERTY(_T, _N)									\
	static inline bTPropertyTypeBase<_T> ms_PropertyMember_m_ ##_N; \
	_T _N

	class bCPropertyTypeBase
	{

	};

//	template <class C, typename T> class bTPropertyTypeBase : public bCPropertyTypeBase
//	{
//
//	};

	template <typename T> class bTPropertyTypeBase : public bCPropertyTypeBase
	{

	};
	
	// Base types

	class bCObjectBase
	{
	public:


		bCObjectBase()			{}
		virtual ~bCObjectBase()	{}

	private:

	};

	class bCObjectRefBase : public bCObjectBase
	{
	public:

		DECLARE_CLASS(bCObjectBase, bCObjectRefBase);

		bCObjectRefBase()			{}
		virtual ~bCObjectRefBase()	{}

		DECLARE_PROPERTY(bool, IsPersistable);

	private:

	};*/
};
