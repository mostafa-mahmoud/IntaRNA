
#ifndef OUTPUTHANDLERHUB_H_
#define OUTPUTHANDLERHUB_H_

#include "OutputHandler.h"

#include <list>

/**
 * Hub implementation of an OutputHandler that forwards each reported interaction
 * to each of the registered OutputHandler instances.
 *
 */
class OutputHandlerHub: public OutputHandler
{
protected:

	//! list of OutputHandler objects to forward to
	std::list< OutputHandler * > outList;

	//! whether or not the elements in outList are to be deleted when this
	//! object is destroyed
	bool deleteOutListOnDestruction;

public:

	/**
	 * construction
	 * @param deleteOutListOnDestruction whether or not the objects the hub is
	 *        forwarding to are to be deleted on destruction
	 */
	OutputHandlerHub( const bool deleteOutListOnDestruction = true );

	/**
	 * copy construction (will not delete the copied OutputHandler list!)
	 *
	 * @param toCopy the hub to copy
	 */
	OutputHandlerHub( const OutputHandlerHub & toCopy );


	/**
	 * destruction
	 */
	virtual ~OutputHandlerHub();


	/**
	 * Forwards a given RNA-RNA interaction to all registered OutputHandler
	 * instances.
	 *
	 * @param interaction the interaction to add
	 */
	virtual
	void
	add( const Interaction & interaction );

	/**
	 * Forwards a given RNA-RNA interaction range to all registered
	 * OutputHandler instances.
	 *
	 * @param range the interaction range to add
	 */
	virtual
	void
	add( const InteractionRange & range );

	/**
	 * Returns the maximal number of reported interactions among all handlers
	 * within forwarding list
	 * @return the maximal number of reported interactions among all handlers
	 */
	virtual
	size_t
	reported() const;


	/**
	 * Adds a new OutputHandler to the forwarding list.
	 * @param handler pointer to the handler to forward to
	 */
	virtual
	void
	addOutputHandler( OutputHandler * handler );

	/**
	 * Defines whether or not the OutputHandler instances this hub forwards to
	 * are deleted when this hub is deleted.
	 * @param deleteOnDestruction true: forward objects are deleted; false: not
	 */
	virtual
	void
	setDeleteOnDestruction( const bool deleteOnDestruction );

	/**
	 * Returns whether or not the OutputHandler instances this hub forwards to
	 * are deleted when this hub is deleted.
	 * @return true: forward objects are deleted; false: not
	 */
	virtual
	bool
	isDeleteOnDestruction() const;


	/**
	 * Copies the outList of toCopy but wont delete it later.
	 *
	 * NOTE: this (copy) hub will NOT delete the outList to avoid duplicated
	 * deletions. This should be done by the original hub (toCopy)
	 *
	 * @param toCopy the handler to make this a copy of
	 * @return the altered range object (*this)
	 */
	OutputHandlerHub &
	operator= ( const OutputHandlerHub & toCopy);


};


/////////////////////////////////////////////////////////////////////////

inline
OutputHandlerHub
::OutputHandlerHub( const bool deleteOnDestruction )
 : OutputHandler()
	, outList()
	, deleteOutListOnDestruction(deleteOnDestruction)
{
}

/////////////////////////////////////////////////////////////////////////

inline
OutputHandlerHub
::OutputHandlerHub( const OutputHandlerHub & toCopy )
 : OutputHandler()
	, outList( toCopy.outList )
	// dont delete on destruction (should be done by original) to avoid double cleanup
	, deleteOutListOnDestruction(false)
{
}

/////////////////////////////////////////////////////////////////////////

inline
OutputHandlerHub::
~OutputHandlerHub()
{
	// check if something is to be done
	if (isDeleteOnDestruction()) {
		// delete all handler
		for( auto it = outList.begin(); it != outList.end(); it++ ) {
			CLEANUP( (*it) );
		}
	}
}



/////////////////////////////////////////////////////////////////////////

inline
void
OutputHandlerHub::
addOutputHandler( OutputHandler * handler )
{
#if IN_DEBUG_MODE
	if (handler != NULL) {
		// check if already present
		for (auto it=outList.begin(); it!=outList.end(); it++) {
			if (handler == (*it)) {
				throw std::runtime_error("OutputHandlerHub::addOutputHandler() : repeated adding of the same OutputHandler pointer");
			}
		}
	}
#endif
	// add if not null
	if (handler != NULL) {
		outList.push_back(handler);
	}
}

/////////////////////////////////////////////////////////////////////////

inline
void
OutputHandlerHub::
add( const Interaction & inter )
{
	// forward to all in list
	for (auto it=outList.begin(); it!=outList.end(); it++) {
		(*it)->add(inter);
	}
}

/////////////////////////////////////////////////////////////////////////

inline
void
OutputHandlerHub::
add( const InteractionRange & range )
{
	// forward to all in list
	for (auto it=outList.begin(); it!=outList.end(); it++) {
		(*it)->add(range);
	}
}

/////////////////////////////////////////////////////////////////////////

inline
size_t
OutputHandlerHub::
reported() const
{
	size_t maxReported = 0;
	// get maximal number of reports among all handlers
	for (auto it=outList.begin(); it!=outList.end(); it++) {
		maxReported = std::min( maxReported, (*it)->reported() );
	}
	// return maximum
	return maxReported;
}

/////////////////////////////////////////////////////////////////////////

inline
void
OutputHandlerHub::
setDeleteOnDestruction( const bool deleteOnDestruction )
{
	this->deleteOutListOnDestruction = deleteOnDestruction;
}

/////////////////////////////////////////////////////////////////////////

inline
bool
OutputHandlerHub::
isDeleteOnDestruction() const
{
	return this->deleteOutListOnDestruction;
}

/////////////////////////////////////////////////////////////////////////

inline
OutputHandlerHub &
OutputHandlerHub::
operator= ( const OutputHandlerHub & toCopy)
{
	// clean up current content
	if (this->deleteOutListOnDestruction) {
		for (auto it=outList.begin(); it!=outList.end(); it++) {
			CLEANUP( (*it) );
		}
	}
	outList.clear();

	// copy data
	outList = toCopy.outList;
	// ensure this copy doesnt delete the list content
	deleteOutListOnDestruction = false;

	// modified object access
	return *this;
}

/////////////////////////////////////////////////////////////////////////

#endif /* OUTPUTHANDLERHUB_H_ */
