/*
========================

	Copyright (c) 2014 Vinyl Games Studio

	DebugGUILayer
		
	Created by: Karl Merten
	Created on: 5/14/2014 4:30:25 PM

	Last Modified: Vladyslav Dolhopolov

========================
*/
#include "DebugGUILayer.h"
#include "Engine.h"
#include "RunningInfo.h"
#include "DebugConsoleModule.h"
#include "InputModule.h"
#include "NetworkModule.h"

#include <sstream>

namespace vgs {

DebugGUILayer::DebugGUILayer( void ) :
	m_isVisible( false ),
	m_prevWidth( 0 )
{}

SFGWinPtr DebugGUILayer::CreateElements( WidgetMap& map ) {
	
	RunningInfo* args = Engine::GetInstance()->GetRunningInfo();

	m_window = sfg::Window::Create( sfg::Window::NO_STYLE );
	m_window->SetRequisition( sf::Vector2f( ( float )args->contextWidth, 0.0f ) );

	sfg::Box::Ptr renderInfoBox = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 1.0f );	
	
	m_renderLabel = sfg::Label::Create( "Thread 2 ( Render )" );
	m_renderLabel->SetAlignment( sf::Vector2f( 0.0f, 0.5f ) );
	
	m_renderFrameRateLabel = sfg::Label::Create( "Instant FPS: 30" );
	m_renderFrameRateLabel->SetAlignment( sf::Vector2f( 0.0f, 0.5f ) );

	m_renderAvgFrameRateLabel = sfg::Label::Create( "Average FPS: 30" );
	m_renderFrameRateLabel->SetAlignment( sf::Vector2f( 0.0f, 0.5f ) );
	
	renderInfoBox->Pack( m_renderLabel );
	renderInfoBox->Pack( m_renderFrameRateLabel );
	renderInfoBox->Pack( m_renderAvgFrameRateLabel );
	
	sfg::Box::Ptr logicInfoBox = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 1.0f );	

	m_logicLabel = sfg::Label::Create( "Thread 1 ( Logic )" );
	m_logicLabel->SetAlignment( sf::Vector2f( 0.0f, 0.5f ) );
	
	m_logicFrameRateLabel = sfg::Label::Create( "Instant FPS: 30" );
	m_logicFrameRateLabel->SetAlignment( sf::Vector2f( 0.0f, 0.5f ) );
	
	m_logicAvgFrameRateLabel = sfg::Label::Create( "Average FPS: 30" );
	m_logicAvgFrameRateLabel->SetAlignment( sf::Vector2f( 0.0f, 0.5f ) );
	
	logicInfoBox->Pack( m_logicLabel );
	logicInfoBox->Pack( m_logicFrameRateLabel );
	logicInfoBox->Pack( m_logicAvgFrameRateLabel );
	
	sfg::Box::Ptr debugInfoBox = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 10.0f );

	sfg::Box::Ptr frameRate = sfg::Box::Create( sfg::Box::Orientation::HORIZONTAL, 10.0f );
	sfg::Box::Ptr netInfoBox = sfg::Box::Create( sfg::Box::Orientation::VERTICAL );
	
	frameRate->Pack( logicInfoBox );
	frameRate->Pack( renderInfoBox );

	std::stringstream ss;
	ss << "Interpolation: " << NetworkModule::GetEnableInterpolation() ? "on" : "off";
	m_interpolation = sfg::Label::Create(ss.str().c_str());
	m_interpolation->SetAlignment(sf::Vector2f(0.0f, 0.5f));

	ss.str(std::string());
	ss << "Client delay: " << NetworkModule::GetClientDelay();
	m_clientDelay = sfg::Label::Create(ss.str().c_str());
	m_clientDelay->SetAlignment(sf::Vector2f(0.0f, 0.5f));

	netInfoBox->Pack(m_interpolation);
	netInfoBox->Pack(m_clientDelay);

	debugInfoBox->Pack(frameRate);
	debugInfoBox->Pack(netInfoBox);

	m_frameAligner = sfg::Fixed::Create();
	m_window->Add( m_frameAligner );

	m_frameAligner->Put( debugInfoBox, sf::Vector2f( args->contextWidth - frameRate->GetRequisition().x * 1.5f, 5.0f ) );
	
	m_consoleText = sfg::Entry::Create();
	m_consoleText->SetRequisition( sf::Vector2f( args->contextWidth - 50.0f, 0.0f ) );
	m_frameAligner->Put( m_consoleText, sf::Vector2f( 20.0f, args->contextHeight - 50.0f ) );

	m_window->Show( false );

	return m_window;	
}

