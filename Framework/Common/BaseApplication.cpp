#include "BaseApplication.hpp"

int Lark::BaseApplication::Initialize()
{
	m_bQuit = false;
	return 0;
}

void Lark::BaseApplication::Finalize()
{

}

void Lark::BaseApplication::Tick()
{

}

bool Lark::BaseApplication::IsQuit()
{
	return m_bQuit;
}
