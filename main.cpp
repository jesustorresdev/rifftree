// Copyright (C) 2013 Jesús Torres <jmtorres@ull.es>
// Copyright (C) 2025 Pedro López-Cabanillas <plcl@users.sf.net>
// SPDX-License-Identifier: Apache-2.0

/*
 * main.cpp - RIFF tree structure viewer
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

#define USE_MMAP

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include "riff.h"

#define OUTPUT_IDENT_WIDTH 4

static uint8_t *g_buffer{nullptr};

//
// Alias for standard output and standard error based on QTextStream.
// We will use them instead of std::cout and std::cerr for convenience.
//

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

//
// Traverse a RIFF tree from specified chunk and print the structure
//

void traverseRiff(const riff::RiffList<>::Chunk* listChunk, int indentWidth)
{
    cout.setFieldWidth(15);
    cout << listChunk->offset(g_buffer);
    cout.setFieldWidth(indentWidth);
    cout << ' ';
    cout.setFieldWidth(0);
    cout << QString("%1(%2) ->")
                .arg(listChunk->typeToQString())
                .arg(listChunk->data->listTypeToQString())
         << " size=" << listChunk->size << Qt::endl;

    int childIndent = indentWidth + OUTPUT_IDENT_WIDTH;

    const riff::RiffChunk<>* child = listChunk->data->chunks;
    const void* end = listChunk->dataEnd();
    while (child < end) {
        if (child->hasTypeList()) {
            traverseRiff(child->castTo<riff::RiffList<> >(), childIndent);
        } else {
            cout.setFieldWidth(15);
            cout << child->offset(g_buffer);
            cout.setFieldWidth(childIndent);
            cout << ' ';
            cout.setFieldWidth(0);
            cout << child->typeToQString() << " size=" << child->size << Qt::endl;
        }
        child = child->nextChunk();
    }
}

//
// Main function
//

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName(QT_STRINGIFY(PROGRAM));
    QCoreApplication::setApplicationVersion(QT_STRINGIFY(VERSION));

    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "RIFF file");
    parser.process(app);

    // Retrieve command line arguments from Qt and parse options
    QStringList args = parser.positionalArguments();

    if (args.size() == 0) {
        cerr << parser.helpText();
        return 99;
    }

    // Map the RIFF file into memory
    QFile rifffile(args.first());
    if (! rifffile.open(QIODevice::ReadOnly)) {
        cerr << QString("%1: %2\n").arg(app.applicationName()).arg(rifffile.errorString());
        return 10;
    }

    // QFile::map doesn't allow options like MAP_HUGETLB, MAP_PRIVATE or MAP_LOCKED
    // but it is more portable between different operating systems than mmap().
    // Previously, we tried to use MAP_HUGETLB with mmap() syscall but it is only
    // valid for anonymous memory.
    g_buffer = rifffile.map(0, rifffile.size());
    if (g_buffer == NULL) {
        cerr << QString("%1: %2\n").arg(app.applicationName()).arg(rifffile.errorString());
        return 11;
    }

    riff::RiffChunk<> *chunk = reinterpret_cast<riff::RiffChunk<> *>(g_buffer);
    if (! chunk->hasTypeRiff()) {
        cerr << QString("%1: '%2' is not a valid RIFF file\n")
                    .arg(app.applicationName())
                    .arg(rifffile.fileName());
        return 20;
    }

    // Traverse the RIFF file and print its structure
    traverseRiff(chunk->castTo<riff::RiffList<> >(), 0);

    // Cleanup
    rifffile.unmap(g_buffer);
    rifffile.close();
}