void DebugGUILayer::SetLogicAverageFrameRateText( const char* str ) {
	m_logicAvgFrameRateLabel->SetText( str );	
}

void DebugGUILayer::SetLogicFrameRateText( const char* str ) {
	m_logicFrameRateLabel->SetText( str );
}

void DebugGUILayer::SetRenderAverageFrameRateText( const char* str ) {
	m_renderAvgFrameRateLabel->SetText( str );
}

void DebugGUILayer::SetRenderFrameRateText( const char* str ) {
	m_renderFrameRateLabel->SetText( str );
}

void DebugGUILayer::RefreshLayer( float dt ) {
	if ( InputModule::KeyPressedDown( sf::Keyboard::Tilde ) ) {
		m_isVisible = !m_isVisible;
		m_window->Show( m_isVisible );
		
		if ( !m_isVisible ) {
			m_consoleText->SetText( "" );
		}
	}

	// update fps labels
	m_FPSUpdateLabelMutex.lock();
	SetLogicAverageFrameRateText(m_FPSValuesToUpdate.lafr.c_str());
	SetLogicFrameRateText(m_FPSValuesToUpdate.lfr.c_str());
	SetRenderAverageFrameRateText(m_FPSValuesToUpdate.rafr.c_str());
	SetRenderFrameRateText(m_FPSValuesToUpdate.rfr.c_str());
	m_FPSUpdateLabelMutex.unlock();

	// update net labels
	m_netUpdateLabelMutex.lock();
	m_interpolation->SetText(m_netValuesToUpdate.inter.c_str());
	m_clientDelay->SetText(m_netValuesToUpdate.delay.c_str());
	m_netUpdateLabelMutex.unlock();

	if ( m_isVisible ) {
		GUILayer::RefreshLayer( dt );

		if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Return ) ) {
			if ( m_consoleText->GetText().getSize() > 0 ) {
				Engine::GetInstance()->GetModuleByType<DebugConsoleModule>( EngineModuleType::DEBUGCONSOLE )->ProcessCommand( m_consoleText->GetText().toAnsiString().c_str() );
				m_consoleText->SetText( "" );
			}
		} else if ( InputModule::KeyPressedDown( sf::Keyboard::Up ) ) {
			const char* const prevCommand = Engine::GetInstance()->GetModuleByType<DebugConsoleModule>( EngineModuleType::DEBUGCONSOLE )->GetPreviousCommand();
			if ( prevCommand ) {
				m_consoleText->SetText( prevCommand );
			}
		} else if ( InputModule::KeyPressedDown( sf::Keyboard::Down ) ) {
			const char* const prevCommand = Engine::GetInstance()->GetModuleByType<DebugConsoleModule>( EngineModuleType::DEBUGCONSOLE )->GetNextCommand();
			if ( prevCommand ) {
				m_consoleText->SetText( prevCommand );
			}
		}

		RunningInfo* args = Engine::GetInstance()->GetRunningInfo();
		if ( args->contextWidth != m_prevWidth ) {
			m_window->SetRequisition( sf::Vector2f( ( float )args->contextWidth, 0.0f ) );	
			m_frameAligner->GetChildren()[0]->SetPosition( sf::Vector2f( args->contextWidth - m_frameAligner->GetChildren()[0]->GetRequisition().x - 20.0f, 5.0f ) );
			m_frameAligner->GetChildren()[1]->SetPosition( sf::Vector2f( 20.0f, args->contextHeight - 50.0f ) );
			m_frameAligner->GetChildren()[1]->SetRequisition( sf::Vector2f( args->contextWidth - 50.0f, 0.0f ) );

			m_prevWidth = args->contextWidth;
		}
	}
}

void DebugGUILayer::SetUpdateMsg(const UpdateFPSLabelsMsg& msg)
{
	m_FPSUpdateLabelMutex.lock();
	m_FPSValuesToUpdate = msg;
	m_FPSUpdateLabelMutex.unlock();
}

void DebugGUILayer::SetUpdateNetMsg(const UpdateNetworkLabelsMsg& msg)
{
	m_netUpdateLabelMutex.lock();
	m_netValuesToUpdate = msg;
	m_netUpdateLabelMutex.unlock();
}

}