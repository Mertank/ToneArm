#pragma once

#include <SFGUI/Config.hpp>
#include <SFGUI/Signal.hpp>
#include <memory>

namespace sfg {

/** Base class for all Widget, Adjustment and Tooltip objects.
 */
class SFGUI_API Object {
	public:
		typedef std::shared_ptr<Object> Ptr; //!< Shared pointer.
		typedef std::shared_ptr<const Object> PtrConst; //!< Shared pointer.

		/** Ctor.
		 */
		Object(){}

		/** Dtor.
		 */
		virtual ~Object(){}

		/// @cond
		// Fix for VS2013 not supporting = default move members.
#if defined( _MSC_VER )
		/** Deleted Copy Ctor.
		 */
		Object( const Object& ){}

		/** Deleted Copy Assignment.
		 */
		Object& operator=( const Object& ){}
#else
		/** Move Ctor.
		 */
		Object( Object&& ){}

		/** Move Assignment.
		 */
		Object& operator=( Object&& ){}
#endif
		/// @endcond

		/** Request a reference to a specific signal for this object.
		 * @param id Requested signal id.
		 * @return Reference to requested signal.
		 */
		Signal& GetSignal( Signal::SignalID& id );

	protected:
		/** Get the signal container.
		 * @return Reference to the signal container.
		 */
		SignalContainer& GetSignals();

	private:
		// Signals.
		SignalContainer m_signals;
};

}
