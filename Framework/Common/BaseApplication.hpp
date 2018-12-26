#pragma once
#include "IApplication.hpp"

namespace Lark{
	class BaseApplication : implements IApplication
	{
	public:
		virtual int Initialize();
		virtual void Finalize();

		virtual void Tick();

		virtual bool IsQuit();
	protected:

		bool m_bQuit;
	};
}
