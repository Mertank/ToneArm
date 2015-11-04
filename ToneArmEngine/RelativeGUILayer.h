/*
========================

Copyright (c) 2014 Vinyl Games Studio

	RelativeGUILayer.h

		Created by: Vladyslav Dolhopolov
		Created on: 6/25/2014 2:51:00 PM

========================
*/
#ifndef __RELATIVEGUILAYER_H__
#define __RELATIVEGUILAYER_H__

#include "GUILayer.h"

namespace vgs
{
	/*
	========================

		RelativeGUILayer

			Keeps layer's window relative to window size.
			m_relativePosition is [0.f, 1.f]

			Created by: Vladyslav Dolhopolov
			Created on: 6/25/2014 2:51:00 PM

	========================
	*/
	class RelativeGUILayer : public GUILayer
	{
	public:
						RelativeGUILayer();
		virtual void	RefreshLayer(float dt) override;

		void			SetRelativePosition(const sf::Vector2f& vec);
		sf::Vector2f&	GetRelativePosition()							{ return m_relativePosition; }

	protected:
		sf::Vector2f	m_relativePosition;
		unsigned int	m_contextWidth;
		unsigned int	m_contextHeight;

	};

} // namespace vgs

#endif __RELATIVEGUILAYER_H__