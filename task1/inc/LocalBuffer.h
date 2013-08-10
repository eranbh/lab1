#ifndef __LOCAL_BUFFER_H__
#define __LOCAL_BUFFER_H__

#include <iostream>
#include <sstream>
#include "GenTypes.h"
#include "JException.h"

template <class TYPE>
class LocalBuffer
{
public:

	LocalBuffer(UQWordType xi_bufferSize)
	{
		m_buffer = new (std::nothrow) TYPE[xi_bufferSize];
		if (m_buffer == NULL) {
			std::stringstream msg;
			msg << "failed to allocate buffer. size: " << xi_bufferSize << std::endl;
			THROW_JEXCEPTION(0, msg.str(), "", NULL);
		}
	}

	~LocalBuffer(void) { delete m_buffer; }

	TYPE *getBuffer() { return m_buffer; }

private:
	TYPE *m_buffer;
};

#endif
