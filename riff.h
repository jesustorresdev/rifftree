// Copyright (C) 2013 Jesús Torres <jmtorres@ull.es>
// Copyright (C) 2025 Pedro López-Cabanillas <plcl@users.sf.net>
// SPDX-License-Identifier: Apache-2.0

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

#if defined(QT_CORE_LIB)
#include <QString>
#endif

#include <cstdint>
#include <string>

namespace riff
{

template <typename D>
struct RiffList;

//
// Class RiffChunk
//
// Template which instantiate to a class of RIFF chunk according
// to the type of data stored.
//

constexpr int alignsize{sizeof(uint32_t)};

#pragma pack(push, alignsize)
template<typename D = uint8_t>
struct RiffChunk
{
    typedef RiffChunk<D> Type;

    //
    // Chunk fields
    //

    uint32_t type;
    uint32_t size;
    D data[1];

    //
    // Known types of chunks
    //

    static constexpr uint32_t TYPE_RIFF = 0x46464952;
    static constexpr uint32_t TYPE_LIST = 0x5453494C;
    static constexpr uint32_t TYPE_INFO = 0x4F464E49;

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
        return std::string(
                    reinterpret_cast<const char*>(&type), sizeof(type));
    }

#if defined(QT_CORE_LIB)
    QString typeToQString() const
    {
        return QString::fromLatin1(
                    reinterpret_cast<const char*>(&type), sizeof(type));
    }
#endif

    //
    // Get a pointer to the end of data section
    //

    void *dataEnd() { return reinterpret_cast<uint8_t *>(data) + size; }

    //
    // Get the offset to the beginning of the structure
    //

    const uintptr_t offset(uint8_t *baseptr) const
    {
        return reinterpret_cast<const uint8_t *>(&type) - baseptr;
    }

    const void *dataEnd() const { return reinterpret_cast<const uint8_t *>(data) + size; }

    //
    // Get a pointer to the next chunk (16-bit aligned)
    //

    template <typename C = uint8_t>
    RiffChunk<C>* nextChunk()
    {
        return reinterpret_cast<RiffChunk<C> *>(alignPointer(dataEnd(), sizeof(int16_t)));
    }

    template <typename C = uint8_t>
    const RiffChunk<C>* nextChunk() const
    {
        return reinterpret_cast<const RiffChunk<C> *>(alignPointer(dataEnd(), sizeof(int16_t)));
    }

    //
    // Type conversion to other types of chunk
    //

    template <typename C>
    RiffChunk<C>* castTo()
    {
        return reinterpret_cast<RiffChunk<C>*>(this);
    }

    template <typename C>
    const RiffChunk<C>* castTo() const
    {
        return reinterpret_cast<const RiffChunk<C>*>(this);
    }

private:
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
// Data type of data section in chunks of type RIFF list
//

template<typename D = uint8_t>
struct RiffList
{
    typedef RiffList<D> Type;
    typedef RiffChunk<RiffList<D> > Chunk;

    uint32_t listType;
    RiffChunk<D> chunks[1];

    std::string listTypeToStdString() const
    {
        return std::string(reinterpret_cast<const char*>(&listType),
                           sizeof(listType));
    }

#if defined(QT_CORE_LIB)
    QString listTypeToQString() const
    {
        return QString::fromLatin1(reinterpret_cast<const char *>(&listType), sizeof(listType));
    }
#endif

private:
    RiffList()
        : RiffList<D>()
    {}
};
#pragma pack(pop)

} // namespace riff

#endif // RIFF_H
