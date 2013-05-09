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
struct RiffChunk
{
    //
    // Chunk fields
    //

    uint32_t type;
    uint32_t size;
    uint8_t data[];

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

    template <typename T>
    T* begin()
    {
       return reinterpret_cast<T*>(data);
    }

    template <typename T>
    const T* begin() const
    {
       return reinterpret_cast<const T*>(data);
    }

    template <typename T>
    T* end()
    {
        return reinterpret_cast<T*>(data + size);
    }

    template <typename T>
    const T* end() const
    {
        return reinterpret_cast<const T*>(data + size);
    }

    //
    // Pointer to the next chunk
    //

    RiffChunk* next()
    {
        uintptr_t align = 2;    // chunks are aligned to 16-bit
        uintptr_t alignMinusOne = align - 1;
        uintptr_t p = reinterpret_cast<uintptr_t>(data + size);
        return reinterpret_cast<RiffChunk*>((p + alignMinusOne) & ~alignMinusOne);
    }

    const RiffChunk* next() const
    {
        uintptr_t align = 2;    // chunks are aligned to 16-bit
        uintptr_t alignMinusOne = align - 1;
        uintptr_t p = reinterpret_cast<uintptr_t>(data + size);
        return reinterpret_cast<const RiffChunk*>((p + alignMinusOne) & ~alignMinusOne);
    }

    //
    // Type conversion to other types of chunk
    //

    template <typename T>
    T* castTo()
    {
        return static_cast<T*>(this);
    }

    template <typename T>
    const T* castTo() const
    {
        return static_cast<const T*>(this);
    }

protected:
    RiffChunk()
    {}
};

//
// Class RiffList
//
// Data type of list RIFF chunks
//

#pragma pack(4)
struct RiffList : public RiffChunk
{
    uint32_t listType;
    RiffChunk chunks[];

    std::string listTypeToStdString() const
    {
        return std::string(reinterpret_cast<const char*>(&listType),
                           sizeof(listType));
    }

#if defined QT_VERSION
    QString listTypeToQString() const
    {
        return QString::fromLatin1(reinterpret_cast<const char*>(&listType),
                                   sizeof(listType));
    }
#endif /* QT_VERSION */

    //
    // Pointer to data section
    //

    RiffChunk* begin()
    {
       return chunks;
    }

    const RiffChunk* begin() const
    {
       return chunks;
    }

private:
    RiffList()
        : RiffChunk()
    {}
};

} // namespace riff

#endif // RIFF_H
