#ifndef MESSAGESTORE_IODEVICES_H
#define MESSAGESTORE_IODEVICES_H

/*
* this header contains our representaion of an IO device.
* there can, essentially, be 2 types of an IO operations:
* 1. reading - handles reading a stream of data off of
*    a device.
* 2. writing - handles an output stream that an application
*    layer code emits on to a device
* any new device we add, needs to adhere to this interface.
*/
#include <cstdint>
#include <any>

namespace framework {


/*
* this class represents the interface for a new device.
* please adhere to the following guidelines:
* 1. not all of the apis in this interface need to be impl'ed [pure].
*    one can implement a subset of them according to her needs.
*    however calling an unimp'ed api is a hard error.
* 2. any data moving in or out of the interface has to be done
*    in a way which avoids copying. however, no attempt is done
*    to enforce it here, as the apis excepts *any* type of DataChunk
*    that the impl chooses.
* 3. the integral type mentioned in each api is in bytes!
* 4. any implementaion needs to decide how it deals with partial reads / writes.
*    this is a property of the underlying device, and not this layer.
*    in the case where partial read / write is possible, proper error codes
*    indications need to be propogated back to the caller, in order to allow
*    a re-try / re-call
* 5. any initialization / release of resources or hardware needs to be done
*    in the c/d'tor of the impl layer. this keeps the details of a specific
*    device abstracted away, and provides RAII semantics.
* 6. no support is given to streams, as we are agnostic to will the device
*    support it. it is up to the higher layer to provide it, if needed
* */
class IODevice {

public:

    enum class IODeviceErrors;

    // these apis support simple read / write semantics
    // an error from them should be propagated via an exception
    // the user should define the exception type and capabilities

    virtual std::any readData(std::uint64_t){ throw ;}

    virtual void writeData(std::any, std::uint64_t){throw ;}


    // these apis are the classic error code based.
    // again the user has to define the errors she needs for
    // the special semantics of her device.

    virtual void readData(std::any, std::uint64_t, IODeviceErrors&){throw ;}

    virtual void writeData(std::any, std::uint64_t, IODeviceErrors&){throw ;}

    // .... many other IO related apis
};

} // namespace framework

#endif //MESSAGESTORE_IODEVICES_H
