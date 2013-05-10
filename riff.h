/*
 * riff.h - RIFF file format data types
 *
 *   Copyright 2013 Jesús Torres <jmtorres@ull.es>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RIFF_H
#define RIFF_H

#if defined QT_VERSION
#include <QString>
#endif

#include <stdint.h>
#include <string>

namespace riff
{

//
// Class RiffChunk
//
// Base data type of every RIFF chunk
//

#pragma pack(4)
template <typename T = uint8_t>
struct RiffChunk
{
    //
    // Chunk fields
    //

    uint32_t type;
    uint32_t size;
    T data[];

    //
    // Known types of chunks
    //

    static const uint32_t TYPE_RIFF = 0x46464952;
    static const uint32_t TYPE_LIST = 0x5453494C;
    static const uint32_t TYPE_INFO = 0x4F464E49;

    bool hasTypeRiff() const
    {
        return type == TYPE_RIFF;
    }

    bool hasTypeList() const
    {
        return type == TYPE_LIST;
    }

    std::string typeToStdString() const
    {
        return std::string(reinterpret_cast<const char*>(&type), sizeof(type));
    }

#if defined QT_VERSION
    QString typeToQString() const
    {
        return QString::fromLatin1(reinterpret_cast<const char*>(&type), sizeof(type));
    }
#endif /* QT_VERSION */

    //
    // Pointers to data section
    //

    T* begin()
    {
       return data;
    }

    const T* begin() const
    {
       return data;
    }

    T* end()
    {
        return reinterpret_cast<T*>(
                    reinterpret_cast<const uint8_t*>(data) + size);
    }

    const T* end() const
    {
        return reinterpret_cast<const T*>(
                    reinterpret_cast<const uint8_t*>(data) + size);
    }

    //
    // Pointer to the next chunk
    //
    // Chunks are aligned to 16-bit
    //

    template <typename C = uint8_t>
    RiffChunk<C>* next()
    {
        return reinterpret_cast<RiffChunk<C>*>(alignPointer(data + size, 2));
    }

    template <typename C = uint8_t>
    const RiffChunk<C>* next() const
    {
        return reinterpret_cast<const RiffChunk<C>*>(alignPointer(data + size, 2));
    }

    //
    // Type conversion to other types of chunk
    //

    template <typename C>
    C* castTo()
    {
        return reinterpret_cast<C*>(this);
    }

    template <typename C>
    const C* castTo() const
    {
        return reinterpret_cast<const C*>(this);
    }

protected:
    RiffChunk()
    {}

    void* alignPointer(const void* pointer, int alignment) const
    {
        uintptr_t p = reinterpret_cast<uintptr_t>(pointer);
        uintptr_t alignmentMinusOne = alignment - 1;
        return reinterpret_cast<void*>((p + alignmentMinusOne) & ~alignmentMinusOne);
    }
};

//
// Class RiffListFields
//
// Additional fields of a RIFF list chunk
//

#pragma pack(4)
template <typename T = uint8_t>
struct RiffListFields {
    uint32_t listType;
    RiffChunk<T> chunks[];
};

//
// Class RiffList
//
// Data type of list RIFF chunks
//

#pragma pack(4)
template <typename T = uint8_t>
struct RiffList : public RiffChunk<RiffListFields<T> >
{
    typedef RiffChunk<RiffListFields<T> > BaseType;     // Type of base class

    std::string listTypeToStdString() const
    {
        return std::string(
                    reinterpret_cast<const char*>(&BaseType::data[0].listType),
                sizeof(&BaseType::data[0].listType));
    }

#if defined QT_VERSION
    QString listTypeToQString() const
    {
        return QString::fromLatin1(
                    reinterpret_cast<const char*>(&BaseType::data[0].listType),
                sizeof(&BaseType::data[0].listType));
    }
#endif /* QT_VERSION */

    //
    // Pointer to data section
    //

    RiffChunk<T>* begin()
    {
       return BaseType::data[0].chunks;
    }

    const RiffChunk<T>* begin() const
    {
       return BaseType::data[0].chunks;
    }

    RiffChunk<T>* end()
    {
       return reinterpret_cast<RiffChunk<T>*>(BaseType::end());
    }

    const RiffChunk<T>* end() const
    {
        return reinterpret_cast<const RiffChunk<T>*>(BaseType::end());
    }

private:
    RiffList()
        : RiffChunk<T>()
    {}
};

} // namespace riff

#endif // RIFF_H
