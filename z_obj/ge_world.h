#pragma once

#include "ge_base.h"

namespace GothicLib
{
	namespace Genome
	{
		class gCProject : public eCProcessibleElement
		{
		public:

			GE_DECLARE_CLASS(gCProject, eCProcessibleElement);

			gCProject()				{ }
			virtual ~gCProject()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

			GE_DECLARE_PROPERTY(gCProject, std::string, ActiveWorld);

		private:

		};

		class gCWorld : public eCProcessibleElement
		{
		public:

			GE_DECLARE_CLASS(gCWorld, eCProcessibleElement);

			gCWorld()			{ }
			virtual ~gCWorld()	{ }

			virtual bool OnWrite(FileStream*);
			virtual bool OnRead(FileStream*);

		private:

		};
	};
};
