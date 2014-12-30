/*
  This source is part of the libosmscout library
  Copyright (C) 2013  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <osmscout/import/GenOptimizeAreaWayIds.h>

#include <osmscout/util/FileScanner.h>
#include <osmscout/util/FileWriter.h>
#include <osmscout/util/HashSet.h>

#include <osmscout/DataFile.h>

namespace osmscout {

  std::string OptimizeAreaWayIdsGenerator::GetDescription() const
  {
    return "Optimize ids for areas and ways";
  }

  bool OptimizeAreaWayIdsGenerator::ScanAreaIds(const ImportParameter& parameter,
                                                Progress& progress,
                                                const TypeConfig& typeConfig,
                                                NodeUseMap& nodeUseMap)
  {
    FileScanner scanner;
    uint32_t    dataCount=0;

    progress.SetAction("Scanning ids from 'wayarea.tmp'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "wayarea.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetAreaDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(dataCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    for (size_t current=1; current<=dataCount; current++) {
      uint8_t type;
      Id      id;
      Area    data;

      progress.SetProgress(current,dataCount);

      if (!scanner.Read(type) ||
          !scanner.Read(id) ||
          !data.Read(typeConfig,
                     scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      for (const auto& ring: data.rings) {
        OSMSCOUT_HASHSET<Id> nodeIds;

        if (!ring.GetType()->CanRoute()) {
          continue;
        }

        for (const auto id: ring.ids) {
          if (nodeIds.find(id)==nodeIds.end()) {
            nodeUseMap.SetNodeUsed(id);

            nodeIds.insert(id);
          }
        }
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    progress.SetAction("Scanning ids from 'relarea.tmp'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "relarea.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetAreaDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(dataCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    for (size_t current=1; current<=dataCount; current++) {
      uint8_t type;
      Id      id;
      Area    data;

      progress.SetProgress(current,dataCount);

      if (!scanner.Read(type) ||
          !scanner.Read(id) ||
          !data.Read(typeConfig,
                     scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      for (const auto& ring: data.rings) {
        OSMSCOUT_HASHSET<Id> nodeIds;

        if (!ring.GetType()->CanRoute()) {
          continue;
        }

        for (const auto id: ring.ids) {
          if (nodeIds.find(id)==nodeIds.end()) {
            nodeUseMap.SetNodeUsed(id);

            nodeIds.insert(id);
          }
        }
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    return true;
  }

  bool OptimizeAreaWayIdsGenerator::ScanWayIds(const ImportParameter& parameter,
                                               Progress& progress,
                                               const TypeConfig& typeConfig,
                                               NodeUseMap& nodeUseMap)
  {
    FileScanner scanner;
    uint32_t    dataCount=0;

    progress.SetAction("Scanning ids from 'wayway.tmp'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "wayway.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetWayDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(dataCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    for (size_t current=1; current<=dataCount; current++) {
      uint8_t type;
      Id      id;
      Way     data;

      progress.SetProgress(current,dataCount);

      if (!scanner.Read(type) ||
          !scanner.Read(id) ||
          !data.Read(typeConfig,
                     scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      if (!data.GetType()->CanRoute()) {
        continue;
      }

      OSMSCOUT_HASHSET<Id> nodeIds;

      for (const auto& id : data.ids) {
        if (nodeIds.find(id)==nodeIds.end()) {
          nodeUseMap.SetNodeUsed(id);

          nodeIds.insert(id);
        }
      }

      // If we have a circular way, we "fake" a double usage,
      // to make sure, that the node id of the first node
      // is not dropped later on, and we cannot detect
      // circular ways anymore
      if (data.ids.front()==data.ids.back()) {
        nodeUseMap.SetNodeUsed(data.ids.back());
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    return true;
  }

  bool OptimizeAreaWayIdsGenerator::CopyAreas(const ImportParameter& parameter,
                                              Progress& progress,
                                              const TypeConfig& typeConfig,
                                              NodeUseMap& nodeUseMap)
  {
    FileScanner scanner;
    FileWriter  writer;
    uint32_t    dataCount=0;
    uint32_t    wayAreaCount=0;
    uint32_t    relAreaCount=0;

    if (!writer.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                     "areas.tmp"))) {
      progress.Error(std::string("Cannot create '")+writer.GetFilename()+"'");
      return false;
    }

    writer.Write(dataCount);

    progress.SetAction("Copy data from 'wayarea.tmp' to 'areas.tmp'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "wayarea.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetAreaDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(wayAreaCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    for (size_t current=1; current<=wayAreaCount; current++) {
      uint8_t type;
      Id      id;
      Area    data;

      progress.SetProgress(current,wayAreaCount);

      if (!scanner.Read(type) ||
          !scanner.Read(id) ||
          !data.Read(typeConfig,
                     scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(wayAreaCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      for (auto& ring : data.rings) {
        OSMSCOUT_HASHSET<Id> nodeIds;

        for (auto& id : ring.ids) {
          if (!nodeUseMap.IsNodeUsedAtLeastTwice(id)) {
            id=0;
          }
        }
      }

      if (!writer.Write(type) ||
          !writer.Write(id) ||
          !data.Write(typeConfig,
                      writer)) {
        progress.Error(std::string("Error while writing data entry to file '")+
                       writer.GetFilename()+"'");

        return false;
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    progress.SetAction("Copy data from 'relarea.tmp' to 'areas.tmp'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "relarea.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetAreaDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(relAreaCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    for (size_t current=1; current<=relAreaCount; current++) {
      uint8_t type;
      Id      id;
      Area    data;

      progress.SetProgress(current,relAreaCount);

      if (!scanner.Read(type) ||
          !scanner.Read(id) ||
          !data.Read(typeConfig,
                     scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(relAreaCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      for (auto& ring : data.rings) {
        OSMSCOUT_HASHSET<Id> nodeIds;

        for (auto& id : ring.ids) {
          if (!nodeUseMap.IsNodeUsedAtLeastTwice(id)) {
            id=0;
          }
        }
      }

      if (!writer.Write(type) ||
          !writer.Write(id) ||
          !data.Write(typeConfig,
                      writer)) {
        progress.Error(std::string("Error while writing data entry to file '")+
                       writer.GetFilename()+"'");

        return false;
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    dataCount=wayAreaCount+relAreaCount;
    if (!writer.SetPos(0)) {
      return false;
    }

    if (!writer.Write(dataCount)) {
      return false;
    }

    if (!writer.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     writer.GetFilename()+"'");
      return false;
    }

    return true;
  }

  bool OptimizeAreaWayIdsGenerator::CopyWays(const ImportParameter& parameter,
                                             Progress& progress,
                                             const TypeConfig& typeConfig,
                                             NodeUseMap& nodeUseMap)
  {
    FileScanner scanner;
    FileWriter  writer;
    uint32_t    dataCount=0;

    progress.SetAction("Copy data from 'wayway.tmp' to 'ways.tmp'");

    if (!scanner.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                      "wayway.tmp"),
                                      FileScanner::Sequential,
                                      parameter.GetWayDataMemoryMaped())) {
      progress.Error(std::string("Cannot open '")+scanner.GetFilename()+"'");
      return false;
    }

    if (!scanner.Read(dataCount)) {
      progress.Error("Error while reading number of data entries in file");
      return false;
    }

    if (!writer.Open(AppendFileToDir(parameter.GetDestinationDirectory(),
                                     "ways.tmp"))) {
      progress.Error(std::string("Cannot create '")+writer.GetFilename()+"'");
      return false;
    }

    writer.Write(dataCount);

    for (size_t current=1; current<=dataCount; current++) {
      uint8_t type;
      Id      id;
      Way     data;

      progress.SetProgress(current,dataCount);

      if (!scanner.Read(type) ||
          !scanner.Read(id) ||
          !data.Read(typeConfig,
                     scanner)) {
        progress.Error(std::string("Error while reading data entry ")+
                       NumberToString(current)+" of "+
                       NumberToString(dataCount)+
                       " in file '"+
                       scanner.GetFilename()+"'");

        return false;
      }

      for (auto& id : data.ids) {
        if (!nodeUseMap.IsNodeUsedAtLeastTwice(id)) {
          id=0;
        }
      }

      if (!writer.Write(type) ||
          !writer.Write(id) ||
          !data.Write(typeConfig,
                      writer)) {
        progress.Error(std::string("Error while writing data entry to file '")+
                       writer.GetFilename()+"'");

        return false;
      }
    }

    if (!scanner.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     scanner.GetFilename()+"'");
      return false;
    }

    if (!writer.Close()) {
      progress.Error(std::string("Error while closing file '")+
                     writer.GetFilename()+"'");
      return false;
    }

    return true;
  }

  bool OptimizeAreaWayIdsGenerator::Import(const TypeConfigRef& typeConfig,
                                           const ImportParameter& parameter,
                                           Progress& progress)
  {
    progress.SetAction("Optimize ids for areas and ways");

    NodeUseMap nodeUseMap;

    if (!ScanAreaIds(parameter,
                        progress,
                        typeConfig,
                        nodeUseMap)) {
      return false;
    }

    if (!ScanWayIds(parameter,
                       progress,
                       typeConfig,
                       nodeUseMap)) {
      return false;
    }

    if (!CopyAreas(parameter,
                   progress,
                   typeConfig,
                   nodeUseMap)) {
      return false;
    }

    if (!CopyWays(parameter,
                    progress,
                    typeConfig,
                    nodeUseMap)) {
      return false;
    }

    return true;
  }
}

