/*
========================

Copyright (c) 2014 Vinyl Games Studio

	UICommand.cpp

		Created by: Vladyslav Dolhopolov
		Created on: 7/10/2014 11:17:33 AM

========================
*/
#include "UICommand.h"

using namespace vgs;

sf::Mutex			UICommand::s_mutex;
UICommand::CmdQueue	UICommand::s_netQueue;
UICommand::CmdQueue	UICommand::s_engQueue;

UICommand::UICommand(UICommandID id, unsigned int len, unsigned char* data)
	: m_id(id)
	, m_data(data)
	, m_length(len)
	, m_stream(nullptr)
{
}

UICommand::UICommand(UICommandID id, RakNet::BitStream& stream)
	: m_id(id)
	, m_data(nullptr)
	, m_length(0)
	, m_stream(new RakNet::BitStream())
{
	m_stream->Write(stream, stream.GetNumberOfBitsUsed());
}

UICommand::~UICommand()
{
}

void UICommand::AddNetCommand(const UICommand& cmd)
{
	s_mutex.lock();
	s_netQueue.push_back(cmd);
	s_mutex.unlock();
}

void UICommand::AddEngCommand(const UICommand& cmd)
{
	s_mutex.lock();
	s_engQueue.push_back(cmd);
	s_mutex.unlock();
}

void UICommand::SwapNet(CmdQueue& queue)
{
	s_mutex.lock();
	s_netQueue.swap(queue);
	s_mutex.unlock();
}

void UICommand::SwapEng(CmdQueue& queue)
{
	s_mutex.lock();
	s_engQueue.swap(queue);
	s_mutex.unlock();
}

int UICommand::SizeNet()
{
	s_mutex.lock();
	int size = s_netQueue.size();
	s_mutex.unlock();
	return size;
}

int UICommand::SizeEng()
{
	s_mutex.lock();
	int size = s_engQueue.size();
	s_mutex.unlock();
	return size;
}

void UICommand::Free()
{
	delete [] m_data;
	delete m_stream;
}