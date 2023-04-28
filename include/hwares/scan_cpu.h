/************************************************************************************
      
 *                 Copyright (C) 2021 - 2023, Barca, Inc. 
 
 *    Email: <opensource@barca.com>  GitHub: @BarcaWebCloud. 
 *    Project: BSCAN to scanner MotherBoards. CPU, Memory Ram, SO and more
 
 * This software is licensed as described in the file COPYING, which                    
 * you should have received as part of this distribution. The terms                     
 * are also available at https://project-barca.github.io/docs/copyright.html.           
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell             
 * copies of the Software, and permit persons to whom the Software is                   
 * furnished to do so, under the terms of the COPYING file.                             
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY            
 * KIND, either express or implied.                                                      
 *
 **************************************************************************************/
#pragma once

#include <optional>
#include <string>
#include <vector>

namespace bscan {

  struct InstructionSet {
    bool _isHTT = false;
    bool _isSSE = false;
    bool _isSSE2 = false;
    bool _isSSE3 = false;
    bool _isSSE41 = false;
    bool _isSSE42 = false;
    bool _isAVX = false;
    bool _isAVX2 = false;

    bool _init_ = false;
  };

  class CPU {
    friend std::optional<CPU> getCPU(uint8_t socket_id);

   public:
    CPU() = default;
    CPU(int id);
    ~CPU() = default;

    std::string& modelName();
    std::string& vendor();
    int cacheSize_Bytes();
    int numPhysicalCores();
    int numLogicalCores();
    int maxClockSpeed_kHz();
    int regularClockSpeed_kHz();
    InstructionSet& instructionSet();

    static int currentClockSpeed_kHz();

    static std::string getModelName();
    static std::string getVendor();
    static int getNumPhysicalCores();
    static int getNumLogicalCores();
    static int getMaxClockSpeed_kHz();
    static int getRegularClockSpeed_kHz();
    static int getCacheSize_Bytes();

   private:
    std::string _modelName;
    std::string _vendor;
    int _numPhysicalCores = -1;
    int _numLogicalCores = -1;
    int _maxClockSpeed_kHz = -1;
    int _regularClockSpeed_kHz = -1;
    int _cacheSize_Bytes = -1;
    InstructionSet _instructionSet;

    int _id = 0;
  };

  class Socket {
   public:
    explicit Socket(uint8_t id);
    Socket(uint8_t id, const CPU& cpu);
    ~Socket() = default;

    class CPU& CPU();

   private:
    uint8_t _id;
    class CPU _cpu;
  };

  std::optional<CPU> getCPU(uint8_t socket_id);

  std::vector<Socket> getAllSockets();

};
