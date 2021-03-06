#ifndef SIMPLEDATAOBJECT_HPP
#define SIMPLEDATAOBJECT_HPP

#include "DataObject.hpp"
#include "Access/Access.hpp"

namespace fast {

// Workaround for data types with comma (,)
template<typename T> struct argument_type;
template<typename T, typename U> struct argument_type<T(U)> { typedef U type; };

// A macro for creating new simple data objects
#define FAST_SIMPLE_DATA_OBJECT(NAME, DATA_TYPE)                                            \
    class NAME : public SimpleDataObject<argument_type<void(DATA_TYPE)>::type > {                                      \
	FAST_OBJECT(NAME)                                                                       \
public:                                                                                     \
    typedef DataAccess<argument_type<void(DATA_TYPE)>::type >::pointer access;                                \
private:                                                                                    \
	NAME() {};                                                                              \
};                                                                                          \

// Forward declarations
template <class DataType>
class DataAccess;

template <class DataType, typename AccessObject = DataAccess<DataType> >
class SimpleDataObject;


template <class DataType>
class DataAccess {
public:
    DataAccess(DataType* data, SharedPointer<SimpleDataObject<DataType> > dataObject);
    DataType getData();
    void setData(const DataType& data);
    void release();
    ~DataAccess();

    typedef UniquePointer<DataAccess<DataType> > pointer;
protected:
    DataType* mData;
    SharedPointer<SimpleDataObject<DataType> > mDataObject;
};


template <class DataType>
DataAccess<DataType>::DataAccess(DataType *data, SharedPointer<SimpleDataObject<DataType> > dataObject) {
    mData = data;
    mDataObject = dataObject;
}

template <class DataType>
DataType DataAccess<DataType>::getData() {
    return *mData;
}

template <class DataType>
void DataAccess<DataType>::setData(const DataType &data) {
    *mData = data;
}

template <class DataType>
void DataAccess<DataType>::release() {
    mDataObject->accessFinished();
}

template <class DataType>
DataAccess<DataType>::~DataAccess() {
    release();
}



template <class DataType, typename AccessObject>
class SimpleDataObject : public DataObject {
public:
    void create(DataType data);
    typename AccessObject::pointer getAccess(accessType type);
protected:
    SimpleDataObject();

    virtual void free(ExecutionDevice::pointer device) override;

    virtual void freeAll() override;

    DataType mData;

private:
    // AccessObject needs to be friends with SimpleDataObject, so that it can reach the accessFinished method
    friend AccessObject;
};


template <class DataType, class AccessObject>
void SimpleDataObject<DataType, AccessObject>::free(ExecutionDevice::pointer device) {

}

template <class DataType, class AccessObject>
void SimpleDataObject<DataType, AccessObject>::freeAll() {

}

template <class DataType, class AccessObject>
void SimpleDataObject<DataType, AccessObject>::create(DataType data) {
    mData = data;
}

template <class DataType, class AccessObject>
typename AccessObject::pointer SimpleDataObject<DataType, AccessObject>::getAccess(accessType type) {

    blockIfBeingWrittenTo();

    if(type == ACCESS_READ_WRITE) {
    	blockIfBeingAccessed();
    	{
    		boost::lock_guard<boost::mutex> lock(mDataIsBeingWrittenToMutex);
            mDataIsBeingWrittenTo = true;
    	}
        updateModifiedTimestamp();
    }


    {
        boost::lock_guard<boost::mutex> lock(mDataIsBeingAccessedMutex);
        mDataIsBeingAccessed = true;
    }

    typename AccessObject::pointer accessObject(new AccessObject(&mData, mPtr.lock()));
    return std::move(accessObject);
}

template <class DataType, class AccessObject>
SimpleDataObject<DataType, AccessObject>::SimpleDataObject() {

}



} // end namespace

#endif //SIMPLEDATAOBJECT_HPP
