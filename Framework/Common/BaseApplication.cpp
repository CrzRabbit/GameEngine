#include "BaseApplication.hpp"

int Neptune::BaseApplication::Initialize()
{
	m_bQuit = false;
	return 0;
}

void Neptune::BaseApplication::Finalize()
{

}

void Neptune::BaseApplication::Tick()
{

}

bool Neptune::BaseApplication::IsQuit()
{
	return m_bQuit;
}
