#ifndef __DEBUGGUILAYER_H__
#define __DEBUGGUILAYER_H__
/*
========================

Copyright (c) 2014 Vinyl Games Studio

	DebugGUILayer

		Description.

		Created by: Karl Merten
		Created on: 5/14/2014 4:30:25 PM

========================
*/
#include <SFGUI\Window.hpp>
#include <SFML\System\Mutex.hpp>
#include "GUILayer.h"

namespace vgs {

struct UpdateFPSLabelsMsg
{
	std::string rfr;
	std::string rafr;
	std::string lfr;
	std::string lafr;
};

struct UpdateNetworkLabelsMsg
{
	std::string inter;
	std::string delay;
};

class DebugGUILayer : public GUILayer {
public:						
							DebugGUILayer( void );
	virtual SFGWinPtr		CreateElements(WidgetMap& widgetMap)	override;

	virtual void			ConnectSignals( void ) {}

	void					SetUpdateMsg(const UpdateFPSLabelsMsg& msg);
	void					SetUpdateNetMsg(const UpdateNetworkLabelsMsg& msg);

protected:
	virtual void			RefreshLayer( float dt );
	virtual void			InitPools() {}

private:
	void					SetRenderFrameRateText( const char* str );
	void					SetRenderAverageFrameRateText( const char* str );
	void					SetLogicFrameRateText( const char* str );
	void					SetLogicAverageFrameRateText( const char* str );

	sfg::Label::Ptr			m_renderLabel;
	sfg::Label::Ptr			m_renderFrameRateLabel;
	sfg::Label::Ptr			m_renderAvgFrameRateLabel;

	sfg::Label::Ptr			m_logicLabel;
	sfg::Label::Ptr			m_logicFrameRateLabel;
	sfg::Label::Ptr			m_logicAvgFrameRateLabel;

	sfg::Label::Ptr			m_clientDelay;
	sfg::Label::Ptr			m_interpolation;

	sfg::Fixed::Ptr			m_frameAligner;

	sfg::Entry::Ptr			m_consoleText;
	bool					m_isVisible;

	UpdateFPSLabelsMsg		m_FPSValuesToUpdate;
	UpdateNetworkLabelsMsg	m_netValuesToUpdate;
	sf::Mutex				m_FPSUpdateLabelMutex;
	sf::Mutex				m_netUpdateLabelMutex;
	
	unsigned int			m_prevWidth;
};

}

#endif //__DEBUGGUILAYER_H__